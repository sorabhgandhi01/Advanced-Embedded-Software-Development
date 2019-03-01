#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFF_SIZE (128)
#define MSG_SIZE (100)
#define SHM_NAME "/my_sm"
#define SEM_NAME "/my_sem"

struct shared_memory
{
	uint8_t is_ready;
	char message[MSG_SIZE];
}shared_memory;

char msg_buf[BUFF_SIZE];
struct shared_memory *data;
int sfd;


int main(int argc, char **argv)
{
    size_t size = sizeof(struct shared_memory);

    sfd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (sfd == -1) {
        perror("Client: open error");
        exit(EXIT_FAILURE);
    }

    if ((ftruncate(sfd, sizeof(struct shared_memory))) < 0) {
        perror("Client: ftruncate error");
        exit(EXIT_FAILURE);
    }

    data = mmap(NULL, sizeof(struct shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED , sfd, 0);
    if (data < 0) {
        perror("Client: Memory map error");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        data->is_ready = 0;
        memset(data, 0, sizeof(shared_memory));

        printf("Client: Enter the message to be sent to Server\n");
        scanf(" %[^\n]%*c", data->message);

        data->is_ready = 1;
        while(data->is_ready != 1) {
            sleep(1);
        }

        printf("Client: Message recieved from server = %s   length = %ld\n", data->message, strlen(data->message));
    }

    return 0;
}
