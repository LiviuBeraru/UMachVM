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
#include "assemble.h"

static void parse_opts(int argc, char *argv[]);

struct umach_options options;


int main(int argc, char *argv[])
{
    parse_opts(argc, argv);
    int    nargc = argc - optind;
    char **nargv = argv + optind;
    
    if (nargc <= 0) {
        logmsg(LOG_ERR, "No file names specified");
        abort();
    }
    
    if (options.assemble) {
        assemble_files(nargc, nargv);
        return 0;
    }

    if (options.disassemble) {
        disassemble_files(nargc, nargv);
        return 0;
    }
    
    if (mem_init(options.memory) == -1) {
        logmsg(LOG_ERR, 
        "Can not initialize memory with size %d.", 
        options.memory);
        exit(1);
    }
    
    core_init();

    int progsize = mem_load_program_file(nargv[0]);

    if (progsize <= 0) {
        logmsg(LOG_ERR, "Cannot load program file %s.", nargv[0]);
        mem_free();
        abort();
    }

    logmsg(LOG_INFO, "Loaded %d bytes program", progsize);
    if (options.debug) {
        debugger_run();
    } else {
        core_run_program();
    }   

    mem_free();
    return 0;
}

void parse_opts(int argc, char *argv[])
{
    options.assemble = 0;
    options.disassemble = 0;
    options.debug = 0;
    options.memory = 512;
    options.hexa = 0;
    options.verbose = 0;

    opterr = 0;
    /*
     * Program options:
     * -a assemble files
     * -s disassemble program files
     * -d debug mode
     * -m <number> memory size
     * -x hexa output
     * -v verbose mode 
     */
    const char opts[] = "asdm:xv";
    int c;
    while((c = getopt(argc, argv, opts)) != -1) {
        switch (c) {
            case 'a':
                options.assemble++;
                break;
            case 's':
                options.disassemble++;
                break;
            case 'd':
                options.debug++;
                break;
            case 'm':
                options.memory = atoi(optarg);
                break;
            case 'x':
                options.hexa++;
                break;
            case 'v':
                options.verbose++;
                break;
            case '?':
                printf("Unknown option: -%c\n", optopt);
                break;
            default:
                abort();
        }
    }
}
