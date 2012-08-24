#include <stdio.h>
#include <stdlib.h> // free
#include <string.h> // memset, strcmp

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
static const char byspace[] = " \t\n";
static int        db_run    = 0;

static int     split(char *string, const char *pattern, 
                     char *target[], size_t max);
static cmd_fcn find_cmd(const char *name);

/* Command functions */
static void    db_step(int argc, char *argv[]);
static void    db_quit(int argc, char *argv[]);
static void    db_help(int argc, char *argv[]);


static
struct db_command db_cmd_map[] = {
    {"step", db_step, "Step one instruction"},
    {"s",    db_step, "Same as step"},
    {"quit", db_quit, "Quit debugger"},
    {"q",    db_quit, "Same as quit"},
    {"help", db_help, "Show help"},
    {"h",    db_help, "Same as help"},
    {"?",    db_help, "Same as help"},
    {NULL, NULL, NULL}
};


void debugger_run(void)
{
    char   *input = NULL;
    size_t  len  = 0;
    char   *argv[MAX_ARGS] = { NULL };
    int     argc = 0;
    cmd_fcn cmd = NULL;
    
    db_run = 1;
    while (db_run) {
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

/** Split string into maximum max items using the specified split pattern.
    The items are stored into the array target, which is supposed to be
    at least max long. */
int split(char *string, const char *pattern, char *target[], size_t max)
{
    memset(target, 0, max * sizeof(*target));
    int i = 0;
    char *item = NULL;
    
    item = strtok(string, pattern);
    while(i < max && item != NULL) {
        target[i] = item;
        i++;
        item = strtok(NULL, pattern);
    }
    
    return i;
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
    printf("Stepping through...\n");
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