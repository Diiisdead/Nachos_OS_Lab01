#include "syscall.h"

int main()
{
    int serverid,clientid;
    char buffer[100];
    int lendataRcv;
    int lendataSend;
    int check;
    check = 0;
    serverid = SocketTCP();
    Bind(serverid);
    Listen(serverid,10);
    Write("Dang lang nghe\n",100,1);
    while(1){
        clientid = Accept(serverid);
        if(clientid > 0){
            lendataRcv = -1;
             Write("Du lieu nhan duoc:\n",100,1);
            while(lendataRcv !=0){
                lendataRcv = Receive(clientid,buffer,100);
                if(lendataRcv!=0){
                    Write(buffer,100,1);
                    check = 1;
                    Toupper(buffer);
                    Send(clientid,buffer,lendataRcv);
                }
            }
            if (check == 1){
                 Write("\nGui du lieu thanh cong\n\n",100,1);
            }

        }
    }
    CloseSoc(serverid);
    Halt();
    return;
    /* not reached */
}