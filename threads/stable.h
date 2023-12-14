#ifndef STABLE_H
#define STABLE_H
//#define MAX_PROCESS 10
#define MAX_SEMAPHORE 10
#include "string.h"
#include "bitmap.h"
#include "sem.h"


class Sem;
class STable
{
private:
    Bitmap *bm;
    Sem *semTab[MAX_SEMAPHORE];

public:
    STable();
    ~STable();
    int Create(char *name, int init);
    int Wait(char *name);
    int FindFreeSlot();

    int Signal(char *name);
};
#endif