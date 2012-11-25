#ifndef UMACHSHAREDMEMORY_H
#define UMACHSHAREDMEMORY_H
#include <QSharedMemory>
#include <QSystemSemaphore>

struct sharedHeaderStruct;
//struct sharedRegisterStruct;

class UmachSharedMemory
{
public:
    UmachSharedMemory();
    ~UmachSharedMemory();
    void init();

    QSharedMemory *m_sharedHeaderMemory;
    //QSharedMemory *m_sharedRegisterMemory;
    sharedHeaderStruct *m_sharedHeaderData;
    //sharedRegisterStruct *m_sharedRegisterData;

    QSystemSemaphore *m_waitForExecuteReqest;
    QSystemSemaphore *m_waitForExecuteDone;
    QSystemSemaphore *m_waitForFetchRequest;
    QSystemSemaphore *m_waitForFetchDone;
};

//extern UmachSharedMemory g_sharedMemory;

#endif // UMACHSHAREDMEMORY_H
