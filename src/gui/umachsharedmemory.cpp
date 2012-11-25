#include "umachsharedmemory.h"
#include "../gui_core/sharedmemorystructs.h"

//UmachSharedMemory g_sharedMemory;

UmachSharedMemory::UmachSharedMemory()
{
    m_waitForExecuteReqest = NULL;
    m_waitForExecuteDone = NULL;
    m_waitForFetchRequest = NULL;
    m_waitForFetchDone = NULL;
    m_sharedHeaderMemory = NULL;
    //m_sharedRegisterMemory = NULL;

}

void UmachSharedMemory::init()
{
    delete(m_waitForExecuteReqest);
    delete(m_waitForExecuteDone);
    delete(m_waitForFetchRequest);
    delete(m_waitForFetchDone);
    delete(m_sharedHeaderMemory);
    //delete(m_sharedRegisterMemory);

    m_waitForExecuteReqest = new QSystemSemaphore("executeRequest_UMCore45");
    m_waitForExecuteDone = new QSystemSemaphore("executeDone_UMCore45");
    m_waitForFetchRequest = new QSystemSemaphore("fetchRequest_UMCore45");
    m_waitForFetchDone = new QSystemSemaphore("fetchDone_UMCore45");

    m_sharedHeaderMemory = new QSharedMemory(KEY_HEADER);
    m_sharedHeaderMemory->attach(QSharedMemory::ReadOnly);
    m_sharedHeaderData = (sharedHeaderStruct*)m_sharedHeaderMemory->data();
    m_sharedHeaderMemory->detach();

//    m_sharedRegisterMemory = new QSharedMemory(KEY_REGISTER);
//    m_sharedRegisterMemory->attach(QSharedMemory::ReadOnly);
//    m_sharedRegisterData = (sharedRegisterStruct*)m_sharedRegisterMemory->data();
//    m_sharedRegisterMemory->detach();

}

UmachSharedMemory::~UmachSharedMemory()
{
    delete(m_waitForExecuteReqest);
    delete(m_waitForExecuteDone);
    delete(m_waitForFetchRequest);
    delete(m_waitForFetchDone);
    delete(m_sharedHeaderMemory);
   // delete(m_sharedRegisterMemory);
}
