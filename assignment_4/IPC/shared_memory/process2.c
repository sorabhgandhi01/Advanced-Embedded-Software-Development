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
#define MSG_KEY (0x2323)
#define SHM_NAME "my_sm"

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
    sfd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (sfd == -1) {
        perror("Server: open error");
        exit(EXIT_FAILURE);
    }

    if ((ftruncate(sfd, sizeof(struct shared_memory))) < 0) {
        perror("Server: ftruncate error");
        exit(EXIT_FAILURE);
    }

    data = mmap(NULL, sizeof(struct shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED , sfd, 0);
    if (data < 0) {
        perror("Server: Memory map error");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        data->is_ready = 0;

        while(data->is_ready != 1) {
            sleep(1);
        }

        printf("Server: Message recieved from client = %s   length = %ld\n", data->message, strlen(data->message));

        memset(data, 0, sizeof(shared_memory));
        data->is_ready = 0;

        printf("Server: Enter the message to be sent to client\n");
        scanf(" %[^\n]%*c", data->message);

        data->is_ready = 1;
    }

    return 0;
}
