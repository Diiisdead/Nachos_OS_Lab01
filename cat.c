#include "syscall.h"


int main()
{
  int id; 
  int fileSize;
  int i;
  char buf[100];
  char buf1[1000];
  char c;
  
  Write("Enter file name",16, 1);
  Create("test.txt");
  //id = Open("test.txt",0);
  

  ReadString(buf, 100);
  Write(buf, 100, 1);
  //Close(id);
  
  id = Open(buf,0);
 
  fileSize = Seek(-1, id);
  Seek(0, id);
  //Write("Data in file",20, 1);
  i = 0;
  Read(buf1, fileSize, id);
  Write(buf1,fileSize, 1);
  
  Close(id);
  
  Halt();
  /* not reached */
}
