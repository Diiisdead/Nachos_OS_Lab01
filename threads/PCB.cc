#include "PCB.h"

PCB::PCB(int id) {
    parentID = id;
    joinsem = new Semaphore(0); 
    exitsem = new Semaphore(0); 
    mutex = new Semaphore(1); 
    exitcode = 0;
    numwait = 0;
    filename = nullptr;
}

PCB::~PCB() {
    delete joinsem;
    delete exitsem;
    delete mutex;
    delete[] filename;
}

int PCB::Exec(char* name, int pid) {
    multex->P();
    this->thread = new thread(filename);

    if (this->thread == NULL) {
        printf("\nPCB::Exec: Not enough memory!\n");
        multex->V();
        return -1;
    }

    this->thread->processID = pid;
    this->parentID = kernel->currentThread->processID;
    
    this->thread->Fork(StartProcess_2, &this->thread->processID);

    multex->V();
    return pid;

}

int PCB::GetID() { return thread->processID; }

int PCB::GetNumWait()
{
    return numwait;
}

void PCB::JoinWait()
{
    joinsem->P();
}

void PCB::ExitWait()
{
    exitsem->P();
}

void PCB::JoinRelease()
{
    joinsem->V();
}

void PCB::ExitRelease()
{
    exitsem->V();
}

void PCB::IncNumWait()
{
    multex->P();
    numwait++;
    multex->V();
}

void PCB::DecNumWait()
{
    multex->P();
    if (numwait > 0)
    {
        numwait--;
    }
    multex->V();
}

void PCB::SetExitCode(int exit_code)
{
    exitcode = exit_code;
}

int PCB::GetExitCode()
{
    return exitcode;
}

void PCB::SetFileName(char *filename)
{
    strcpy(thread->name, filename);
}

char *PCB::GetFileName()
{
    return thread->name;
}

