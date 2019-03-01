#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

//#define QUEUE_NAME  "/my_queue"
#define BUFFER_SIZE 200
#define MAX_SIZE 5

typedef struct {
    char message[24];
    size_t message_length;
    uint8_t LED_status;
}msg_q;

void server_queue(msg_q msg, char *name);

void client_queue(msg_q msg, char *name);
