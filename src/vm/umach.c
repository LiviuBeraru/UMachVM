#include <stdio.h>
#include <stdlib.h>    // exit
#include <unistd.h>    // getopt

#include "umach.h"
#include "core.h"
#include "memory.h"
#include "logmsg.h"
#include "command.h"
#include "disassemble.h"
#include "debugger.h"

static void parse_opts(int argc, char *argv[]);
static void run_through(void);

struct umach_options options;

int main(int argc, char *argv[])
{
    parse_opts(argc, argv);

    FILE *file = fopen(options.filename, "r");
    if(file == NULL) {
        perror(options.filename);
        abort();
    }

    if (options.disassemble) {
        disassemble_file(file);
        fclose(file);
        return 0;
    }
    
    if (mem_init(options.memory) == -1) {
        logmsg(LOG_ERR, 
        "Can not initialize memory with size %d.", 
        options.memory);
        exit(1);
    }
    
    core_init();

    int progsize = mem_load_program_file(file);
    fclose(file);

    if (progsize <= 0) {
        logmsg(LOG_ERR, "Cannot load program file %s.", options.filename);
        abort();
    }

    logmsg(LOG_INFO, "Loaded %d bytes program", progsize);
    if (options.debug) {
        debugger_run();
    } else {
        run_through();
    }   

    mem_free();
    return 0;
}

void parse_opts(int argc, char *argv[])
{
    options.verbose = 0;
    options.debug = 0;
    options.memory = 512;
    options.filename = NULL;
    options.disassemble = 0;

    opterr = 0;
    /*
     * Program options:
     * -v verbose mode 
     * -d debug mode
     * -s disassemble program file
     * -m <number> memory size
     */
    const char opts[] = "vdsm:";
    int c;
    while((c = getopt(argc, argv, opts)) != -1) {
        switch (c) {
            case 'v':
                options.verbose++;
                break;
            case 'd':
                options.debug++;
                break;
            case 's':
                options.disassemble = 1;
                break;
            case 'm':
                options.memory = atoi(optarg);
                break;
            case '?':
                printf("Unknown option: -%c\n", optopt);
                break;
            default:
                abort();
        }
    }
    
    if (optind >= argc) {
        logmsg(LOG_ERR, "No program file specified");
        abort();
    } else {
        options.filename = argv[optind];
    }
}

void run_through(void)
{
    core_run_program();
    if (options.verbose) {
        core_dump_regs();
    }
}
