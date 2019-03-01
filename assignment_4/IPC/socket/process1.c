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
int cfd;

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

void client_handler(int num)
{
    log_msg("log.txt", "Client: Killing the client process");

    close(cfd);
    exit(0);
}

int main(int argc, char **argv)
{
    signal(SIGINT, client_handler);

	if ((argc < 3) || (argc >3)) {
		printf("Client: Usage --> ./[%s] [IP Address] [Port Number]\n", argv[0]);  //Should have the IP of the server
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in send_addr;
	char cmd_send[128];
    char print_msg[128];
	ssize_t numRead = 0;

	FILE *fptr;

	/*Clear all the data buffer and structure*/
	memset(&send_addr, 0, sizeof(send_addr));

	/*Populate send_addr structure with IP address and Port*/
	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(atoi(argv[2]));
	send_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("CLient: socket");
        exit(EXIT_FAILURE);
    }

    if((connect(cfd, (struct sockaddr *) &send_addr, sizeof(send_addr))) < 0) {
        perror("Client: connect");
        exit(EXIT_FAILURE);
    }

    snprintf(print_msg, BUFF_SIZE, "IPC = SOCKET <--> Process ID = %d <---> FD = %d", getpid(), cfd);
    log_msg("log.txt", print_msg);

    while(1)
    {
        memset(print_msg, 0, sizeof(print_msg));
        printf("\nClient: Enter the message to pass from client to server\n");
		scanf(" %[^\n]%*c", cmd_send);

        send(cfd, cmd_send, BUFF_SIZE, 0);
        memset(cmd_send, 0, sizeof(cmd_send));

        recv(cfd, cmd_send, BUFF_SIZE, 0);
        printf("Client: %s\n", cmd_send);
        snprintf(print_msg, BUFF_SIZE, "Client: Recieved msg from server = %s    length = %ld", cmd_send, strlen(cmd_send));
        log_msg("log.txt", print_msg);

        memset(cmd_send, 0, sizeof(cmd_send));
    }

    close(cfd);

    return 0;
}
