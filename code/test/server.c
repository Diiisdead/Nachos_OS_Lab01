#include "syscall.h"

int main()
{
    int serverid;
    int checkuper;
    int lendatasend;
    int clientid;
    char buffer[100];
    int lendataRcv;
    serverid = SocketTCP();
    Bind(serverid);
    Listen(serverid,10);
    Write("Dang lang nghe\n",100,1);
    while(1){
        clientid = Accept(serverid);
        if(clientid > 0){
            lendataRcv = Receive(clientid,buffer,1024);
            if (lendataRcv >0){
                Write("Du lieu server nhan duoc tu client: ",50,1);
                Write(buffer,lendataRcv,1);
                Write("\n",1,1);
                checkuper = Toupper(buffer);
                if (checkuper ==0){
                    Write("Du lieu sau khi chuyen doi: ",50,1);
                    Write(buffer,lendataRcv,1);
                    Write("\n",1,1);
                    lendatasend = Send(clientid,buffer,lendataRcv);
                    if(lendatasend<0){
                        Write("ko the gui du lieu: ",50,1);
                    }
                }
                else{
                    Write("Chuyen doi du lieu that bai\n",50,1);
                }
            }
            else{
              
                Write("Nhan du lieu that bai\n",50,1);
            }
            Write("\n",1,1);
        }
                
    }
    CloseSoc(serverid);
    Halt();
    return;
    /* not reached */
}
