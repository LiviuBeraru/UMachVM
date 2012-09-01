#include <stdio.h>
#include <stdlib.h> // exit
#include <string.h> // strchr
#include <ctype.h>  // isspace

#include "strings.h"
#include "asm_labels.h"
#include "asm_data.h"
#include "command.h"
#include "registers.h"
#include "memory.h"
#include "logmsg.h"


static const char comment = '#';
static const char quote = '"';
static const char data_mark[] = ".data";
static const char string_mark[] = ".string";
static const char number_mark[] = ".number";
static int dmark_len = 0;
static int smark_len = 0;
static int nmark_len = 0;

static int        cmd_offset = 0;
static char      *outname = "a.ux";
static char      *filename = NULL;
static int        lineno = 0;


static void collect_labels(FILE *file);
static int collect_data(FILE* file);
static int collect_string_data(char *line);
static int collect_numeric_data(char *line);
static int assemble_file(FILE *output);

/* assemble every instruction format */
static int assembleNUL(char **items, int n, uint8_t instruction[4]);
static int assembleNNN(char **items, int n, uint8_t instruction[4]);
static int assembleR00(char **items, int n, uint8_t instruction[4]);
static int assembleRNN(char **items, int n, uint8_t instruction[4]);
static int assembleRR0(char **items, int n, uint8_t instruction[4]);
static int assembleRRN(char **items, int n, uint8_t instruction[4]);
static int assembleRRR(char **items, int n, uint8_t instruction[4]);


void assemble_files(int argc, char** argv)
{
    FILE *output = fopen(outname, "w");
    if (output == NULL) {
        perror(outname);
        return;
    }
    dmark_len = strlen(data_mark);
    smark_len = strlen(string_mark);
    nmark_len = strlen(number_mark);

    int i;
    for (i = 0; i < argc; i++) {
        FILE *f = fopen(argv[i], "r");
        if (f == NULL) {
            perror(argv[i]);
            goto clean;
        }
        filename = argv[i];
        lineno = 0;
        collect_labels(f);
        if (collect_data(f) == -1) {
            /* some syntax error */
            fclose(f);
            goto clean;
        }
        
        fclose(f);
    }
    
    insert_data_labels(cmd_offset);

    cmd_offset = 0;
    for (i = 0; i < argc; i++) {
        logmsg(LOG_INFO, "Assembling %s...", argv[i]);
        filename = argv[i];
        lineno = 0;
        if (assemble_file(output) == -1) {
            break;
        }
    }
    
    write_data(output); // write programm data
    
clean:
    fclose(output);
    delete_data();
    labels_delete();
}


int assemble_file(FILE *output)
{
    FILE *input = fopen(filename, "r");
    if (input == NULL) {
        perror(filename);
        return -1;
    }

    char buffer[128] = {'\0'}; // this is where we read from file to
    char *line = NULL;         // begin of input, after skiping whitespace
    char labelbuffer[16] = {'\0'}; // translated labels go here

    char *items[8] = { NULL }; // after spliting the input, we store the items here
    int  itemcount = 0; // how many items we split the input into
    struct command *cmd = NULL;

    uint8_t instruction[4] = { 0 }; // 4 byte instruction, this is what we write to file
    /* which assembly function we call */
    int (*func)(char **items, int n, uint8_t instruction[4]);

    while (fgets(buffer, sizeof(buffer), input)) {
        lineno++;
        line = find_nonblank(buffer);
        if (line == NULL || line[0] == comment) {
            continue;
        }
        if (strncasecmp(data_mark, line, dmark_len) == 0) {
            /* we hit the data section */
            goto clean;
        }
        
        if (strchr(line, ':')) {
            // ignore label lines
            continue;
        }

        itemcount = split(line, byspace, items, 8);
        if (itemcount < 1) {
            continue;
        }

        cmd = command_by_name(items[0]);
        if (! cmd) {
            logmsg(LOG_ERR, "%s line %d:", filename, lineno);
            logmsg(LOG_ERR, "No such command: %s", items[0]);
            return -1;
        }

        /* if the command expects a label, search for that label */
        if (cmd->labeled) {
            int labeloffset = 0;
            if (itemcount < 2) {
                /* no label, just the command alone */
                logmsg(LOG_ERR, "%s line %d:", filename, lineno);
                logmsg(LOG_ERR, "Command <%s> expects a label");
                return -1;
            }

            if (label_get_offset(items[1], &labeloffset) == -1) {
                /* label was not set */
                logmsg(LOG_ERR, "%s line %d:", filename, lineno);
                logmsg(LOG_ERR, "Unknown label <%s>", items[1]);
                return -1;
            } else {
                labeloffset = labeloffset - cmd_offset;
                memset(labelbuffer, 0, sizeof(labelbuffer));
                sprintf(labelbuffer, "%d", labeloffset);
                items[1] = labelbuffer;
            }
        }

        memset(instruction, 0, 4);
        instruction[0] = cmd->opcode;

        switch(cmd->format) {
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
                logmsg(LOG_ERR, "!%s:%d: Unknown Command Format",
                       __func__, __LINE__);
                return -1;
        }

        if (func(items, itemcount, instruction)) {
            break;
        }

        fwrite(instruction, 1, 4, output);
        cmd_offset++;
    }
clean:
    fclose(input);
    return 0; // OK
}

void collect_labels(FILE *file)
{
    char buffer[128] = {'\0'};
    char *line = NULL;

    while (fgets(buffer, sizeof(buffer), file)) {
        lineno++;
        /* check if the line is empty or a comment
         * and if so ignore it */

        // ignore leading whitespace
        line = find_nonblank(buffer);
        if (line == NULL) {
            // only whitespace
            continue;
        }

        if (line[0] == comment) {
            // comment line
            continue;
        }
        
        if (strncasecmp(data_mark, line, dmark_len) == 0) {
            return;
        }
        
        /* here we have a non empty line */

        /* search for the colon character
         * (do we have a label?) */
        char *colon = strchr(line, ':');
        if (colon) {
            // label found, delete the colon
            *colon = '\0';
            colon--;
            // also delete every space before the colon
            while(isspace(*colon)) {
                *colon = '\0';
                colon--;
            }
            
            label_insert_name(line);
        } else {
            label_insert_offset(cmd_offset);
            cmd_offset++;
        }
    }
}


int collect_data(FILE *file)
{
    if (ferror(file) || feof(file)) {
        logmsg(LOG_ERR, "%s: cannot read %s", __func__, filename);
        return - 1;
    }

    char buffer[256] = { '\0' };
    char *line = NULL;

    while (fgets(buffer, sizeof(buffer), file)) {
        lineno++;
        line = buffer;
        // skip whitespace
        while (isspace(*line)) {
            line++;
        }
        if (*line == '\0' || *line == comment) {
            continue;
        }

        if (strncasecmp(string_mark, line, smark_len) == 0) {
            line = line + smark_len;
            if (collect_string_data(line) == -1) {
                return -1;
            }
            continue;
        }

        if (strncasecmp(number_mark, line, nmark_len) == 0) {
            line = line + nmark_len;
            if (collect_numeric_data(line) == -1) {
                return -1;
            }
            continue;
        }

        fseek(file, 0, SEEK_END); // move to EOF
        logmsg(LOG_ERR, "%s, line %d: Invalid line: %s", filename, lineno, line);
        return -1;
    }
    
    return 0;
}

int collect_string_data(char *line)
{
    // skip whitespace
    while (isspace(*line))
        line++;
    
    char *name = line;
    char *text = NULL;

    if (! isalnum(*name)) {
        logmsg(LOG_ERR, "%s, line %d: Wrong format: no label name provided", 
               filename, lineno);
        return -1;
    }
    
    while (isalnum(*line))
        line++;
    *line = '\0';
    line++;
    
    // find the first quote
    line = strchr(line, quote);
    if (line == NULL) {
        logmsg(LOG_ERR, "%s, line %d: Wrong format: no begin quotes", 
               filename, lineno);
        return -1;
    }
    line ++;
    text = line;
    
    // find the second quote
    line = strchr(line, quote);
    if (line == NULL) {
        logmsg(LOG_ERR, "%s, line %d: Wrong format: no closing quotes", 
               filename, lineno);
        return -1;
    }
    *line = '\0'; // delete the quotation mark
    
    insert_data(name, text, strlen(text) + 1); // +1 for '\0'

    return 0;
}

int collect_numeric_data(char *line)
{
    char *items[2] = {0};
    if (split(line, byspace, items, 2) < 2) {
        logmsg(LOG_ERR, "%s, line %d: Wrong format: try '%s <label> <number>'", 
               filename, lineno, number_mark);
        return -1;
    }
    
    long number = 0;
    if (str_to_int(items[1], &number) == -1) {
        logmsg(LOG_ERR, "%s, line %d: Wrong format: Not a number: %s", 
               filename, lineno, items[1]);
        return -1;
    }
    
    char num[4]; // little endian crazyness
    num[0] = (number >> 24);
    num[1] = (number >> 16);
    num[2] = (number >>  8);
    num[3] = (number >>  0);
    
    insert_data(items[0], num, 4);
    
    return 0;
}


int assembleNUL(char** items, int n, uint8_t instruction[4])
{
    return 0;
}


int assembleNNN(char** items, int n, uint8_t instruction[4])
{
    if (n != 2) {
        logmsg(LOG_ERR, "%s line %d:", filename, lineno);
        logmsg(LOG_ERR, "Command <%s> takes one argument", items[0]);
        return -1;
    }

    long number = 0;

    if (str_to_int(items[1], &number) == -1) {
        logmsg(LOG_ERR, "%s line %d, command <%s>:", filename, lineno, items[0]);
        logmsg(LOG_ERR, "Not a number: <%s>", items[1]);
        return -1;
    }

    instruction[1] = number >> 16;
    instruction[2] = number >> 8;
    instruction[3] = number;
    return 0;
}

int assembleR00(char** items, int n, uint8_t instruction[4])
{
    if (n != 2) {
        logmsg(LOG_ERR, "%s line %d:", filename, lineno);
        logmsg(LOG_ERR, "Command <%s> takes one argument", items[0]);
        return -1;
    }

    Register *r = NULL;
    r = get_register_byname(items[1]);
    if (! r) {
        logmsg(LOG_ERR, "%s line %d:", filename, lineno);
        logmsg(LOG_ERR, "Not a register: %s", items[1]);
        return -1;
    }
    instruction[1] = r->regno;
    return 0;
}

int assembleRNN(char** items, int n, uint8_t instruction[4])
{
    if ( n != 3) {
        logmsg(LOG_ERR, "%s line %d:", filename, lineno);
        logmsg(LOG_ERR, "Command <%s> takes 2 arguments", items[0]);
        return -1;
    }
    
    char *R  = items[1]; // Register name
    char *NN = items[2]; // Number or label

    Register *r = get_register_byname(R);
    if (! r) {
        logmsg(LOG_ERR, "%s line %d:", filename, lineno);
        logmsg(LOG_ERR, "Not a register: %s", R);
        return -1;
    }

    long number = 0;
    if (str_to_int(NN, &number) == -1) {
        /* the command argument is not a number
           is it perhaps a label? */
        int offset = 0;
        if (label_get_offset(NN, &offset) == -1) {
            logmsg(LOG_ERR, "%s line %d, command <%s>:", filename, lineno, items[0]);
            logmsg(LOG_ERR, "Undefined label: <%s>", NN);
            return -1;
        } else {            
            number = ITABLE_SIZE + (offset * 4);
        }
    }

    uint16_t num16 = number;
    instruction[1] = r->regno;
    instruction[2] = num16 >> 8;
    instruction[3] = num16;

    return 0;
}

int assembleRR0(char **items, int n, uint8_t instruction[4])
{
    if (n != 3) {
        logmsg(LOG_ERR, "%s line %d, command <%s>:", filename, lineno, items[0]);
        logmsg(LOG_ERR, "Command <%s> takes 2 arguments", items[0]);
        return -1;
    }

    Register *r1 = get_register_byname(items[1]);
    if (! r1) {
        logmsg(LOG_ERR, "%s line %d:", filename, lineno);
        logmsg(LOG_ERR, "Not a register: %s", items[1]);
        return -1;
    }
    Register *r2 = get_register_byname(items[2]);
    if (! r2) {
        logmsg(LOG_ERR, "%s line %d:", filename, lineno);
        logmsg(LOG_ERR, "Not a register: %s", items[2]);
        return -1;
    }

    instruction[1] = r1->regno;
    instruction[2] = r2->regno;

    return 0;
}

int assembleRRN(char **items, int n, uint8_t instruction[4])
{
    if (n != 4) {
        logmsg(LOG_ERR, "%s line %d, command <%s>:", filename, lineno, items[0]);
        logmsg(LOG_ERR, "Command <%s> takes 3 arguments", items[0]);
        return -1;
    }

    Register *r1 = get_register_byname(items[1]);
    if (! r1) {
        logmsg(LOG_ERR, "%s line %d:", filename, lineno);
        logmsg(LOG_ERR, "Not a register: %s", items[1]);
        return -1;
    }
    Register *r2 = get_register_byname(items[2]);
    if (! r2) {
        logmsg(LOG_ERR, "%s line %d:", filename, lineno);
        logmsg(LOG_ERR, "Not a register: %s", items[2]);
        return -1;
    }

    long number = 0;
    if (str_to_int(items[3], &number) == -1) {
        logmsg(LOG_ERR, "%s line %d, command <%s>:", filename, lineno, items[0]);
        logmsg(LOG_ERR, "Not a number: <%s>", items[3]);
        return -1;
    }

    uint8_t n8 = number;

    instruction[1] = r1->regno;
    instruction[2] = r2->regno;
    instruction[3] = n8;

    return 0;
}

int assembleRRR(char **items, int n, uint8_t instruction[4])
{
    if (n != 4) {
        logmsg(LOG_ERR, "%s line %d, command <%s>:", filename, lineno, items[0]);
        logmsg(LOG_ERR, "Command <%s> takes 3 register names", items[0]);
        return -1;
    }

    Register *r1 = get_register_byname(items[1]);
    if (! r1) {
        logmsg(LOG_ERR, "%s line %d:", filename, lineno);
        logmsg(LOG_ERR, "Not a register: %s", items[1]);
        return -1;
    }

    Register *r2 = get_register_byname(items[2]);
    if (! r2) {
        logmsg(LOG_ERR, "%s line %d:", filename, lineno);
        logmsg(LOG_ERR, "Not a register: %s", items[2]);
        return -1;
    }
    Register *r3 = get_register_byname(items[3]);
    if (! r3) {
        logmsg(LOG_ERR, "%s line %d:", filename, lineno);
        logmsg(LOG_ERR, "Not a register: %s", items[3]);
        return -1;
    }

    instruction[1] = r1->regno;
    instruction[2] = r2->regno;
    instruction[3] = r3->regno;

    return 0;
}
