#ifndef UMACHSHAREDMEMORY_H
#define UMACHSHAREDMEMORY_H
#include <QSharedMemory>
#include <QSystemSemaphore>

struct sharedHeaderStruct;


class UmachSharedMemory
{
public:
    UmachSharedMemory();
    ~UmachSharedMemory();
    void init();

    QSharedMemory *m_sharedHeaderMemory;
    QSharedMemory *m_sharedMachineMemory;
    sharedHeaderStruct *m_sharedHeaderData;

    QSystemSemaphore *m_waitForExecuteReqest;
    QSystemSemaphore *m_waitForExecuteDone;
    QSystemSemaphore *m_waitForFetchRequest;
    QSystemSemaphore *m_waitForFetchDone;
};

#endif // UMACHSHAREDMEMORY_H
