#include <string>
#include <iostream>
#include <inttypes.h>
#include "umachcore.h"

int main(int argc, char *argv[])
{
    //parse the ops

    bool debug = false;
    char *binPath = NULL;
    size_t memorySize = 2048;

    binPath = argv[1];

    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "-m"))
            memorySize = (size_t)atoi(argv[i + 1]);
        else if (!strcmp(argv[i], "-d"))
            debug = true;
    }

    if (!binPath)
        exit(EXIT_FAILURE);


    UMachCore core(binPath, debug, memorySize);
    core.run();
    
}
