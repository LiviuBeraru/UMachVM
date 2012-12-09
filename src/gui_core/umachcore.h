#ifndef UMACHCORE_H
#define UMACHCORE_H

#include <QSharedMemory>
#include <QSystemSemaphore>
#include "sharedmemorystructs.h"

class UMachCore
{
public:
    UMachCore(const char *executable, bool debug = true, size_t memory = 2048);
    ~UMachCore();

    void run();

private:

    QSharedMemory *m_sharedHeaderMemory;
    QSharedMemory *m_sharedMachineMemory;
    QSystemSemaphore *m_waitForExecuteReqest;
    QSystemSemaphore *m_waitForExecuteDone;
    QSystemSemaphore *m_waitForFetchRequest;
    QSystemSemaphore *m_waitForFetchDone;

    sharedHeaderStruct *m_sharedHeaderData;

    const char *m_executable;

};

#endif // UMACHCORE_H
