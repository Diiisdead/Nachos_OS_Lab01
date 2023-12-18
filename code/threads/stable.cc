#include "stable.h"

STable::STable()
{
    bm = new Bitmap(MAX_SEMAPHORE);
    for (int i = 0; i < MAX_SEMAPHORE; i++)
        semTab[i] = NULL;
}

STable::~STable() {
    if (bm) {
        delete bm;
        bm = NULL;
    }
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        if (semTab[i]) {
            delete semTab[i];
            semTab[i] = NULL;
        }
    }
}

int STable::Create(char *name, int init)
{
    for (int i = 0; i < MAX_SEMAPHORE; i++)
    {
        if (bm->Test(i))
        {
            if (strcmp(name, semTab[i]->GetName()) == 0)
            {
                return -1;
            }
        }
    }

    int index = FindFreeSlot(index);
    if (index < 0)
    {
        return -1;
    }

    semTab[index] = new Sem(name, init);
    return 0;
}

int STable::Wait(char *name)
{
    for (int i = 0; i < MAX_SEMAPHORE; i++)
    {
        if (bm->Test(i))
        {
            if (strcmp(name, semTab[i]->GetName()) == 0)
            {
                semTab[i]->wait();
                return 0;
            }
        }
    }
    return -1;
}

int STable::Signal(char *name)
{
    for (int i = 0; i < MAX_SEMAPHORE; ++i)
    {
        if (bm->Test(i))
        {
            if (strcmp(name, semTab[i]->GetName()) == 0)
            {
                semTab[i]->signal();
                return 0;
            }
        }
    }
    return -1;
}


int STable::FindFreeSlot(int id) { return this->bm->FindAndSet(); }
