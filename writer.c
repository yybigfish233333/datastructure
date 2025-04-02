#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#define SHM_SIZE 1024

int main()
{
    int shmid;
    char *shmaddr;
    char write_buf[SHM_SIZE];

    shmid = shmget((key_t)1234, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    // 共享内存连接到当前进程
    shmaddr = (char *)shmat(shmid, 0, 0);
    if (shmaddr == (void *)-1)
    {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    // 从标准输入读取数据并将其写入到共享内存
    while (1)
    {
        fgets(write_buf, SHM_SIZE, stdin);
        strncpy(shmaddr, write_buf, SHM_SIZE);
        if (strncmp(write_buf, "exit", 4) == 0)
        {
            break;
        }
    }

    // 断开共享内存连接
    if (shmdt(shmaddr) == -1)
    {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }

    // 删除共享内存段
    if (shmctl(shmid, IPC_RMID, 0) == -1)
    {
        perror("shmctl failed");
        exit(EXIT_FAILURE);
    }

    printf("write exit\n");
    return 0;
}
