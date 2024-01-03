#include "syscall.h"

int main()
{
    int id;
    int fileSize;
    int len;
    char buf[100];
    char buf1[1000];

    // Prompt for file name
    Write("Enter file name: ", 18, 1);
    len = Read(buf, 98, 0);
    buf[len] = '\0'; // Null-terminate the input string

    // Attempt to open the file
    id = Open(buf, 0);
    if (id == -1)
    {
        Write("Error: Unable to open the file", 31, 1);
    }
    else
    {
        // Read the file content
        fileSize = Seek(-1, id);
        Seek(0, id);
        Read(buf1, fileSize, id);
        buf1[fileSize] = '\0'; // Null-terminate the content

        // Write the file content to console
        Write(buf1, fileSize, 1);
        Write("\n",1,1);

        // Close the file
        Close(id);
    }

    // Terminate the program
    Halt();

    /* not reached */
}