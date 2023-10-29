#include "syscall.h"


int main()
{
  int id, id1, id2;
  int len;
  int create, create1, create2;
  int result, result1, result2;

  char buffer[100];
  int write, write1,write2, read, read1;

  create = Create("test.txt");
  id = Open("test.txt",0);
  write = Write("Okay12345", 10, id);
  Read(buffer, 5, id);
  result=Close(id);

  id = Open("test.txt",0);
  Read(buffer, 5, id);
  Close(id);


  create1= Create("test1.txt");
  id1 = Open("test1.txt",0);
  write1 = Write(buffer, 5, id1);
  result1=Close(id1);
  
  


  create2 = Create("test2.txt");
  id2 = Open("test2.txt",0);
  len = Read(buffer, 10, 0);
  Write(buffer, len, id2);
  result=Close(id2);
  
  id2 = Open("test2.txt",0);
  Read(buffer, 10, id2);
  result=Close(id2);



  Write(buffer, 5, 1);

  id2 = Open("test2.txt",1);
  Write(buffer, 5, id2);
  result=Close(id2);
   
  
  Halt();
  /* not reached */
}
