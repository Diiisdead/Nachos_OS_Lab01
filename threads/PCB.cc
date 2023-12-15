#include "PCB.h"
#define MAX_FILE 10
PCB::PCB(int id) {
    parentID = id;
    joinsem = new Semaphore("joinsem", 0);
    exitsem = new Semaphore("exitsem", 0);
    multex = new Semaphore("multex", 1);
    exitcode = 0;
    numwait = 0;
    
    fileTable = new FILE *[MAX_FILE];
    filemap = new Bitmap(MAX_FILE);
    filemap->Mark(0);
    filemap->Mark(1);

    thread = NULL;
}

PCB::~PCB() {
    delete joinsem;
    delete exitsem;
    delete multex;
    delete filemap;
    delete fileTable;
}

int PCB::Exec(char* name, int pid) {
    multex->P();
    this->thread = new Thread(name);

    if (this->thread == NULL) {
        printf("\nPCB::Exec: Not enough memory!\n");
        multex->V();
        return -1;
    }

    this->thread->processID = pid;
    this->parentID = kernel->currentThread->processID;
    
    //● Call Fork(StartProcess_2,id)

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

void PCB::SetExitCode(int ec)
{
    exitcode = exit_code;
}

int PCB::GetExitCode()
{
    return exitcode;
}

void PCB::SetFileName(char *fn)
{
    strcpy(thread->name, filename);
}

char *PCB::GetFileName()
{
    return thread->name;
}

