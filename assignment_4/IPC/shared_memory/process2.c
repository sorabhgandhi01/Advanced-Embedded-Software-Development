#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>

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
#define SHM_NAME "my_sm"

struct shared_memory
{
	uint8_t is_ready;
	char message[MSG_SIZE];
}shared_memory;

char msg_buf[BUFF_SIZE];
struct shared_memory *data;
int sfd;

void log_msg(char *filename, char *msg)
{
    FILE *fptr;
    char write_buffer[512];

    fptr = fopen(filename, "a+");
    if (fptr == NULL) {
        perror("File open error:");
    }

    snprintf(write_buffer, 512, "Timestamp: %lu *** %s\n", (unsigned long)time(NULL), msg);
    fwrite(write_buffer, sizeof(char), strlen(write_buffer), fptr);

    fclose(fptr);
}

void signal_handler(int num)
{
    shm_unlink(SHM_NAME);
    log_msg("log.txt", "SIG Handler: Killing the Server in handler");
    exit(0);
}

int main(int argc, char **argv)
{
    signal(SIGINT, signal_handler);
    char print_msg[BUFF_SIZE];
    char command[32];
    size_t size = sizeof(struct shared_memory);

    sfd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (sfd == -1) {
        perror("Server: open error");
        exit(EXIT_FAILURE);
    }

    snprintf(print_msg, 512, "IPC = SHARED MQ [SERVER] <---> Process ID = %d   FD = %d\n", getpid(), sfd);
    log_msg("log.txt", print_msg);

    if ((ftruncate(sfd, size)) < 0) {
        perror("Server: ftruncate error");
        exit(EXIT_FAILURE);
    }

    data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED , sfd, 0);
    if (data < 0) {
        perror("Server: Memory map error");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        memset(print_msg, 0, sizeof(print_msg));
        data->is_ready = 0;

        while(data->is_ready != 1) {
            sleep(1);
        }

        printf("Server: Message recieved from client = %s   length = %ld\n", data->message, strlen(data->message));

        snprintf(print_msg, 512, "Server: Recieved msg from Client = %s    length = %ld", data->message, strlen(data->message));
        log_msg("log.txt", print_msg);

        sscanf(data->message, "%s", command);
        if (strcmp(command, "LON") == 0) {
            printf("LED turned ON\n");
            log_msg("log.txt", "Server: Led turned ON");
        }
        if (strcmp(command, "LOFF") == 0) {
            printf("LED turned OFF\n");
            log_msg("log.txt", "Server: Led turned OFF");
        }

        memset(data->message, 0, sizeof(shared_memory));
        //data->is_ready = 0;

        printf("Server: Enter the message to be sent to client\n");
        scanf(" %[^\n]%*c", data->message);

        data->is_ready = 1;
    }

    return 0;
}
