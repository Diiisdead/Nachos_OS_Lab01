#include "syscall.h"

int main(){
    int id,i;
    int fileSize;
    int len;
    char buf[100];
    char buf1[1000];
    Write("Procces 1 is starting\n",50,1);
    // id = Open("hello.txt",0);
    // if (id == -1)
    // {
    //     Write("Error: Unable to open the file", 31, 1);
    // }
    // else
    // {
    //     // Read the file content
    //     fileSize = Seek(-1, id);
    //     Seek(0, id);
    //     Read(buf1, fileSize, id);
    //     buf1[fileSize] = '\0'; // Null-terminate the content

    //     // Write the file content to console
    //     Write(buf1, fileSize, 1);
    //     Write("\n",1,1);

    //     // Close the file
    //     Close(id);
    // }
    for(i = 0;i<10;i++){
        Write("Procces 1 is running\n",50,1);
    }
    Write("Procces 1 is finishing\n",50,1);
}