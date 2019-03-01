#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>

#define BUFF_SIZE 128
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

void server_handler(int num)
{
    log_msg("log.txt", "Server: Killing the server process");

    close(sfd);

    exit(0);
}


int main(int argc, char **argv)
{
    signal(SIGINT, server_handler);

    if ((argc < 2) || (argc > 2)) {				
		printf("Usage --> ./[%s] [Port Number]\n", argv[0]);		//Should have a port number > 5000
		exit(EXIT_FAILURE);
	}

    struct sockaddr_in send_addr, from_addr;
    char cmd_send[128];
    char print_msg[128];
    char command[32];
    ssize_t numRead = 0;

    int new_conn = 0;
    socklen_t len = sizeof(from_addr);

    FILE *fptr;

    /*Clear all the data buffer and structure*/
    memset(&send_addr, 0, sizeof(send_addr));
    memset(&from_addr, 0, sizeof(from_addr));

    /*Populate send_addr structure with IP address and Port*/
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(atoi(argv[1]));
    send_addr.sin_addr.s_addr = INADDR_ANY;

    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Server: socket");
        exit(EXIT_FAILURE);
    }

    snprintf(print_msg, BUFF_SIZE, "IPC = SOCKET <--> Process ID = %d <---> FD = %d", getpid(), sfd);
    log_msg("log.txt", print_msg);

    if((bind(sfd, (struct sockaddr*) &send_addr, sizeof(send_addr))) < 0) {
        perror("Server: bind");
        exit(EXIT_FAILURE);
    }

    /*Listening for clients*/
    if(listen(sfd, 5) < 0) {
        perror("Server: listen");
        exit(EXIT_FAILURE);
    }
       

    /*Accepting Client connection*/
    new_conn = accept(sfd, (struct sockaddr*) &from_addr, &len);
    if(new_conn < 0) {
        perror("Server: accept");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        memset(print_msg, 0, sizeof(print_msg));

        recv(new_conn, cmd_send, BUFF_SIZE, 0);
        printf("Server: %s\n", cmd_send);
        snprintf(print_msg, BUFF_SIZE, "Server: Recieved msg from Client = %s    length = %ld", cmd_send, strlen(cmd_send));
        log_msg("log.txt", print_msg);

        sscanf(cmd_send, "%s", command);
        if (strcmp(command, "LON") == 0) {
            printf("LED turned ON\n");
            log_msg("log.txt", "Server: Led turned ON");
        }
        if (strcmp(command, "LOFF") == 0) {
            printf("LED turned OFF\n");
            log_msg("log.txt", "Server: Led turned OFF");
        }

        memset(cmd_send, 0, sizeof(cmd_send));

        printf("\nServer: Enter the message to pass from server to client\n");
        scanf(" %[^\n]%*c", cmd_send);

        send(new_conn, cmd_send, BUFF_SIZE, 0);
        memset(cmd_send, 0, sizeof(cmd_send));
    }

    close(sfd);

    return 0;
}
