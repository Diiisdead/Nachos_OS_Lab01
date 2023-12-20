#include "syscall.h"

int main(){
    int srcId, desId;
    int fileSize, i;
    char c;
    Write("Procces 2 is starting\n",50,1);
    // Open source and destination files
    srcId = Open("b.txt", 1);
    desId = Open("a.txt", 0);
    // Check if files opened successfully
    if (srcId == -1 || desId == -1)
    {
        Write("Error: Unable to open source or destination file", 100, 1);
    }
    else
    {
        // Read content from source file and write it to destination file
        fileSize = Seek(-1, srcId);
        Seek(0, srcId);
        Seek(0, desId);
        for (i = 0; i < fileSize; i++)
        {
            Read(&c, 1, srcId);
            Write(&c, 1, desId);
        }

        // Notify successful copy
        Write("Copy successful\n", 20, 1);

        // Close files
        Close(srcId);
        Close(desId);
    }
}