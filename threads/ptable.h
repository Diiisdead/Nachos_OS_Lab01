#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "PCB.h"
#include "synch.h"


#define MAX_PROCESS 10

class PCB;
class Semaphore;

class PTable
{
private:
    int psize;
    Bitmap *bm;
    PCB *pcb[MAX_PROCESS];
    Semaphore* bmsem;

public:
    PTable(int size);
    ~PTable();

    int ExecUpdate(char*);
    int ExitUpdate(int);
    int JoinUpdate(int);

    int GetFreeSlot();
    bool IsExist(int pid);

    void Remove(int pid);
    char* GetFileName(int id);


};

#endif