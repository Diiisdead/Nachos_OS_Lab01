// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"

#define MaxFileLength 32


char *User2System(int virtAddr, int limit)
{
	int i; // index
	int oneChar;
	char *kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; // need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);
	// printf("\n Filename u2s:");
	for (i = 0; i < limit; i++)
	{
		kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		// printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

int System2User(int virtAddr, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}




//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;

		case SC_Create:
			int firstAdd;
			char* filename;
			DEBUG('u',"Reading first address of filename");
			firstAdd = kernel->machine->ReadRegister(4);
			DEBUG('u',"Coppy filename");
			filename = User2System(firstAdd,MaxFileLength + 1);
			if(filename == NULL){
				printf("Not enough memory in system");
				DEBUG('u',"Not enough money in system");
				kernel->machine->WriteRegister(2,-1);
				DEBUG('u',"Finish reading");
			}
			else{
				if(kernel->fileSystem->Create(filename,0) == false){
					printf("\n Error create file '%s'", filename);
					DEBUG('u',"Error create file");
					kernel->machine->WriteRegister(2, -1);
				}
				else{
					DEBUG('u',"Create file succesfull");
					kernel->machine->WriteRegister(2, 0);
				}

			}
			delete filename;
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			break;

    case SC_Open:
			DEBUG('u',"-------------------------\nSystem call Open")
			DEBUG('u',"Reading first address of file name and type");
			firstAdd = kernel->machine->ReadRegister(4);
			int type;
			type = kernel->machine->ReadRegister(5);
			char* filenameopen;
			if(type!=0 && type!=1){
				DEBUG('u',"Type are not 0 and 1");
				printf("Type must be 0 or 1\n");
				kernel->machine->WriteRegister(4,-1);
			}
			else{
				DEBUG('u',"Find the slot free in table");
				int index = kernel->fileSystem->findIndexFree();
				if(index == -1){
					DEBUG('u',"Table is Full");
					printf("Table is full! there are many files opened in the same time\n");
					kernel->machine->WriteRegister(2,-1);
				}
				else{
					DEBUG('u',"Coppy filename from user space to kernel space");
					filenameopen = User2System(firstAdd,MaxFileLength+1);
					if(filenameopen == NULL){
						printf("Not enough memory in system\n");
						DEBUG('u',"Not enough money in system");
						kernel->machine->WriteRegister(2,-1);
						DEBUG('u',"Finish open");
					}
					else{
						if((kernel->fileSystem->openfile[index] = kernel->fileSystem->Open(filenameopen,type)) == NULL){
							DEBUG('u',"File is not Exist!");
							printf("We are try to open a file that a not exist!\n");
							kernel->machine->WriteRegister(2,-1);
						}
						else{
							kernel->machine->WriteRegister(2,index);
							DEBUG('u',"Open file successfull");
							if(type == 1){
								printf("Open file read only '%s' successfull with OpenID: '%d'\n",filenameopen,index);
							}
							else{
								printf("Open file read and write '%s' successfull with OpenID: '%d'\n",filenameopen,index);
							}
						}
					}
				}
			}

			delete filenameopen;
			DEBUG('u',"System call finish");
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			return;
			ASSERTNOTREACHED();
			break;

		case SC_Close:
			DEBUG('u',"-------------------------\nSystem call Close")
			int id;
			DEBUG('u',"Read file id which want to close");
			id  = kernel->machine->ReadRegister(4);

			if(kernel->fileSystem->openfile[id] == NULL){
				DEBUG('u',"File is not open!");
				printf("File with ID '%d' is not open\n",id);
				kernel->machine->WriteRegister(2,-1);
			}
			else{
				DEBUG('u',"Close file successfull!");
				printf("File with id '%d' has been close\n",id);
				delete kernel->fileSystem->openfile[id];
				kernel->fileSystem->openfile[id] = NULL;
				kernel->machine->WriteRegister(2,0);
			}
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			DEBUG('u',"System call Close finish!");
			return;
			ASSERTNOTREACHED();
			break;


        case SC_Read:
        {
            DEBUG('u', "-------------------------\nSystem call Read\n");
            int bufferAddr = kernel->machine->ReadRegister(4);
            int size = kernel->machine->ReadRegister(5);
            int fileId = kernel->machine->ReadRegister(6);

            int bytesRead;

            char *buf = new char[size];
            buf = User2System(bufferAddr, size);
            if  (fileId < 0|| fileId > 20) {
                    printf("File with ID '%d' is out of range \n",fileId);
                    kernel->machine->WriteRegister(2,-1);
            }  else {
                   bytesRead = SysRead(buf, size, fileId);
                   kernel->machine->WriteRegister(2, bytesRead);
                   System2User(bufferAddr, bytesRead, buf);

            }
            delete[] buf;
            kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
            kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
            kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            return;
            ASSERTNOTREACHED();
            break;


        }


        case SC_Write:
        {
            // Input: buffer(char*), so ky tu(int), id cua file(OpenFileID)
            DEBUG('u', "-------------------------\nSystem call Write\n");
            int bufferAddr = kernel->machine->ReadRegister(4);
            int size = kernel->machine->ReadRegister(5);
            int fileId = kernel->machine->ReadRegister(6);

            char *buf = new char[size];
            int bytesWritten = 0;

            if  (fileId < 0 || fileId > 20) {
                    printf("File with ID '%d' is out of range \n",fileId);
                    kernel->machine->WriteRegister(2,-1);
            }else if (fileId == 1) {
                buf  = User2System(bufferAddr, size);
                bytesWritten = SysWrite(buf, size, fileId);
                kernel->machine->WriteRegister(2, bytesWritten);
                System2User(bufferAddr, bytesWritten, buf);
                printf("Finish writing \n");
            }else if ( kernel->fileSystem->openfile[fileId]->type ==1) {
                printf("Writing no allowed \n");
                kernel->machine->WriteRegister(2,-1);
            } else {
                buf  = User2System(bufferAddr, size);
                bytesWritten = SysWrite(buf, size, fileId);
                kernel->machine->WriteRegister(2, bytesWritten);
                System2User(bufferAddr, bytesWritten, buf);
                printf("Finish writing \n");

            }

            delete[] buf;

            kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
            kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
            kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            return;
            ASSERTNOTREACHED();
            break;
        }

        case SC_Seek:
        {
            DEBUG('u', "-------------------------\nSystem call Seek\n");
            int pos = kernel->machine->ReadRegister(4);
            int fileId = kernel->machine->ReadRegister(5);

            if  (fileId < 2 || fileId > 20) {
                    printf("File with ID '%d' is out of range \n",fileId);
                    kernel->machine->WriteRegister(2,-1);
            }else if ( kernel->fileSystem->openfile[fileId]  == NULL) {
                printf("file don't exit \n");
                kernel->machine->WriteRegister(2,-1);
            } else {
                pos = (pos == -1) ?kernel->fileSystem->openfile[fileId] ->Length() : pos;
                if (pos > kernel->fileSystem->openfile[fileId]->Length() || pos < 0) {
                    printf("\nCan't seek to this position.");
                    kernel->machine->WriteRegister(2, -1);
                } else {
                    kernel->fileSystem->openfile[fileId]->Seek(pos);
                    kernel->machine->WriteRegister(2, pos);
                }
            }

            kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
            kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
            kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            return;
            ASSERTNOTREACHED();
            break;
        }

		case SC_Add:
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}

			return;

			ASSERTNOTREACHED();

			break;


		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
