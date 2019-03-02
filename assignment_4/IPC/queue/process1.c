#include "queue_handler.h"
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>


#define queue_name "/my_test_queue"
mqd_t mq;
size_t size = sizeof(msg_q);

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

void queue_handle(int num)
{
    log_msg("log.txt", "SIG Handler: Killing the client process in handler");
    mq_close(mq);
    mq_unlink(queue_name);
    exit(0);
}

int main()
{
    signal(SIGINT, queue_handle);
    msg_q msg_send, msg_recieve = {0};
    char cmd_send[24];
    char command[32];
    struct mq_attr attr;

    //First we need to set up the attribute structure
    attr.mq_maxmsg = MAX_SIZE;
    attr.mq_msgsize = size;
    attr.mq_flags = 0;
    attr.mq_curmsgs = 5;

    mq = mq_open(queue_name, O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t) -1) {

        perror("Server: Queue Open Error");
        exit(EXIT_FAILURE);
    }

    char print_msg[128];
    snprintf(print_msg, 128, "IPC = MQ (Client) <--> Process ID = %d, FD = %d", getpid(), mq);
    log_msg("log.txt", print_msg);

    while(1) {

        printf("\nClient: Enter the message to send to the Server\n");
        scanf(" %[^\n]%*c", cmd_send);
        strcpy(msg_send.message, cmd_send);
        msg_send.message_length = strlen(msg_send.message);

        if ((mq_send(mq, (char *) &msg_send, size, 0)) == -1) {
            perror("Client: Message send error");
            exit(EXIT_FAILURE);
        }

        memset(&msg_send, 0, sizeof(msg_send));
        
        if ((mq_receive(mq, (char *)&msg_recieve, size, 0)) == -1) {
            perror("Client: Message Recieve error");
            exit(EXIT_FAILURE);
        }

        printf("Message --> %s  Buffer_len = %ld\n", msg_recieve.message, msg_recieve.message_length);

        snprintf(print_msg, 128, "Client: Recieved msg from server --> %s  length = %ld\n", msg_recieve.message, msg_recieve.message_length);
        log_msg("log.txt", print_msg);

        sscanf(msg_recieve.message, "%s", command);
        if (strcmp(command, "LON") == 0) {
            printf("LED turned ON\n");
            log_msg("log.txt", "Client: Led turned ON");
        }
        if (strcmp(command, "LOFF") == 0) {
            printf("LED turned OFF\n");
            log_msg("log.txt", "Client: Led turned OFF");
        }

        memset(&msg_recieve, 0, sizeof(msg_recieve));
    }

    return 0;
}
