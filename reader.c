#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<string.h>
#define SHM_SIZE 1024

int main(){
    int shmid;
    char *shmaddr;
    char read_buf[SHM_SIZE];

    //获取共享内存段
    shmid=shmget((key_t)1234,SHM_SIZE,0666|IPC_CREAT);
    if(shmid==-1){
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    //将共享内存段连接到当前进程
    shmaddr=(char*)shmat(shmid,0,0);
    if(shmaddr==(void*)-1){
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    //从共享内存读取数据并输出到标准输出
    while(1){
        strncpy(read_buf,shmaddr,SHM_SIZE);
        printf("received message:%s\n",read_buf);
        if(strncmp(read_buf,"exit",4)==0){
            printf("received exit\n");
            break;
        }
        sleep(1);
    }

    if(shmdt(shmaddr)==-1){
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }

    printf("read exit\n");
    return 0;
}
