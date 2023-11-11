#include "syscall.h"

int main()
{
    int idSocket[4];
    int i;
    int status;
    char buffer1[100];
    char buffer2[100];
    char buffer3[100];
    char buffer4[100];
    char dataRecv[100];
    int lendata[4];
    int lendataRcv;
    // input data:
    Write("Nhap vao du lieu client1 can gui cho server: ",100,1);
    Read(buffer1,100,0);
    Write("\n",100,1);

    Write("Nhap vao du lieu client2 can gui cho server: ",100,1);
    Read(buffer2,100,0);
    Write("\n",100,1);

    Write("Nhap vao du lieu client3 can gui cho server: ",100,1);
    Read(buffer3,100,0);
    Write("\n",100,1);

    Write("Nhap vao du lieu client4 can gui cho server: ",100,1);
    Read(buffer4,100,0);
    Write("\n",100,1);


    for(i = 0;i<4;i++){
        idSocket[i] = SocketTCP();
        Connect(idSocket[i],"127.0.0.1",1234);
    }
    lendata[0] = Send(idSocket[0],buffer1,1024);
    lendata[1] =Send(idSocket[1],buffer2,1024);
    lendata[2] = Send(idSocket[2],buffer3,1024);
    lendata[3] = Send(idSocket[3],buffer4,1024);

    for(i =0 ;i<4;i++){
        if(lendata[i] <0){
            Write("gui du lieu that bai\n",50,1);
        }
        else if(lendata[i] == 0){
            Write("Ket noi bi dong\n ",50,1);
        }
        else{
            lendataRcv = Receive(idSocket[i],dataRecv,lendata[i]);
            if(lendataRcv <0){
                Write("Error: Nhan du lieu that bai\n",50,1);
            }
            else if(lendataRcv == 0){
                Write("Ket noi bi dong\n ",50,1);
            }
            else{
                Write("Du lieu gui ve tu server: ",50,1);
                Write(dataRecv,lendata[i],1);
                Write("\n",1,1);
            }
        }
    }
    CloseSoc(idSocket[i]);



    Halt();
    return;
    /* not reached */
}
