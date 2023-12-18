#include "syscall.h"

int main()
{
    int srcId, desId;
    int fileSize, i;
    char source[100];
    char destination[100];
    char c;

    // Prompt for source file name
    Write("Enter source file name: ", 25, 1);
    Read(source, 98, 0);

    // Prompt for destination file name
    Write("Enter destination file name: ", 30, 1);
    Read(destination, 98, 0);

    // Open source and destination files
    srcId = Open(source, 1);
    desId = Open(destination, 0);

    // Check if files opened successfully
    if (srcId == -1 || desId == -1)
    {
        Write("Error: Unable to open source or destination file", 48, 1);
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

    // Terminate the program
    Halt();

    /* not reached */
}
