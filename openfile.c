#include "syscall.h"

int main()
{
  int id;
  int create;
  int result;
  create = Create("test.txt");
  id = Open("test.txt",1);
  result=Close(id);

  Halt();
  /* not reached */
}
