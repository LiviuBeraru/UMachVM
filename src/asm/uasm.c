#include <stdio.h>
#include <stdlib.h> // exit
#include <ctype.h>
#include <string.h> // strchr
#include <unistd.h> // getopt
#include "uasm.h"
#include "asm_labels.h"
#include "asm_data.h"
#include "strings.h"
#include "command.h"
#include "asm_formats.h"

static const char comment       = '#';
static const char label_sep     = ':';
static const char quotation     = '"';
static const char whitespace[]  = " \t\n";
static const char data_mark[]   = ".data";
static const char string_mark[] = ".string";
static const char number_mark[] = ".number";


char *outputname  = "u.out";
int   line_number = 0;
char *current_filename = "somefile";
int   current_offset = 0;

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
        fprintf(stderr, "No files specified\n");
        exit(1);
    }

    /* parse command line options */
    opterr = 0; // suppress getopt error messages
    int c = 0;
    while ((c = getopt(argc, argv, "o:")) != -1) {
        switch(c) {
            case 'o':
                outputname = optarg;
                break;
            case '?':
                fprintf(stderr, "Unknown option: -%c\n", optopt);
                break;
            default:
                abort();
        }

    }

    int cmd_count = 0;
    int i;
    FILE *f;

    /* first run over the files: collect labels */
    for (i = optind; i < argc; i++) {
        if ((f = fopen(argv[i], "r")) == NULL) {
            perror(outputname);
            goto clean;
        }

        cmd_count += collect_code_labels(f); // reads until .data
        if (collect_data_labels(f) == -1) {
            fclose(f);
            goto clean;
        }
        fclose(f);
    }

    /* associate data labels with offsets based on
     * how many commands we read so far */
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
        f = fopen(argv[i], "r"); // don't check again for errors
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
    char *white = NULL;   // pointer to the first whitespace of line
    char *lab_sep = NULL; // pointer to the label separator
    int  cmd_counter = 0; // how many commands did we read

    rewind(file); // set the read position at the begin of file
    while ((line = next_line(file, 0)) != NULL) {
        white = strpbrk(line, whitespace);
        if (white) {
            *white = '\0';
        }
        if (strcasecmp(line, data_mark) == 0) {
            // don't parse beyond the data mark
            break;
        }
        lab_sep = strchr(line, label_sep);
        if (lab_sep) {
            /* we have found a label */
            *lab_sep = '\0';
            label_insert_name(line);
        } else {
            /* not a label, must be a command */
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
            fprintf(stderr, "No label for %s provided\n", type);
            return -1;
        }

        if ((content = strtok(NULL, "\n")) == NULL) {
            fprintf(stderr, "No content for %s provided\n", type);
            return -1;
        }

        /* decide what to do depending on the type of the label */
        if (strcasecmp(type, string_mark) == 0) {
            if (collect_string_data(label, content) == -1) {
                return -1;
            }
        } else if (strcasecmp(type, number_mark) == 0) {
            if (collect_numeric_data(label, content) == -1) {
                return -1;
            }
        } else {
            fprintf(stderr, "No such data type: <%s>\n", type);
            return -1;
        }
    }

    return 0;
}

int collect_string_data(char *label, char *content)
{
    if (content[0] != quotation) {
        fprintf(stderr, "No leading quotation mark in string content <%s>\n", content);
        return -1;
    }
    content++; // skip quotation mark

    int content_len = strlen(content);
    if (content_len < 1 || content[content_len - 1] != quotation) {
        fprintf(stderr, "No trailing quotation mark in string content <%s>\n", content);
        return -1;
    }
    content[content_len-1] = '\0'; // delete quotation mark

    insert_data(label, content, content_len);

    return 0;
}

int collect_numeric_data(char *label, char *content)
{
    long number = 0;
    if (str_to_int(content, &number) == -1) {
        fprintf(stderr, "<%s> is not a number\n", content);
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
        fprintf(stderr, "No such command: <%s>\n", items[0]);
        return -1;
    }

    char labelbuffer[LABEL_LENGHT] = {'\0'};

    /* nullify instruction */
    memset(instruction, 0, 4);
    instruction[0] = cmd->opcode;

    /* if the command expects a label,
     * replace the label with an offset */

    if (cmd -> labeled) {
        int labeloffset = 0;
        if (itemcount < 2) {
            /* no label, just the command alone */
            fprintf(stderr, "%s, line %d:", current_filename, line_number);
            fprintf(stderr, "Command <%s> expects a label", cmd->opname);
            return -1;
        }

        if (label_get_offset(items[1], &labeloffset) == -1) {
            /* label was not set */
            fprintf(stderr, "%s, line %d: ", current_filename, line_number);
            fprintf(stderr, "Unknown label <%s>", items[1]);
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

        /* delete the comment character */
        char *p = strchr(buffer, comment);
        if (p) {
            *p = '\0';
        }

        if (skiplabels) {
            /* we should skip lines containing labels */
            p = strchr(buffer, label_sep);
            if (p) {
                /* found label separator */
                continue;
            }
        }

        p = buffer;
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
