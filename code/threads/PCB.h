
#ifndef PCB_H
#define PCB_H

#include "synch.h"


class Semaphore;
class PCB
{
private:
    Semaphore *joinsem; // semaphore for join process
    Semaphore *exitsem; // semaphore for exit process
    Semaphore *multex;  // semaphore for exclusive access
    int exitcode;
    int numwait; // number of process joined
    Thread *thread;

public:
    int parentID;
    int processID;

    FILE **fileTable;
    Bitmap *filemap;

public:
    PCB(int id);
    ~PCB();

    int Exec(char *name, int pid);

    int GetID();
    int GetNumWait();

    void JoinWait();
    void ExitWait();
    void JoinRelease();
    void ExitRelease();

    void IncNumWait();
    void DecNumWait();

    void SetExitCode(int ec);
    int GetExitCode();

    void SetFileName(char *fn);
    char *GetFileName();
};

void StartProcess(int id);

#endif // PCB_H