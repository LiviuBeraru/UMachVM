#ifndef SHAREDMEMORYSTRUCTS_H
#define SHAREDMEMORYSTRUCTS_H

#define KEY_HEADER "UMachCore_sharedHeader73"

#include "inttypes.h"

struct sharedHeaderStruct {
    bool m_runFlag;
    int sizeDataAdressArray;
    int sizeDataArray;
    int32_t m_registers[44];
};


#endif // SHAREDMEMORYSTRUCTS_H
