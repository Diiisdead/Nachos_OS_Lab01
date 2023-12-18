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
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MaxFileLength 32
#define MaxLenghtData 1024

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

// NoException,           // Everything ok!
// 		     SyscallException,      // A program executed a system call.
// 		     PageFaultException,    // No valid translation found
// 		     ReadOnlyException,     // Write attempted to page marked 
// 					    // "read-only"
// 		     BusErrorException,     // Translation resulted in an 
// 					    // invalid physical address
// 		     AddressErrorException, // Unaligned reference or one that
// 					    // was beyond the end of the
// 					    // address space
// 		     OverflowException,     // Integer overflow in add or sub.
// 		     IllegalInstrException, // Unimplemented or reserved instr.
		     
// 		     NumExceptionTypes

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case NoException:
		return;
		DEBUG('u',"No Exception");
		break;
	case PageFaultException:
		DEBUG('u',"No valid translation found");
		SysHalt();
        break;
	case  BusErrorException:
		DEBUG('u',"naligned reference or one that was beyond the end of the address space");
		SysHalt();
        break;
	case ReadOnlyException:
		DEBUG('u',"Write attempted to page marked read-only");
		SysHalt();
        break;
	case AddressErrorException:
		DEBUG('u',"Translation resulted in an invalid physical address");
		SysHalt();
        break;
	case OverflowException:
		DEBUG('u',"Integer overflow in add or sub");
		SysHalt();
        break;
	case IllegalInstrException:
		DEBUG('u',"Unimplemented or reserved instr");
		SysHalt();
        break;
	case  NumExceptionTypes:
		DEBUG('u',"Number exception");
		SysHalt();
        break;
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;

		case SC_Create:
			DEBUG('u', "-------------------------\nSystem call Create")
			int firstAdd;
			char *filename;
			DEBUG('u', "Reading first address of filename");
			firstAdd = kernel->machine->ReadRegister(4);
			DEBUG('u', "Coppy filename");
			filename = User2System(firstAdd, MaxFileLength + 1);
			if (filename == NULL)
			{
				printf("Not enough memory in system\n");
				DEBUG('u', "Not enough money in system");
				kernel->machine->WriteRegister(2, -1);
				DEBUG('u', "Finish reading");
			}
			else
			{
				if (kernel->fileSystem->Create(filename, 0) == false)
				{
					DEBUG('u', "Error create file");
					kernel->machine->WriteRegister(2, -1);
				}
				else
				{
					DEBUG('u', "Create file succesfull");
					kernel->machine->WriteRegister(2, 0);
				}
			}
			delete filename;
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			ASSERTNOTREACHED();
			break;

		case SC_Open:
			DEBUG('u', "-------------------------\nSystem call Open")
			DEBUG('u', "Reading first address of file name and type");
			firstAdd = kernel->machine->ReadRegister(4);
			int type;
			type = kernel->machine->ReadRegister(5);
			char *filenameopen;
			if (type != 0 && type != 1)
			{
				DEBUG('u', "Type are not 0 and 1");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				DEBUG('u', "Find the slot free in table");
				int index = kernel->fileSystem->findIndexFree();
				if (index == -1)
				{
					DEBUG('u', "Table is Full");
					kernel->machine->WriteRegister(2, -1);
				}
				else
				{
					DEBUG('u', "Coppy filename from user space to kernel space");
					filenameopen = User2System(firstAdd, MaxFileLength + 1);
					if (filenameopen == NULL)
					{
						DEBUG('u', "Not enough memory in system");
						kernel->machine->WriteRegister(2, -1);
					}
					else
					{
						if ((kernel->fileSystem->openfile[index] = kernel->fileSystem->Open(filenameopen, type)) == NULL)
						{
							DEBUG('u', "File is not Exist!");
							kernel->machine->WriteRegister(2, -1);
						}
						else
						{
							kernel->machine->WriteRegister(2, index);
							DEBUG('u', "Open file successfull");
							if (type == 0)
							{
								DEBUG('u', "Open file read and write " << filenameopen << " successfull with OpenID: " << index);
							}
							else
							{
								DEBUG('u', "Open file read only " << filenameopen << " successfull with OpenID: " << index);
							}
						}
					}
				}
			}

			delete filenameopen;
			DEBUG('u', "System call finish");
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			return;
			ASSERTNOTREACHED();
			break;

		case SC_Close:
			DEBUG('u', "-------------------------\nSystem call Close")
			int id;
			DEBUG('u', "Read file id which want to close");
			id = kernel->machine->ReadRegister(4);

			if (kernel->fileSystem->openfile[id] == NULL)
			{
				DEBUG('u', "File is not open!");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				DEBUG('u', "Close file successfull!");
				delete kernel->fileSystem->openfile[id];
				kernel->fileSystem->openfile[id] = NULL;
				kernel->machine->WriteRegister(2, 0);
			}
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			DEBUG('u', "System call Close finish!");
			return;
			ASSERTNOTREACHED();
			break;

		case SC_Remove:
			// syscall remove file, check if file is opened or not
			DEBUG('u', "-------------------------\nSystem call Remove")
			char *filenameRemove;
			DEBUG('u', "Read file name");
			filenameRemove = User2System(kernel->machine->ReadRegister(4), MaxFileLength + 1);

			if (filenameRemove == NULL){
				DEBUG('u', "Read file name");
				kernel->machine->WriteRegister(2,-1);
			}
			else{
				bool check = false;
				for (int i =0;i<MaxFile;i++){
					if (kernel->fileSystem->openfile[i] != NULL && strcmp(kernel->fileSystem->openfile[i]->filename,filenameRemove)==0){
						check = true;
					}
				}
				if(check){
					DEBUG('u',"File is opening");
					kernel->machine->WriteRegister(2,-1);
				}
				else{
					if (kernel->fileSystem->Remove(filenameRemove) == false)
					{
						DEBUG('u', "File is not exist!");
						kernel->machine->WriteRegister(2, -1);
					}
					else
					{
						DEBUG('u', "Remove file successfull!");
						kernel->machine->WriteRegister(2, 0);
					}
				}
			}

			delete filenameRemove;
			DEBUG('u', "System call Remove finish!");
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			return;
			ASSERTNOTREACHED();
			break;

		case SC_SocketTCP:
			DEBUG('u', "-------------------------\nSystem call SocketTCP")
			DEBUG('u', "Find index free for socket");
			int indexSocket;
			indexSocket = kernel->fileSystem->findIndexSocketFree();
			if (indexSocket == -1)
			{
				DEBUG('u', "Socket is full!");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				int idSocket;
				DEBUG('u', "Create Socket!");
				idSocket = socket(AF_INET, SOCK_STREAM, 0);
				if (idSocket < 0)
				{
					DEBUG('u', "Cannot create socket!");
					kernel->machine->WriteRegister(2, -1);
				}
				else
				{
					kernel->fileSystem->socket[indexSocket] = idSocket;
					DEBUG('u', "Create socket successfull!");
					kernel->machine->WriteRegister(2, idSocket);
				}
			}
			DEBUG('u', "System call SocketTCP finish!");
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			return;

			break;

		case SC_Connect:
			DEBUG('u', "-------------------------\nSystem call Connect")
			DEBUG('u', "System call connect start!");
			int idSocket;
			idSocket = kernel->machine->ReadRegister(4);
			if (idSocket < 0)
			{
				DEBUG('u', "Id socket is not exist!");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				struct sockaddr_in server;
				DEBUG('u', "Read ip and port server");
				int port = kernel->machine->ReadRegister(6);
				firstAdd = kernel->machine->ReadRegister(5);
				char *ipAddr;
				ipAddr = User2System(firstAdd, MaxFileLength);
				if (ipAddr == NULL)
				{
					DEBUG('u', "Not enough memory in system");
					kernel->machine->WriteRegister(2, -1);
				}
				else
				{

					DEBUG('u', "Init infromation of server");
					server.sin_family = AF_INET;
					server.sin_addr.s_addr = inet_addr(ipAddr);
					server.sin_port = htons(port);
					int len = sizeof(server);

					if (connect(idSocket, (struct sockaddr *)&server, len) < 0)
					{
						DEBUG('u', "Cannot connect to server");
						kernel->machine->WriteRegister(2, -1);
					}
					else
					{
						DEBUG('u', "Connect succesffull!");
						kernel->machine->WriteRegister(2, 0);
					}
				}
				delete ipAddr;
			}
			DEBUG('u', "System call connect finish!");

			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			break;

		case SC_Send:
			DEBUG('u', "-------------------------\nSystem call Send")
			DEBUG('u', "System call Send start");
			idSocket = kernel->machine->ReadRegister(4);
			if (idSocket < 0)
			{
				DEBUG('u', "Id socket is not exist!");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				char *buffer;
				DEBUG('u', "Read first address of data");
				firstAdd = kernel->machine->ReadRegister(5);
				DEBUG('u', "Coppy data from user space to kernel space");
				buffer = User2System(firstAdd, MaxLenghtData);
				if (buffer == NULL)
				{
					DEBUG('u', "Not enough memory in system");
					kernel->machine->WriteRegister(2, -1);
				}
				else
				{
					int len = kernel->machine->ReadRegister(6);
					int numbyte;
					DEBUG('u', "Send data");
					if (len < strlen(buffer))
					{
						numbyte = send(idSocket, buffer, len, 0);
					}
					else
					{
						numbyte = send(idSocket, buffer, strlen(buffer), 0);
					}
					kernel->machine->WriteRegister(2, numbyte);
					if (numbyte > 0)
					{
						DEBUG('u', "Number of bytes to send is " << numbyte << ". Message: %s\n"
																 << buffer);
					}
				}

				delete buffer;
			}
			DEBUG('u', "System call send finish");
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			break;

		case SC_Receive:
			DEBUG('u', "-------------------------\nSystem call Receive")
			DEBUG('u', "System call Receive start");
			idSocket = kernel->machine->ReadRegister(4);
			if (idSocket < 0)
			{
				DEBUG('u', "Id socket is not exist!");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				DEBUG('u', "Read first address of data");
				firstAdd = kernel->machine->ReadRegister(5);
				if (0)
				{
				}
				else
				{
					int len = kernel->machine->ReadRegister(6);
					int numbyte;
					DEBUG('u', "Receive data");
					char recv[MaxLenghtData] = {0};
					
					DEBUG('u', "1");
					numbyte = read(idSocket, recv, len);
					DEBUG('u', "2");
					kernel->machine->WriteRegister(2, numbyte);
					DEBUG('u', "3");
					if (numbyte != -1)
					{
						DEBUG('u', "Number of bytes to recv is: " << numbyte << " Message: " << recv);
						System2User(firstAdd, len, recv);
						
					}
					else
					{
						DEBUG('u', "No reply!");
					}
				}

				// delete buffer;
			}
			DEBUG('u', "System call Receive finish");
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			break;

		case SC_CloseSoc:
			DEBUG('u', "-------------------------\nSystem call CloseSoc")
			DEBUG('u', "System call CloseSoc start");
			idSocket = kernel->machine->ReadRegister(4);
			if (idSocket < 0)
			{
				DEBUG('u', "Id socket is not exist!");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				int pos = kernel->fileSystem->findIndexSocket(idSocket);
				if (pos != -1)
				{
					DEBUG('u', "Close Socket");
					kernel->fileSystem->socket[pos] = -1;
					close(idSocket);
					kernel->machine->WriteRegister(2, 0);
				}
				else
				{
					DEBUG('u', "Id socket invalid!");
					kernel->machine->WriteRegister(2, -1);
				}
			}

			DEBUG('u', "System call CloseSoc finish");
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			break;

		case SC_Bind:
			DEBUG('u', "-------------------------\nSystem call Bind")
			int serverid;
			DEBUG('u', "Read Id socket server");
			serverid = kernel->machine->ReadRegister(4);
			if (serverid < 0 || kernel->fileSystem->findIndexSocket(serverid) == -1)
			{
				DEBUG('u', "Id socket is not exist!");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				struct sockaddr_in server;
				DEBUG('u', "Init Port");
				int port = 1234;
				int len;
				DEBUG('u', "Init Socket Server");
				server.sin_family = AF_INET;
				server.sin_addr.s_addr = INADDR_ANY;
				server.sin_port = htons(port);
				len = sizeof(server);
				if (bind(serverid, (struct sockaddr *)&server, len) < 0)
				{
					DEBUG('u', "Cannot bind sokcet");
					kernel->machine->WriteRegister(2, -1);
				}
				else
				{
					char *server_ip = inet_ntoa(server.sin_addr);
					DEBUG('u', "Bind Socket successfull " << server_ip);
					kernel->machine->WriteRegister(2, 0);
				}
			}

			DEBUG('u', "System call Bind finish");
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			break;

		case SC_Listen:
			DEBUG('u', "-------------------------\nSystem call Listen")
			DEBUG('u', "Read Id socket server");
			serverid = kernel->machine->ReadRegister(4);
			if (serverid < 0 || kernel->fileSystem->findIndexSocket(serverid) == -1)
			{
				DEBUG('u', "Id socket is not exist!");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				int count;
				count = kernel->machine->ReadRegister(5);
				if (listen(serverid, count) < 0)
				{
					DEBUG('u', "Listen Error");
					kernel->machine->WriteRegister(2, -1);
				}
				else
				{
					DEBUG('u', "Listen ...");
					kernel->machine->WriteRegister(2, 0);
				}
			}

			DEBUG('u', "System call Listen finish");
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			break;

		case SC_Accept:
			DEBUG('u', "-------------------------\nSystem call Accept")
			DEBUG('u', "Read Id socket server");
			serverid = kernel->machine->ReadRegister(4);
			if (serverid < 0 || kernel->fileSystem->findIndexSocket(serverid) == -1)
			{
				DEBUG('u', "Id socket is not exist!");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				socklen_t len;
				int idclient;
				struct sockaddr_in client;
				len = sizeof(client);
				DEBUG('u', "Wating for client");
				if ((idclient = accept(serverid, (struct sockaddr *)&client, &len)) < 0)
				{
					DEBUG('u', "Error while accepting!");
					kernel->machine->WriteRegister(2, -1);
				}
				else
				{
					char *client_ip = inet_ntoa(client.sin_addr);
					DEBUG('u', "Accept id: " << idclient << " ip: " << client_ip);
					kernel->machine->WriteRegister(2, idclient);
				}
			}

			DEBUG('u', "System call Accept finish");
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			break;

		case SC_Read:
		{
			DEBUG('u', "-------------------------\nSystem call Read\n");
			int bufferAddr = kernel->machine->ReadRegister(4);
			int size = kernel->machine->ReadRegister(5);
			int fileId = kernel->machine->ReadRegister(6);

			int bytesRead;
			char *buf = new char[size];

			if (fileId < 0 || fileId > 20)
			{
				DEBUG('u', "File id is out of range");
				kernel->machine->WriteRegister(2, -1);
			}
			else if (fileId == 0)
			{

				bytesRead = SysRead(buf, size, fileId);
				kernel->machine->WriteRegister(2, bytesRead);
				System2User(bufferAddr, size, buf);
			}
			else
			{

				buf = User2System(bufferAddr, size);
				bytesRead = SysRead(buf, size, fileId);
				kernel->machine->WriteRegister(2, bytesRead);
				System2User(bufferAddr, bytesRead, buf);
			}
			DEBUG('u', "System call Read finish");
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

			if (fileId < 0 || fileId > 20)
			{
				DEBUG('u', "File id is out of range");
				kernel->machine->WriteRegister(2, -1);
			}
			else if (fileId == 1)
			{
				buf = User2System(bufferAddr, size);
				bytesWritten = SysWrite(buf, size, fileId);
				kernel->machine->WriteRegister(2, bytesWritten);
				System2User(bufferAddr, bytesWritten, buf);
			}
			else if (kernel->fileSystem->openfile[fileId]->type == 1)
			{
				DEBUG('u',"Writing no allowed ");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				buf = User2System(bufferAddr, size);
				bytesWritten = SysWrite(buf, size, fileId);
				kernel->machine->WriteRegister(2, bytesWritten);
				System2User(bufferAddr, bytesWritten, buf);
				DEBUG('u',"Write successfull");
			}
			DEBUG('u',"System call Read finish");
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

			if (fileId < 2 || fileId > 20)
			{
				DEBUG('u', "File with ID " << fileId << " is out of range \n");
				kernel->machine->WriteRegister(2, -1);
			}
			else if (kernel->fileSystem->openfile[fileId] == NULL)
			{
				DEBUG('u', "File is not exist");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				pos = (pos == -1) ? kernel->fileSystem->openfile[fileId]->Length() : pos;
				if (pos > kernel->fileSystem->openfile[fileId]->Length() || pos < 0)
				{
					DEBUG('u', "Can't seek to this position.");
					kernel->machine->WriteRegister(2, -1);
				}
				else
				{
					kernel->fileSystem->openfile[fileId]->Seek(pos);
					kernel->machine->WriteRegister(2, pos);
				}
			}
			DEBUG('u', "System call Seek finish");

			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			return;
			// ASSERTNOTREACHED();
			break;
		}

		case SC_PrintString:
		{
			int virtAddr;
			char *buffer;
			virtAddr = kernel->machine->ReadRegister(4);
			buffer = User2System(virtAddr, 255);
			int length = 0;
			while (buffer[length] != 0)
				length++;

			delete buffer;

			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			break;
		}

		case SC_Toupper:
			DEBUG('u', "System call Toupper start!");
			firstAdd = kernel->machine->ReadRegister(4);
			char *data;
			data = User2System(firstAdd, 1024);
			if (data == NULL)
			{
				DEBUG('u', "Not enough memory");
				kernel->machine->WriteRegister(2, -1);
			}
			DEBUG('u', "Length data: " << strlen(data));
			for (int i = 0; i < strlen(data); i++)
			{
				data[i] = toupper(data[i]);
			}
			DEBUG('u', "Upper data successfull " << strlen(data));
			System2User(firstAdd, strlen(data), data);
			kernel->machine->WriteRegister(2, 0);
			delete data;
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			break;

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
