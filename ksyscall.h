/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"



void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}


int SysRead(char* buffer, int charCount, int fileId) {
    if (fileId == 0) {
        return kernel->synchConsoleIn->GetString(buffer, charCount);

    }
    return kernel->fileSystem->openfile[fileId]->Read(buffer, charCount);
}

char*  SysReadString( int length)
{
	char *buffer = new char[length + 1];

	int i = -1;
	while (i < length)
	{
		char c = kernel->synchConsoleIn->GetChar();
		if (c != '\n')
			buffer[++i] = c;
		else
			break;
	}
	buffer[i + 1] = 0;
	return buffer;
}

int SysWrite(char* buffer, int charCount, int fileId) {
    if (fileId == 1) {
        return kernel->synchConsoleOut->PutString(buffer, charCount);
    }
    return kernel->fileSystem->openfile[fileId]->Write(buffer, charCount);
}


#endif /* ! __USERPROG_KSYSCALL_H__ */
