#include "syscall.h"

int main()
{
    SpaceId newProc1;
    SpaceId newProc2;

    newProc1 = Exec("proc1"); // Project 01
    newProc2 = Exec("proc2"); // Project 01

    Join(newProc1);
    Join(newProc2);

}