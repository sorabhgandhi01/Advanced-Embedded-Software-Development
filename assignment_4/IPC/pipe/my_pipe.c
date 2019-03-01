#include "my_pipe.h"

#define BUFF_SIZE 128

int pipepd[2];
int pipecd[2];

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

void p_proc_handler(int num)
{
    log_msg("log.txt", "SIG Handler: Killing the parent process in handler");
    close(pipepd[1]);
   	close(pipepd[0]);

    exit(0);
}

void c_proc_handler(int num)
{
    log_msg("log.txt", "SIG Handler: Killing the child process in handler");
    close(pipecd[0]);
    close(pipecd[1]);

    exit(0);
}

int main (int argc, char **argv)
{
   // int pipepd[2];
   // int pipecd[2];
    pid_t cpid;
    char buf;
    char command[32];

    char p_msg[BUFF_SIZE];
    char c_msg[BUFF_SIZE];

    if (pipe(pipepd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipecd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {    /* Child reads from pipe */
    
        signal(SIGINT, c_proc_handler);
        char print_msg[128];
        snprintf(print_msg, BUFF_SIZE, "IPC = PIPE [CHILD] <--> Process ID = %d write_FD = %d Read_RD = %d", getpid(), pipecd[0], pipecd[1]);
        log_msg("log.txt", print_msg);
        
        while(1) {

                read(pipepd[0], c_msg, BUFF_SIZE);
                printf("Child: Recieved msg from parent = %s\n  length = %ld", c_msg, strlen(c_msg));
                snprintf(print_msg, BUFF_SIZE, "Child: Recieved msg from parent = %s    length = %ld", c_msg, strlen(c_msg));
                log_msg("log.txt", print_msg);

                sscanf(c_msg, "%s", command);
                if (strcmp(command, "LON") == 0) {
                    printf("LED turned ON\n");
                    log_msg("log.txt", "Child: Led turned ON");
                }
                if (strcmp(command, "LOFF") == 0) {
                    printf("LED turned OFF\n");
                    log_msg("log.txt", "Child: Led turned OFF");
                }

                memset(c_msg, 0, sizeof(c_msg));

                printf("\nChild: Enter the message to pass from child process\n");
                scanf(" %[^\n]%*c", c_msg);

                write(pipecd[1], c_msg, BUFF_SIZE);

                memset(c_msg, 0, sizeof(c_msg));
        }

    } else {            /* Parent writes argv[1] to pipe */

        signal(SIGINT, p_proc_handler);
        char print_msg[128];
        snprintf(print_msg, BUFF_SIZE, "IPC = PIPE [PARENT] <--> Process ID = %d write_FD = %d Read_FD = %d", getpid(), pipepd[0], pipepd[1]);
        log_msg("log.txt", print_msg);
       
        while(1) {

            printf("\nParent: Enter the message to pass from parent process\n");
		    scanf(" %[^\n]%*c", p_msg);

            write(pipepd[1], p_msg, BUFF_SIZE);

            memset(p_msg, 0, sizeof(p_msg));

            read(pipecd[0], p_msg, BUFF_SIZE);
            printf("Parent: Recieved msg from parent = %s   length = %ld\n", p_msg, strlen(p_msg));
            snprintf(print_msg, BUFF_SIZE, "Parent: Recieved msg from Child = %s    length = %ld", p_msg, strlen(p_msg));
            log_msg("log.txt", print_msg);

            sscanf(p_msg, "%s", command);
            if (strcmp(command, "LON") == 0) {
                printf("LED turned ON\n");
                log_msg("log.txt", "Parent: Led turned ON");
            }
            if (strcmp(command, "LOFF") == 0) {
                printf("LED turned OFF\n");
                log_msg("log.txt", "Parent: Led turned OFF");
            }

            memset(p_msg, 0, sizeof(p_msg));
        }
        
    }
}
