#include "umachsharedmemory.h"
#include "../gui_core/sharedmemorystructs.h"


UmachSharedMemory::UmachSharedMemory()
{
    m_waitForExecuteReqest = NULL;
    m_waitForExecuteDone = NULL;
    m_waitForFetchRequest = NULL;
    m_waitForFetchDone = NULL;
    m_sharedHeaderMemory = NULL;
}

void UmachSharedMemory::init()
{
    delete(m_waitForExecuteReqest);
    delete(m_waitForExecuteDone);
    delete(m_waitForFetchRequest);
    delete(m_waitForFetchDone);
    delete(m_sharedHeaderMemory);

    m_waitForExecuteReqest = new QSystemSemaphore(KEY_REQ_EXEC);
    m_waitForExecuteDone = new QSystemSemaphore(KEY_DONE_EXEC);
    m_waitForFetchRequest = new QSystemSemaphore(KEY_REQ_FETCH);
    m_waitForFetchDone = new QSystemSemaphore(KEY_DONE_FETCH);

    m_sharedHeaderMemory = new QSharedMemory(KEY_HEADER);
    m_sharedHeaderMemory->attach(QSharedMemory::ReadOnly);
    m_sharedHeaderData = (sharedHeaderStruct*)m_sharedHeaderMemory->data();
    m_sharedHeaderMemory->detach();

    m_sharedMachineMemory = new QSharedMemory(KEY_MEMORY);
}

UmachSharedMemory::~UmachSharedMemory()
{
    delete(m_waitForExecuteReqest);
    delete(m_waitForExecuteDone);
    delete(m_waitForFetchRequest);
    delete(m_waitForFetchDone);
    delete(m_sharedHeaderMemory);
    delete(m_sharedMachineMemory);
}
