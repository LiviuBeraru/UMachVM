#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assemble.h"
#include "uasm.h"
#include "symbols.h"
#include "str_func.h"
#include "commands.h"
#include "asm_formats.h"
#include "collect_data.h"

static const char    DATA_MARK[] = ".data";   // marks the data section
static const char  STRING_MARK[] = ".string"; // marks string data
static const char INTEGER_MARK[] = ".int";    // marks numeric data
static const char COMMENT_MARK   = '#';       // marks the beginning of a comment

static int assemble_pass_one(asm_context_t *cntxt, char *files[], int file_count);
static int assemble_pass_two(asm_context_t *cntxt, char *files[], int file_count,
                             FILE *fmapfile, FILE *debugfile);
static int assemble_file(asm_context_t *cntxt, FILE *infile, FILE *outfile, FILE *debugfile);
static int assemble_line(asm_context_t *cntxt, char *items[], int item_count, uint8_t instruction[4]);
static void assemble_data(FILE *outfile);
static int collect_jump_labels(asm_context_t *cntxt, FILE *file);
static int collect_data_labels(asm_context_t *cntxt, FILE *file);
static int collect_string_data(asm_context_t *cntxt, char *label, char *content);
static int collect_int_data(asm_context_t *cntxt, char *label, char *content);
static void free_dynamic_data(asm_context_t *cntxt);
static char *read_line(asm_context_t *cntxt, FILE *file);

int assemble(asm_context_t *cntxt, char *files[], int file_count) {
    const size_t outfile_name_len = strlen(cntxt->output_file);
    FILE *fmap_file = NULL, *debug_file = NULL;

    char *fmap_name  = malloc(sizeof(char) * (outfile_name_len + 8));
    char *debug_name = malloc(sizeof(char) * (outfile_name_len + 8));
    
    if (cntxt->gen_debuginf) {
        strcpy(fmap_name, cntxt->output_file);
        strcat(fmap_name, ".fmap");
        strcpy(debug_name, cntxt->output_file);
        strcat(debug_name, ".debug");

        if ((fmap_file = fopen(fmap_name, "w")) == NULL) {
            perror(fmap_name);
            goto cleanup;
        }

        if ((debug_file = fopen(debug_name, "w")) == NULL) {
            perror(debug_name);
            goto cleanup;
        }
    }

    if (!assemble_pass_one(cntxt, files, file_count))
        goto cleanup;

    if (!assemble_pass_two(cntxt, files, file_count, fmap_file, debug_file))
        goto cleanup;

    if (cntxt->gen_debuginf) {
        fclose(fmap_file);
        fclose(debug_file);

        FILE *sym_file;
        char *sym_file_name = malloc(sizeof(char) * (outfile_name_len + 8));
        strcpy(sym_file_name, cntxt->output_file);
        strcat(sym_file_name, ".sym");

        if ((sym_file = fopen(sym_file_name, "w")) == NULL) {
            perror(sym_file_name);
            goto cleanup;
        }

        write_symbols_file(sym_file);
        fclose(sym_file);
        free(sym_file_name);
    }

    free_dynamic_data(cntxt);
    free(fmap_name);
    free(debug_name);

    return TRUE;

cleanup:
    free_dynamic_data(cntxt);
    
    remove(cntxt->output_file);

    if (cntxt->gen_debuginf) {
        remove(fmap_name);
        remove(debug_name);
    }
    
    free(fmap_name);
    free(debug_name);
    
    return FALSE;
}

static int assemble_pass_one(asm_context_t *cntxt, char *files[], int file_count) {
    // 1st run: discover symbols
    FILE *f;

    for (int i = 0; i < file_count; i++) {
        if ((f = fopen(files[i], "r")) == NULL) {
            perror(files[i]);
            return FALSE;
        }

        cntxt->current_f_id = i;
        cntxt->current_file = files[i];
        cntxt->current_line = 0;

        if (!collect_jump_labels(cntxt, f)) {
            fclose(f);
            return FALSE;
        }

        if (!collect_data_labels(cntxt, f)) {
            fclose(f);
            return FALSE;
        }

        fclose(f);
    }

    // cntxt->current_addr += 4; // space for .dat marker
    if (!insert_data_symbols(cntxt))
        return FALSE;

    return TRUE;
}

static int assemble_pass_two(asm_context_t *cntxt, char *files[], int file_count,
                             FILE *fmapfile, FILE *debugfile)
{
    FILE *f, *outfile = fopen(cntxt->output_file, "w");

    if (outfile == NULL) {
        perror(cntxt->output_file);
        return FALSE;
    }

    // 2nd run: assemble
    cntxt->current_addr = INTTABLE_SIZE;

    for (int i = 0; i < file_count; i++) {
        if ((f = fopen(files[i], "r")) == NULL) {
            perror(files[i]);
            return FALSE;
        }

        cntxt->current_f_id = i;
        cntxt->current_file = files[i];
        cntxt->current_line = 0;

        if (!assemble_file(cntxt, f, outfile, debugfile)) {
            fclose(f);
            return FALSE;
        }

        fclose(f);

        if (cntxt->gen_debuginf)
            fprintf(fmapfile, "%u %s\n", cntxt->current_f_id, cntxt->current_file);
    }

    assemble_data(outfile);
    fclose(outfile);

    return TRUE;
}

int assemble_file(asm_context_t *cntxt, FILE *infile, FILE *outfile, FILE *debugfile) {
    rewind(infile); // read from begin

    char *line;
    uint8_t instruction[4];

    while ((line = read_line(cntxt, infile)) != NULL) {
        cntxt->current_line++;

        const size_t len = strlen(line);

        if (len == 0)
            continue; // empty line or comment
            
        // don't parse beyond the data mark
        if (strcasecmp(line, DATA_MARK) == 0)
            break;
            
        if (cntxt->gen_debuginf) {
            uint32_t f_id_be = GUINT32_TO_BE(cntxt->current_f_id);
            uint32_t line_be = GUINT32_TO_BE(cntxt->current_line);
            uint32_t addr_be = GUINT32_TO_BE(cntxt->current_addr);

            fwrite(&f_id_be, sizeof(uint32_t), 1, debugfile);
            fwrite(&line_be, sizeof(uint32_t), 1, debugfile);
            fwrite(&addr_be, sizeof(uint32_t), 1, debugfile);
        }

        if (line[len - 1] == ':') // found a jump label
            continue;

        /* split the input line into components */
        char *parts[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
        char *saveptr = NULL;
        parts[0] = strtok_r(line, " \t", &saveptr);
        parts[1] = strtok_r(NULL, " \t", &saveptr);
        parts[2] = strtok_r(NULL, " \t", &saveptr);
        parts[3] = strtok_r(NULL, " \t", &saveptr);
        parts[4] = strtok_r(NULL, "", &saveptr);

        int nparts = 0;

        while (parts[nparts] != NULL)
            nparts++;

        if (!assemble_line(cntxt, parts, nparts, instruction))
            return FALSE; // error while assembling this line

        fwrite(instruction, sizeof(uint32_t), 1, outfile);

        cntxt->current_addr += 4;
    }

    return TRUE;
}

int assemble_line(asm_context_t *cntxt, char *items[], int item_count, uint8_t instruction[4]) {
    command_t cmd;

    if (!get_command_by_name(items[0], &cmd)) {
        print_error(cntxt, "No such command: <%s>", items[0]);
        return FALSE;
    }

    instruction[0] = cmd.opcode;
    instruction[1] = instruction[2] = instruction[3] = 0;

    /* if the command expects a label,
     * replace the label with an offset */
    if (cmd.has_label) {
        if (item_count < 2) {
            /* no label, just the command alone */
            print_error(cntxt, "Command <%s> expects a label", cmd.opname);
            return FALSE;
        }

        symbol_t label;

        if (get_symbol(items[1], &label)) {
            int32_t label_offset = (label.sym_addr - cntxt->current_addr) / 4;
            static char label_buf[12];

            snprintf(label_buf, sizeof(label_buf), "%d", label_offset);
            items[1] = label_buf;
        } else {
            /* label was not set */
            print_error(cntxt, "Unset label: <%s>", items[1]);
            return FALSE;
        }
    }

    /* choose assembly function */
    int (*asm_func)(asm_context_t *cntxt, char *items[], int n, uint8_t instruction[4]);

    switch (cmd.format) {
        case CMDFMT_NUL:
            asm_func = assemble_nul;
            break;
        case CMDFMT_NNN:
            asm_func = assemble_nnn;
            break;
        case CMDFMT_R00:
            asm_func = assemble_r00;
            break;
        case CMDFMT_RNN:
            asm_func = assemble_rnn;
            break;
        case CMDFMT_RR0:
            asm_func = assemble_rr0;
            break;
        case CMDFMT_RRN:
            asm_func = assemble_rrn;
            break;
        case CMDFMT_RRR:
            asm_func = assemble_rrr;
            break;
        default:
            return FALSE;
    }

    return asm_func(cntxt, items, item_count, instruction);
}

void assemble_data(FILE *outfile) {
    const uint8_t BEGIN_DATA[4] = {0xff, 'D', 'A', 'T'};

    fwrite(BEGIN_DATA, 1, 4, outfile);

    for (GSList *l = get_data_list(); l != NULL; l = g_slist_next(l)) {
        data_t *data = l->data;
        size_t len;

        switch (data->type) {
        case DATATYPE_INT:
            fwrite(&(data->int_data.value), sizeof(int32_t), 1, outfile);
            break;
        case DATATYPE_STRING:
            len = strlen(data->string_data.value) + 1; // strlen() + 1: also write out '\0'

            fwrite(data->string_data.value, sizeof(char), len, outfile);

            while (len % 4 != 0) {
                 fputc('\0', outfile);
                 len++;
            }

            break;
        }
    }
}

static int collect_jump_labels(asm_context_t *cntxt, FILE *file) {
    char *line = NULL; // input line

    rewind(file); // set the read position at the begin of file

    while ((line = read_line(cntxt, file)) != NULL) {
        cntxt->current_line++;

        const size_t len = strlen(line);

        if (len == 0)
            continue; // empty line or comment

        if (line[len - 1] == ':') { // found a jump label
            if (len == 1) {
                /* empty label, this happens if the user
                   just writes ":" instead of "label:" */
                print_error(cntxt, "Empty label name");
                return FALSE;
            }

            // remove trailing ':' from jump label
            line[len - 1] = '\0';

            // check whether the jump label is a single word
            if (strpbrk(line, " \t") != NULL) {
                print_error(cntxt, "Label <%s> contains a whitespace", line);
                return FALSE;
            }

            // create a symbol
            symbol_t *label = malloc(sizeof(symbol_t));
            label->sym_name = strdup(line);
            label->sym_type = SYMTYPE_JUMP;
            label->sym_addr = cntxt->current_addr;

            if (!insert_symbol(label)) {
                print_error(cntxt, "Label <%s> already exists", label->sym_name);
                free(label->sym_name);
                free(label);
                return FALSE;
            }
        }
        else if (strcasecmp(line, DATA_MARK) == 0)
            break; // don't parse beyond the data mark
        else
            cntxt->current_addr += 4; // line is an instruction
    }

    return TRUE;
}

static int collect_data_labels(asm_context_t *cntxt, FILE *file) {
    char *line;

    while ((line = read_line(cntxt, file)) != NULL) {
        cntxt->current_line++;

        if (strlen(line) == 0)
            continue;

        /* split the input line into components */
        char *saveptr = NULL;
        char *type  = strtok_r(line, " \t", &saveptr);
        char *label = strtok_r(NULL, " \t", &saveptr);

        if (label == NULL) {
            print_error(cntxt, "No label for %s provided", type);
            return FALSE;
        }

        char *value = strtok_r(NULL, "", &saveptr);

        if (value == NULL) {
            print_error(cntxt, "No content for <%s> provided", label);
            return FALSE;
        }

        /* decide what to do depending on the type of the label */
        if (strcasecmp(type, STRING_MARK) == 0) {
            if (!collect_string_data(cntxt, label, value))
                return FALSE;
        } else if (strcasecmp(type, INTEGER_MARK) == 0) {
            if (!collect_int_data(cntxt, label, value))
                return FALSE;
        } else {
            print_error(cntxt, "No such data type: <%s>", type);
            return FALSE;
        }
    }

    return TRUE;
}

static int collect_string_data(asm_context_t *cntxt, char *label, char *content) {
    if (content[0] != '"') {
        print_error(cntxt, "No leading quotation mark in string content <%s>", content);
        return FALSE;
    }

    content++; // skip first quotation mark

    int content_len = strlen(content);

    if (content_len < 1 || content[content_len - 1] != '"') {
        print_error(cntxt, "No trailing quotation mark in string content <%s>", content);
        return FALSE;
    }

    content[content_len - 1] = '\0'; // delete second quotation mark

    insert_string_data(label, content);

    return TRUE;
}

static int collect_int_data(asm_context_t *cntxt, char *label, char *content) {
    long number;

    str_trim(content);

    if (!str_to_int(content, &number)) {
        print_error(cntxt, "<%s> is not a number", content);
        return FALSE;
    }

    insert_int_data(label, GINT_TO_BE(number));

    return TRUE;
}

void free_registers_ht();

static void free_dynamic_data(asm_context_t *cntxt) {
    free_data();
    free_symbols();
    free_commands_ht();
    free_registers_ht();

    free(cntxt->read_buf);
    cntxt->read_buf = NULL;
    cntxt->read_buf_size = 0;
}

static char *read_line(asm_context_t *cntxt, FILE *file) {
    // read one line
    if (getline(&(cntxt->read_buf), &(cntxt->read_buf_size), file) == -1)
        return NULL;

    // remove comments
    str_strip_comment(cntxt->read_buf);

    // trim
    str_trim(cntxt->read_buf);

    return cntxt->read_buf;
}
