#include "syscall.h"

int main()
{
    int idSocket;
    int status;
    int isconnect;
    int lenfiledata;
    int lendataRev;
    int lendataSend;
    int lengthdata;
    char filename[25];
    char buffer[100];
    char dataRecv[100];

    idSocket = SocketTCP();
    if (idSocket == -1){
        Write("Error: Can not create Socket\n",100,1);
    }
    else{
        isconnect = Connect(idSocket,"127.0.0.1",1234);
        // input data:
        if (isconnect == 0)
        {
            Write("Nhap vao file can gui cho server: ",100,1);
            Read(filename,100,0);
            Write("\n\n",100,1);

            status = Open(filename,0);
            if(status !=-1){
                lenfiledata = Seek(-1,status);
                Seek(0,status);
                lendataSend = 0;
                while(lendataSend<lenfiledata){
                    lengthdata = Read(buffer,100,status);
                    Seek(lendataSend,status);
                    lendataSend += Send(idSocket,buffer,lengthdata);

                    lendataRev = Receive(idSocket,dataRecv,lendataSend);
                    Write(dataRecv,lendataRev,status);
                    Write(dataRecv,lendataRev,1);
                }
                Close(status);
            }
            else{
                Write("File is not exist!\n\n",100,1);
            }
        }
        else{
            Write("Error: Can not conect to server\n",100,1);
        }
        CloseSoc(idSocket);
    }
    Halt();
    return;
    /* not reached */
}