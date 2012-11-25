#include "umachcore.h"
#include "sharedmemorystructs.h"

extern "C" {
#include "../vm/core.h"
#include "../vm/memory.h"
#include "../vm/logmsg.h"
#include "../vm/command.h"
#include "../vm/disassemble.h"
#include "../vm/debugger.h"
#include "../vm/registers.h"
#include "../vm/options.h"
}


UMachCore::UMachCore(const char *executable, bool debug, size_t memory)
{
    QSystemSemaphore *processStarted = new QSystemSemaphore("startDone_UMCore45");

    //create shared Memory for header info
    m_sharedHeaderMemory = new QSharedMemory();
    m_sharedHeaderMemory->setKey(KEY_HEADER);
    m_sharedHeaderMemory->create(sizeof(sharedHeaderStruct), QSharedMemory::ReadWrite);
    m_sharedHeaderData = (sharedHeaderStruct*)m_sharedHeaderMemory->data();

//    //create shared Memory for register info
//    m_sharedRegisterMemory = new QSharedMemory();
//    m_sharedRegisterMemory->setKey(KEY_REGISTER);
//    m_sharedRegisterMemory->create(sizeof(sharedRegisterStruct), QSharedMemory::ReadWrite);
//    m_sharedRegisterData = (sharedRegisterStruct*)m_sharedRegisterMemory->data();

    //System Semaphores for process sync
    m_waitForExecuteReqest = new QSystemSemaphore("executeRequest_UMCore45", 0, QSystemSemaphore::Create);
    m_waitForExecuteDone = new QSystemSemaphore("executeDone_UMCore45", 0, QSystemSemaphore::Create);
    m_waitForFetchRequest = new QSystemSemaphore("fetchRequest_UMCore45", 0, QSystemSemaphore::Create);
    m_waitForFetchDone = new QSystemSemaphore("fetchDone_UMCore45", 0, QSystemSemaphore::Create);


    options.disassemble = 0;
    options.debug = debug;
    options.memory = memory; // default memory size
    options.hexa = 0;
    options.verbose = 0;

    m_executable = executable;

    processStarted->release();
    delete(processStarted);
}

UMachCore::~UMachCore()
{
    //clean up
    delete(m_sharedHeaderMemory);
    delete(m_waitForExecuteDone);
    delete(m_waitForExecuteReqest);
}

void UMachCore::run()
{
    try {


    /* initialize memory */
    if (mem_init(options.memory) == -1) {
        logmsg(LOG_ERR,
        "Can not initialize memory with size %d.",
        options.memory);
        return;
    }

    logmsg(LOG_INFO, "Initialized memory (%ld bytes)", mem_getsize());

    /* initialize core */
    core_init();

    int progsize = mem_load_program_file(m_executable);

    if (progsize <= 0) {
        logmsg(LOG_ERR, "Cannot load program file %s.", m_executable);
        mem_free();
        return;
    }

    logmsg(LOG_INFO, "Loaded %d bytes program", progsize);

    m_sharedHeaderMemory->lock();
    m_sharedHeaderData->m_runFlag = true;
    m_sharedHeaderData->sizeDataArray = 120;
    m_sharedHeaderMemory->unlock();

    if (options.debug) {

        //fetch synch
        m_waitForFetchRequest->acquire();
        core_fetch();
        //fill in pc
        m_sharedHeaderMemory->lock();
        m_sharedHeaderData->m_registers[32] = registers[PC].value;
        m_sharedHeaderMemory->unlock();

        m_waitForFetchDone->release();

        while (running) {

            //wait for execution requested
            m_waitForExecuteReqest->acquire();

            //Check if stop is requested
            m_sharedHeaderMemory->lock();
            if (m_sharedHeaderData->m_runFlag == false) {
                m_sharedHeaderMemory->unlock();
                break;
            }
            m_sharedHeaderMemory->unlock();

            //if not, execute
            core_execute();

            //copy registers to shared memory
            m_sharedHeaderMemory->lock();
            for (int i = 1; i <= 44; i++){
                m_sharedHeaderData->m_registers[i - 1] = registers[i].value;
            }
            m_sharedHeaderMemory->unlock();

            //set shared flag, if not running anymore
            if (!running) {
                m_sharedHeaderMemory->lock();
                m_sharedHeaderData->m_runFlag = false;
                m_sharedHeaderMemory->unlock();
            }

            //execution done, so requester can work on data
            m_waitForExecuteDone->release();

            //fetch for next execution, if necessary
            if (running) {
                /* execution might have stoped the machine */

                //fetch synch
                m_waitForFetchRequest->acquire();
                core_fetch();
                //fill in pc
                m_sharedHeaderMemory->lock();
                for (int i = 1; i <= 44; i++){
                    m_sharedHeaderData->m_registers[i - 1] = registers[i].value;
                }
                m_sharedHeaderMemory->unlock();

                m_waitForFetchDone->release();

             }
            //next registers aviable

         }

    } else {
        core_run_program();
    }

    mem_free();
    //return 0;
    m_sharedHeaderMemory->lock();
    m_sharedHeaderData->m_runFlag = false;
    m_sharedHeaderMemory->unlock();


    }
    catch (...) {
        //clean up on error
        delete(m_sharedHeaderMemory);
        delete(m_waitForExecuteDone);
        delete(m_waitForExecuteReqest);
    }

}
