#include "syscall.h"

int main()
{
    int srcId, srcId1, desId;
    int fileSize, i;
    char source[100], source1[100], destination[100];
    char c;
    char newline = '\n'; // Newline character

    // Prompt for source file names
    Write("Enter source file name 1: ", 27, 1);
    Read(source, 100, 0);
    Write("Enter source file name 2: ", 27, 1);
    Read(source1, 100, 0);

    // Prompt for destination file name
    Write("Enter destination file name: ", 31, 1);
    Read(destination, 100, 0);

    // Open source files
    srcId = Open(source, 1);
    srcId1 = Open(source1, 1);

    // Check if source files opened successfully
    if (srcId == -1 || srcId1 == -1)
    {
        Write("Error: Unable to open source file(s)", 36, 1);
    }
    else
    {
        // Open destination file
        desId = Open(destination, 0);

        // Check if destination file opened successfully
        if (desId == -1)
        {
            Write("Error: Unable to open destination file", 39, 1);
        }
        else
        {
            // Copy content from source file 1 to destination file
            fileSize = Seek(-1, srcId);
            Seek(0, srcId);
            Seek(0, desId);
            for (i = 0; i < fileSize; i++)
            {
                Read(&c, 1, srcId);
                Write(&c, 1, desId);
            }

            // Write newline character
            //Write(&newline, 1, desId);

            // Copy content from source file 2 to destination file
            fileSize = Seek(-1, srcId1);
            Seek(0, srcId1);
            for (i = 0; i < fileSize; i++)
            {
                Read(&c, 1, srcId1);
                Write(&c, 1, desId);
            }

            // Close files
            Close(srcId);
            Close(srcId1);
            Close(desId);

            // Notify successful concatenation
            Write("Concatenation successful\n", 25, 1);
        }
    }

    // Terminate the program
    Halt();
}