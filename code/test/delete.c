#include "syscall.h"

int main()
{
    int result;
    char buffer[100];
    Write("Enter file name to delete: ", 100, 1);
    Read(buffer, 98, 0);
    result = Remove(buffer);
    if(result == 0){
        Write("delete successfull\n", 25, 1);
    }
    else{
         Write("Eror: can not delete file", 100, 1);
    }

    Halt();
    /* not reached */
}