#include "ptable.h"


PTable::PTable(int size) {

    if (size <= MAX_PROCESS)
        psize = size;
    else
        psize = MAX_PROCESS;

    for (int i = 0; i < size; i++) {
        pcb[i] = NULL;
    }

    bmsem = new Semaphore("bmsem", 1);
    pcb[0] = new PCB(0);
    pcb[0]->parentID = -1;
}


PTable::~PTable() {
    int i;
    for (i = 0; i < psize; i++) {
        if (!pcb[i]) delete pcb[i];
    }
    delete bm;
    delete bmsem;
}

int PTable::ExecUpdate(char *name)
{
    
    bmsem->P();

    if (name == NULL || strlen(name) == 0)
    {
         DEBUG(dbgSys, "\nPTable::Exec : Can't not execute name is NULL.\n");
        bmsem->V();
        return -1;
    }

    
    if (strcmp(name, kernel->currentThread->getName()) == 0) {
        DEBUG(dbgSys, "\nPTable::Exec : Can't not execute itself.\n");
        bmsem->V();
        return -1;
    }

    int index = this->GetFreeSlot();

    if (index < 0)
    {
        bmsem->V();
        return -1;
    }

    pcb[index] = new PCB(index);
    pcb[index]->parentID = kernel->currentThread->processID;

    int pid = pcb[index]->Exec(name, index);

    bmsem->V();
    return pid;
}

int PTable::ExitUpdate(int ec)
{
    int id = kernel->currentThread->processID;

    if (!IsExist(id))
    {
        printf("The process does not exist\n");
        return -1;
    }

    if (id == 0)
    {
        kernel->currentThread->FreeSpace();
        kernel->interrupt->Halt();
        return 0;
    }

    
    pcb[id]->SetExitCode(ec);
    pcb[pcb[id]->parentID]->DecNumWait();


    pcb[id]->JoinRelease();
    pcb[id]->ExitWait();

    Remove(id);
    return ec;
}

int PTable::JoinUpdate(int id)
{

    if (id < 0 || id >= psize || pcb[id] == NULL)
    {
        printf("Invalid ID\n");
        return -1;
    }

    if (kernel->currentThread->processID != pcb[id]->parentID)
    {
        printf("The process can't join not parent process\n");
        return -1;
    }

    pcb[pcb[id]->parentID]->IncNumWait();
    pcb[id]->JoinWait();

    int ec = pcb[id]->GetExitCode();
    pcb[id]->ExitRelease();

    return ec;
}

int PTable::GetFreeSlot() {
    int i;
    for (i = 0; i < psize; i++) {
        if (!pcb[i]) return i;
    }
    return -1;
}

bool PTable::IsExist(int pid) { return pcb[pid] != NULL; }

void PTable::Remove(int pid) {
    if (pcb[pid] != NULL) {
        delete pcb[pid];
        pcb[pid] = NULL;
    }
}

char* PTable::GetFileName(int id) { return pcb[id]->GetFileName(); }
