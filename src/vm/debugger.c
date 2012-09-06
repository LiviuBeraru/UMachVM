#include <stdio.h>
#include <stdlib.h> // free
#include <string.h> // memset, strcmp
#include <ctype.h>  // isprint()
#include <limits.h>

#include "core.h"
#include "disassemble.h"
#include "registers.h"
#include "memory.h"
#include "strings.h"
#include "umach.h"

/** Maximum numbers of items of a debugger command */
#define MAX_ARGS 10

/** A debugger function, implementing some command. */
typedef void (*cmd_fcn)(int argv, char *args[]);

struct db_command {
    const char *name;
    cmd_fcn     fcn;
    const char *help;
};

static const char prompt[]  = "umdb > ";
static int        db_run    = 0;
static int        pc = 0; // last PC

static cmd_fcn find_cmd(const char *name);
static void    print_current_instr(void);

/* Command functions */
static void    db_step(int argc, char *argv[]);
static void    db_quit(int argc, char *argv[]);
static void    db_help(int argc, char *argv[]);
static void    db_show(int argc, char *argv[]);
static void    dump_regs(void);
static void    dump_memory(int address, int nbytes);

static
struct db_command db_cmd_map[] = {
    {"step", db_step, "Step one instruction"},
    {"s",    db_step, "Same as step"},
    {"quit", db_quit, "Quit debugger"},
    {"q",    db_quit, "Same as quit"},
    {"help", db_help, "Show help"},
    {"show", db_show, "Show specified content: reg [name] | ins[truction]"},
    {"h",    db_help, "Same as help"},
    {"?",    db_help, "Same as help"},
    {NULL,   NULL,    NULL}
};


void debugger_run(void)
{
    char   *input = NULL;
    size_t  len  = 0;
    char   *argv[MAX_ARGS] = { NULL };
    int     argc = 0;
    cmd_fcn cmd = NULL;

    core_fetch();

    db_run = 1;
    while (db_run) {
        if (running && pc != registers[PC].value) {
            printf("[%03d]", registers[PC].value);
            print_current_instr();
            pc = registers[PC].value;
        }

        fputs(prompt, stdout);
        getline(&input, &len, stdin);
        argc = split(input, byspace, argv, MAX_ARGS);
        if (argc) {
            cmd = find_cmd(argv[0]);
            if (cmd) {
                cmd(argc, argv);
            } else {
                fprintf(stderr, "No such command: <%s>\n", argv[0]);
            }
        }
    }

    free(input);
}

cmd_fcn find_cmd(const char *name)
{
    struct db_command *cmd = db_cmd_map;
    /* linear search in the command table */
    while (cmd -> name) {
        if (strcmp(cmd->name, name) == 0) {
            return cmd->fcn;
        } else {
            cmd++;
        }
    }
    return NULL;
}

void db_step(int argc, char *argv[])
{
    if (! running) {
        /* the machine stoped running */
        printf("The maschine is not running.\n");
        return;
    }
    core_execute();
    if (running) {
        /* execution might have stoped the machine */
        core_fetch();
    }
}

void db_quit(int argc, char *argv[])
{
    db_run = 0;
}

void db_help(int argc, char *argv[])
{
    struct db_command *cmd = db_cmd_map;
    while (cmd -> name) {
        printf("\t%6s\t%s\n", cmd->name, cmd->help);
        cmd++;
    }
}

void print_current_instr(void)
{
    char instr_buffer[128] = {'\0'};
    disassemble(instruction, instr_buffer, 0);
    printf("\t%s\n", instr_buffer);
}

/* TODO: split this function, it's too long */
void db_show(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Show what?\n");
        return;
    }
    char *arg1 = argv[1];

    // strncasecmp is POSIX.1-2001
    if (strncasecmp("register", arg1, 3) == 0) {
        if (argc == 2) {
            dump_regs();
            return;
        }
        Register *r = NULL;
        int i = 2;
        for (; i < argc; i++) {
            r = get_register_byname(argv[i]);
            if (r) {
                printf("%s = 0x%08X = %d", r->name, r->value, r->value);
                // print as char if it is a printable char
                if (r->value > 0 && r->value < CHAR_MAX && isprint(r->value)) {
                    printf(" = '%c'\n", r->value);

                } else {
                    printf("\n");
                }
            } else {
                printf("%s is not a register\n", argv[i]);
                break;
            }
        }

        return;
    }

    if (strncasecmp("instruction", arg1, 3) == 0) {
        print_current_instr();
        return;
    }

    if (strncasecmp("memory", arg1, 3) == 0) {
        if (argc < 3) {
            /* user entered only 'show memory' */
            printf("No memory address specified.\n");
            printf("Try 'show memory <address> [<bytes>]'\n");
            return;
        }
        int adress = 0;
        int nbytes = 1;

        if (! sscanf(argv[2], "%d", &adress)) {
            /* specified address is not a number */
            printf("Try 'show memory <address> [<bytes>]'\n");
            return;
        }

        /* try to scan the number of bytes to print */
        if (argc > 3) {
            /* user entered something like
             *      show mem 123 something
             * where 'something' should be the number of bytes
             */
            if (! sscanf(argv[3], "%d", &nbytes)) {
                printf("%s is not a number, "
                       "defaulting number of bytes to print to 1\n",
                       argv[3]
                      );
                // nbytes is already set to 1
            }
        }

        dump_memory(adress, nbytes);

        return;
    }

    printf("<%s> is not an option...\n", arg1);
}

void dump_regs(void)
{
    int i;
    int max = 33;
    if (options.verbose) {
        max = NOREGS;
    }

    for (i = 1; i < max; i++) {
        if (! registers[i].name)// no such register
            continue;

        if (options.hexa) {
            printf("| %-4s = %8X ", registers[i].name, registers[i].value);
        } else {
            printf("| %-4s = %8d ", registers[i].name, registers[i].value);
        }
        
        if (i % 4 == 0) {
            printf("|\n");
        }
        if (i == 32 && options.verbose) {
            printf("\n");
        }
    }
    printf("\n");
}

void dump_memory(int address, int nbytes)
{
    uint8_t byte = 0;
    int i;
    /* read byte for byte from the memory module
       we could also allocate nbytes bytes with malloc
       and read the whole block,
       but I think it's safer to read byte for byte */
    printf("0x%04X: ", address);
    for (i = 1; i <= nbytes; i++, address++) {

        mem_read(&byte, address, 1);
        printf("0x%02X ", byte);

        if (i % 4 == 0) {
            printf(" ");
        }
        if (i % 8 == 0) {
            printf("\n");
            printf("0x%04X: ", address);
        }
    }
    printf("\n");
}
