#ifndef SHAREDMEMORYSTRUCTS_H
#define SHAREDMEMORYSTRUCTS_H

#define KEY_HEADER     "UMachCore_sharedHeader_78"
#define KEY_MEMORY     "UMachCore_sharedMemory_78"
#define KEY_DONE_RUN   "UMachCore_sem_doneRun_78"
#define KEY_REQ_FETCH  "UMachCore_sem_reqFetch_78"
#define KEY_DONE_FETCH "UMachCore_sem_doneFetch_78"
#define KEY_REQ_EXEC   "UMachCore_sem_reqExec_78"
#define KEY_DONE_EXEC  "UMachCore_sem_doneExec_78"

#include "inttypes.h"

struct sharedHeaderStruct {
    bool m_runFlag;
    bool m_copyMemory;
    int sizeDataAdressArray;
    int sizeDataArray;
    int32_t m_registers[44];
};


#endif // SHAREDMEMORYSTRUCTS_H
