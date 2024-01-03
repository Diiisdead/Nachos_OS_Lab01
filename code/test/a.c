#include"syscall.h"

int main(int argc, char *argv[]){
    int i;
    for (i = 0;i<argc;i++){
        Write(argv[i],100,1);
    }
}