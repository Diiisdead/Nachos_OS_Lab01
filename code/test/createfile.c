#include "syscall.h"

int main()
{
    int result;
    char buf[100];

    Write("Enter file name to create: ",50, 1);
    Read(buf, 100, 0);
    result = Create(buf);
    if(result == 0){
        Write("Create file ",50, 1);
        Write(buf,50, 1);
        Write(" successfull!\n",50, 1);
    }
    Halt();
}