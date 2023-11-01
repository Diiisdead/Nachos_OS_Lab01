#include "syscall.h"


int main()
{
  int id; 
  int fileSize;
  int len;
  int i;
  char source[100];
  char destination[100];
  char data[1000];
  int srcId;
  int desId;
  char c;

  Write("Enter source file name",100, 1);
  Read(source, 98, 0);

  Write("Enter destination file name",100, 1);
  Read(destination, 98, 0);

  srcId = Open(source, 1);
 
  desId = Open(destination, 0); 
	
  if (desId != -1)
{	
  fileSize = Seek(-1, srcId);
  Seek(0, srcId); 
  Seek(0, desId); 
  i = 0;
  for (; i < fileSize; i++) 
			{
				Read(&c, 1, srcId); 
				Write(&c, 1, desId); 
			}
                        Write("Copy successfull",100, 1);
                 
			
  Close(desId); 
} 		
  Close(srcId);


  
  
  Halt();
  /* not reached */
}
