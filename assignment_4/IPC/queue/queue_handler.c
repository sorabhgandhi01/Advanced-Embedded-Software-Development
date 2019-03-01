
/*
 * Reference  https://users.pja.edu.pl/~jms/qnx/help/watcom/clibref/mq_overview.html
 *
*/

#include "queue_handler.h"

size_t size = sizeof(msg_q);

void client_queue(msg_q msg, char *name)
{
    struct mq_attr attr;
    mqd_t mq;

    //First we need to set up the attribute structure
    attr.mq_maxmsg = MAX_SIZE;
    attr.mq_msgsize = size;
    attr.mq_flags = 0;
    attr.mq_curmsgs = 5;

    mq = mq_open(name, O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t) -1) {

        perror("Server: Queue Open Error");
        exit(EXIT_FAILURE);
    }

    //printf("Size = %d\n", size);

    if ((mq_send(mq, (char *) &msg, size, 0)) == -1) {

        perror("Client: Message send error");
        exit(EXIT_FAILURE);
    }

    mq_close(mq);
    mq_unlink(name);
}

void server_queue (msg_q msg, char *name)
{
    struct mq_attr attr;
    mqd_t mq;

   /* if(mq_unlink(name) == 0)
        fprintf(stdout, "Message queue %s removed from system.\n", name);*/

    //First we need to set up the attribute structure
    attr.mq_maxmsg = MAX_SIZE;
    attr.mq_msgsize = size;
    attr.mq_flags = 0;
    attr.mq_curmsgs = 5;

    mq = mq_open(name, O_CREAT | O_RDONLY, 0666, &attr);
    if (mq == (mqd_t) -1)
    {
        perror("Server: Queue Open Error");
        exit(EXIT_FAILURE);
    }

    if ((mq_receive(mq, (char *)&msg, size, 0)) == -1) {

       perror("Server: Message Recieve error");
       exit(EXIT_FAILURE);
    } 

    printf("Message --> %s\n", msg.message);
    printf("Buffer Size --> %ld\n", msg.message_length);

    mq_close(mq);
    mq_unlink(name);
}
