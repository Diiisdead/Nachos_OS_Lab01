#include "syscall.h"

int main()
{
    int fileId;
    char buffer[256];
    int bytesRead;
    int bytesWritten;
    int createResult;

    // Create a file called "test.txt"
    createResult = Create("test.txt");
    if (createResult == -1)
    {
        // Error creating file
        Exit(-1);
    }

    // Open the file for writing
    fileId = Open("test.txt", 1);
    if (fileId == -1)
    {
        // Error opening file
        Exit(-1);
    }

    // Write data to the file
    char dataToWrite[] = "Hello, Nachos!";
    bytesWritten = Write(dataToWrite, sizeof(dataToWrite) - 1, fileId); // Exclude the null terminator
    if (bytesWritten != sizeof(dataToWrite) - 1)
    {
        // Error writing to file
        Close(fileId);
        Exit(-1);
    }

    // Close the file
    Close(fileId);

    // Reopen the file for reading
    fileId = Open("test.txt", 0);
    if (fileId == -1)
    {
        // Error opening file for reading
        Exit(-1);
    }

    // Read data from the file
    bytesRead = Read(buffer, sizeof(buffer) - 1, fileId); // Exclude space for null terminator
    if (bytesRead > 0)
    {
        // Print the data read from the file
        buffer[bytesRead] = '\0'; // Null-terminate the string
        Print(buffer);
    }
    else
    {
        // Error reading from file
        Close(fileId);
        Exit(-1);
    }

    // Close the file
    Close(fileId);

    // Exit the program
    Halt();
    /* not reached */
}
