#include "syscall.h"


int main()
{
  int id; 
  int fileSize;
  int len;
  int i;
  char buf[100];
  char buf1[1000];
  char c;
  
  Write("Enter file name",16, 1);
  //id = Open("test.txt",0);
  

  len = Read(buf, 98, 0);
  Write(buf, 98, 1);
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
