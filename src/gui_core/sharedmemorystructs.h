#ifndef SHAREDMEMORYSTRUCTS_H
#define SHAREDMEMORYSTRUCTS_H

#define KEY_HEADER     "UMachCore_sharedHeader_83"
#define KEY_MEMORY     "UMachCore_sharedMemory_83"
#define KEY_DONE_RUN   "UMachCore_sem_doneRun_83"
#define KEY_REQ_FETCH  "UMachCore_sem_reqFetch_83"
#define KEY_DONE_FETCH "UMachCore_sem_doneFetch_83"
#define KEY_REQ_EXEC   "UMachCore_sem_reqExec_83"
#define KEY_DONE_EXEC  "UMachCore_sem_doneExec_83"

#include "inttypes.h"

struct sharedHeaderStruct {
    bool m_runFlag;
    bool m_copyMemory;
    int32_t m_registers[44];
};


#endif // SHAREDMEMORYSTRUCTS_H
