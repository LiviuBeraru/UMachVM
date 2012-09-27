/**
 * @file uasm.c
 * @brief Main file of the UMach assembler
 *
 */
#include <stdio.h>
#include <stdlib.h> // exit
#include <ctype.h>
#include <string.h> // strchr
#include <unistd.h> // getopt
#include <stdarg.h> // va_list
#include "uasm.h"
#include "asm_labels.h"
#include "asm_data.h"
#include "strings.h"
#include "command.h"
#include "asm_formats.h"

/* String constants */
static const char comment       = '#';       /// comment character
static const char label_sep     = ':';       /// label separator
static const char quotation     = '"';
static const char whitespace[]  = " \t\n";
static const char data_mark[]   = ".data";   /// marks the data section
static const char string_mark[] = ".string"; /// marks string data
static const char integer_mark[] = ".int";   /// marks numeric data

/** Name of output file. */
static char *outputname  = "u.out";
/** Current line number in input file. Used for diagnostics. */
static int   line_number = 0;
/** Current input file name. Used for diagnostics in case of errors. */
static char *current_filename = NULL;
/** Current offset in the assembly file. */
static int   current_offset = 0;

/* Private functions */
static int collect_code_labels(FILE *file);
static int collect_data_labels(FILE *file);
static int collect_string_data(char *label, char *content);
static int collect_numeric_data(char *label, char *content);
static int assemble_file(FILE *input, FILE *output);
static int assemble_line(char *items[], int n, uint8_t instruction[4]);
static char *next_line(FILE *file, int skiplabels);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        /* We expect at least */
        fprintf(stderr, "No program files specified.\n");
        abort();
    }

    /* parse command line options */
    opterr = 0; // suppress getopt error messages
    int c = 0;  // current command line option
    /* parse the command line options */
    while ((c = getopt(argc, argv, "o:")) != -1) {
        switch(c) {
            case 'o': // output file
                outputname = optarg;
                break;
            default:
                if (optopt == 'o') {
                    fprintf(stderr,
                            "The option -o expects an output file name.\n");
                    abort();
                } else {
                    fprintf(stderr, "Unknown option: -%c.\n", optopt);
                }
                abort();
        }

    }
    /* how many commands did we read so far
     * (used for computing the offset of the data section) */
    int cmd_count = 0;
    int i;
    FILE *f; // input file

    /* first run over the files: collect labels */
    for (i = optind; i < argc; i++) {
        if ((f = fopen(argv[i], "r")) == NULL) {
            perror(outputname);
            goto clean;
        }
        current_filename = argv[i];
        line_number = 0;

        cmd_count += collect_code_labels(f); // this reads from f until .data
        if (collect_data_labels(f) == -1) {
            fclose(f);
            goto clean;
        }
        fclose(f);
    }

    /* associate data labels with 4 byte offsets based on
     * how many commands we read so far (data is stored after all commands) */
    translate_data_labels(cmd_count);

    /* open the ouput file */
    FILE *output = fopen(outputname, "w");
    if (output == NULL) {
        perror(outputname);
        goto clean;
    }

    current_offset = 0;
    /* second run over the files: assemble */
    for (i = optind; i < argc; i++) {
        f = fopen(argv[i], "r"); // don't check again for open errors

        current_filename = argv[i];
        line_number = 0;
        if (assemble_file(f, output) == -1) {
            fclose(f);
            goto clean;
        }
        fclose(f);
    }

    write_data(output);
    fclose(output);

clean:
    labels_delete();
    delete_data();
    return 0;
}

int collect_code_labels(FILE *file)
{
    char *line = NULL;    // input line
    char *white = NULL;   // pointer to whitespace inside the line
    char *lab_sep = NULL; // pointer to the label separator
    int  cmd_counter = 0; // how many commands did we read

    rewind(file); // set the read position at the begin of file
    while ((line = next_line(file, 0)) != NULL) {
        /* search for the label separator */
        lab_sep = strchr(line, label_sep);

        /* if we find the label separator, */
        if (lab_sep) {
            /* delete it before deleting other whitespace.
             * If we delete whitespace now, we might include
             * the label separator into the label name
             */
            *lab_sep = '\0';
            // now the line ends just before the label separator
            
            /* Now delete the first whitespace of the line. 
             * Note that this will ignore every other words after the first
             * label word -> labels must contain no whitespace
             */
            if ((white = strpbrk(line, whitespace)) != NULL) {
                *white = '\0';
            }

            /* insert the label name to the pending list of labels
             * these labels will wait for an offset */
            label_insert_name(line);
        } else {
            /* not a label, so look for the data mark because
             * we stop parsing for code labels if we find the data mark
             * first, delete the first whitespace.
             * note that in this if-branch we delete the very first whitespace.
             * in the other branch, we delete whitespace after deleting the
             * label separator; this adds support for whitespace between
             * the label separator and the label name.
             */
            if ((white = strpbrk(line, whitespace)) != NULL) {
                *white = '\0'; // after whitespace is now junk
                /* consider deleting not whitespace, but only the new line
                 * now you can write junk after ".data" */
            }
            if (strcasecmp(line, data_mark) == 0) {
                // don't parse beyond the data mark
                break;
            }
            
            /* This will associate any pending labels with the current offset
             * (which is just the command counter).
             * If there are no pending labels, this does nothing.
             * This is to support multilple labels pointing to the same command.
             */
            label_insert_offset(cmd_counter);
            cmd_counter++;
        }
    }

    return cmd_counter;
}

/* data labels have the syntax
 * <type> <label> <content> */
int collect_data_labels(FILE *file)
{
    if (feof(file) || ferror(file)) {
        /* nothing to do */
        return 0;
    }

    char *line = NULL;
    char *type = NULL;
    char *label = NULL;
    char *content = NULL;
    while ((line = next_line(file, 0)) != NULL) {
        /* split the input line into components */
        type = strtok(line, whitespace);

        if ((label = strtok(NULL, whitespace)) == NULL) {
            printerror("No label for %s provided", type);
            return -1;
        }

        if ((content = strtok(NULL, "\n")) == NULL) {
            printerror("No content for %s provided", type);
            return -1;
        }

        /* decide what to do depending on the type of the label */
        if (strcasecmp(type, string_mark) == 0) {
            if (collect_string_data(label, content) == -1) {
                return -1;
            }
        } else if (strcasecmp(type, integer_mark) == 0) {
            if (collect_numeric_data(label, content) == -1) {
                return -1;
            }
        } else {
            printerror("No such data type: <%s>", type);
            return -1;
        }
    }

    return 0;
}

int collect_string_data(char *label, char *content)
{
    if (content[0] != quotation) {
        printerror("No leading quotation mark in string content <%s>", content);
        return -1;
    }
    content++; // skip first quotation mark

    int content_len = strlen(content);
    if (content_len < 1 || content[content_len - 1] != quotation) {
        printerror("No trailing quotation mark in string content <%s>", content);
        return -1;
    }
    content[content_len-1] = '\0'; // delete second quotation mark

    insert_data(label, content, content_len);

    return 0;
}

int collect_numeric_data(char *label, char *content)
{
    long number = 0;
    content = str_trim(content);
    if (str_to_int(content, &number) == -1) {
        printerror("<%s> is not a number", content);
        return -1;
    }

    char num[4]; // little endian crazyness
    num[0] = (number >> 24);
    num[1] = (number >> 16);
    num[2] = (number >>  8);
    num[3] = (number >>  0);

    insert_data(label, num, 4);
    return 0;
}

int assemble_file(FILE *input, FILE *output)
{
    rewind(input); // read from begin

    char *line = NULL; // input line
    char *items[8]; // line split into items
    int nitems = 0; // number of items
    uint8_t instruction[4] = {0}; // assembled items

    while ((line = next_line(input, 1)) != NULL) { // 1 for skip label lines
        nitems = split(line, whitespace, items, 8);

        if (strcasecmp(items[0], data_mark) == 0) {
            // don't parse beyond the data mark
            break;
        }

        if (assemble_line(items, nitems, instruction) == -1) {
            /* error while assembling this line */
            return -1;
        } else {
            fwrite(instruction, 1, 4, output);
            current_offset++;
        }
    }

    return 0;
}

int assemble_line(char *items[], int itemcount, uint8_t instruction[4])
{
    struct command *cmd = command_by_name(items[0]);
    if (cmd == NULL) {
        printerror("No such command: <%s>", items[0]);
        return -1;
    }

    char labelbuffer[LABEL_LENGHT + 1] = {'\0'};

    /* nullify instruction */
    memset(instruction, 0, 4);
    instruction[0] = cmd->opcode;

    /* if the command expects a label,
     * replace the label with an offset */

    if (cmd -> labeled) {
        int labeloffset = 0;
        if (itemcount < 2) {
            /* no label, just the command alone */
            printerror("Command <%s> expects a label", cmd->opname);
            return -1;
        }

        if (label_get_offset(items[1], &labeloffset) == -1) {
            /* label was not set */
            printerror("Unset label <%s>", items[1]);
            return -1;
        } else {
            labeloffset = labeloffset - current_offset;
            memset(labelbuffer, 0, sizeof(labelbuffer));
            sprintf(labelbuffer, "%d", labeloffset);
            items[1] = labelbuffer;
        }

    }

    /* which assembly function we call */
    int (*func)(char **items, int n, uint8_t instruction[4]);
    switch (cmd->format) {
        case NUL:
            func = assembleNUL;
            break;
        case NNN:
            func = assembleNNN;
            break;
        case R00:
            func = assembleR00;
            break;
        case RNN:
            func = assembleRNN;
            break;
        case RR0:
            func = assembleRR0;
            break;
        case RRN:
            func = assembleRRN;
            break;
        case RRR:
            func = assembleRRR;
            break;
        default:
            return -1;
    }

    return func(items, itemcount, instruction);
}

void printerror(const char* format, ... )
{
    fprintf(stderr, "%s, line %d: ", current_filename, line_number);

    va_list al;
    va_start(al, format);
    vfprintf(stderr, format, al);
    fprintf(stderr, "\n");
    va_end(al);
}

/**
 * Read next line from file.
 *
 * @param file File to read from
 * @param skiplabels if label lines are to be skiped
 */
char *next_line(FILE *file, int skiplabels)
{
    if (feof(file) || ferror(file)) {
        return NULL;
    }
    static char buffer[256] = { '\0' };

    int found = 0;
    char *line = NULL;
    while (! found) {
        /* read a line */
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            // failed to read from file
            return NULL;
        }
        line_number++;

        /* delete the comment character */
        char *p = strchr(buffer, comment);
        if (p) {
            *p = '\0';
        }

        /* if we should skip labels and if we find the label separator ":",
         * then put the p pointer to the next character after
         * the label separator
         */
        if (skiplabels && (p = strchr(buffer, label_sep))) {
            p++; // skip label
        } else {
            /* don't skip labels or no label found,
             * so put the p pointer at the begin of the input line
             */
            p = buffer;
        }

        /* skip whitespace */
        while (isspace(*p)) {
            p++;
        }
        if (*p) {
            /* we didn't hit the end of the string
             * after skiping whitespace */
            found = 1;
            line = p;
        }
    }

    return line;
}
