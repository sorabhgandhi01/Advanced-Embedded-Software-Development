#include "my_thread.h"
#include <sys/types.h>
#include <sys/syscall.h>

long double utilization;
timer_t timer_id;

char file_name[20];
void t_delay(int interval_ns);

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

void child1_handler(int num)
{
    char print_msg[128];
    snprintf(print_msg, 128, "Timestamp: %lu *** child_Thread1: Child thread1 exiting", (unsigned long)time(NULL));
    log_msg(file_name, print_msg);

    pthread_cancel(thread[1]);
    exit(0);
}

void child2_handler(int num)
{
    char print_msg[128];
    snprintf(print_msg, 128, "Timestamp: %lu *** child_Thread2: Child thread2 exiting", (unsigned long)time(NULL));
    log_msg(file_name, print_msg);

    pthread_cancel(thread[2]);
    exit(0);
}

void my_timer_handler (union sigval val)
{
	char print_msg[128];
	memset(print_msg, 0, sizeof(print_msg));

	sprintf(print_msg, "Total CPU Utilization time = %Lf\n", utilization);
	printf("%s\n", print_msg);
    log_msg(file_name, print_msg);
	
	t_delay(100000000);
}

void t_delay(int interval_ns)
{
   struct itimerspec in;

	in.it_value.tv_sec = 0;
    in.it_value.tv_nsec = interval_ns; 
    in.it_interval.tv_sec = 0;
    in.it_interval.tv_nsec = interval_ns;
    
    if ((timer_settime(timer_id, 0, &in, NULL)) != 0) {
    	perror("Timer setup failed");
    	exit(EXIT_FAILURE);
    }
}

void count_character (char *str, int length)
{
    int count[26] = {0};
    char print_msg[128];
    int i = 0;

    for (i = 0; i < length; i++)
    {
        if ((str[i] >= 65) && (str[i] <= 90))
        {
            count[str[i] - 65]++;
        }
            
        if ((str[i] >= 97) && (str[i] <= 122))
        {
            count[str[i] - 97]++;
        }
    }

    for (i = 0; i < 26; i++)
    {
        if ((count[i] > 0) && (count[i] < 100))
        {
            printf("Count for character '%c' = %d\n", (i + 65), count[i]);

            snprintf(print_msg, 128, "Count for character '%c' = %d\n", (i + 65), count[i]);
            log_msg(file_name, print_msg);
        }
    }
}


long double get_CPU_UTIL_time()
{
    long double a[4], b[4], loadavg;
    FILE *fp;
    char dump[50];

    fp = fopen("/proc/stat","r");
    fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
    fclose(fp);
    /*usleep(100000);

    fp = fopen("/proc/stat","r");
    fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
    fclose(fp);*/

    loadavg = (((a[0]+a[1]+a[2])) / ((a[0]+a[1]+a[2]+a[3])));

    return loadavg;
}


void *child_thread1(void *arg)
{
    FILE *fptr;
    char *read_buff = NULL;
    size_t r_bytes, f_size;

    struct thread_data *data = (struct thread_data *)arg;

    signal(SIGUSR1, child1_handler);
	signal(SIGUSR2, child1_handler);

    printf("In Child thread_1 having Thread des = %ld, PID = %ld, Thread ID = %ld, Process ID = %d\n", thread[1], pthread_self(), syscall(SYS_gettid), getpid());

    fptr = fopen(data->logfile, "a+");
    if (fptr == NULL) {
        perror("File open error_1:");
    }
    char print_msg[512];
    snprintf(print_msg, 512, "Timestamp: %lu *** Child_Thread1: Thread des = %ld, PID = %ld, Thread ID = %ld, Process ID = %d\n", (unsigned long)time(NULL), thread[1], pthread_self(), syscall(SYS_gettid), getpid());
    fwrite(print_msg, sizeof(char), strlen(print_msg), fptr);
    fclose(fptr);

    fptr = fopen(data->readfile, "r");
    if (fptr == NULL) {
        perror("File open error1:");
    }
    else {

        fseek(fptr, 0, SEEK_END);
        f_size = ftell(fptr);
        rewind(fptr);

        read_buff = (char *) malloc(sizeof(char) * (f_size + 1));

        r_bytes = fread(read_buff, sizeof(char), f_size, fptr);

        if (r_bytes != f_size) {
            free(read_buff);
        }

        count_character(read_buff, r_bytes);
        free(read_buff);
    }

    fclose(fptr);
    pthread_cancel(thread[1]);
}


void *child_thread2(void *arg)
{
    FILE *fptr;
    char write_buffer[512];

    struct thread_data *data = (struct thread_data *)arg;

    signal(SIGUSR1, child2_handler);
    signal(SIGUSR2, child2_handler);

    printf("In Child thread_2 having Thread des = %ld, PID = %ld, Thread ID = %ld, Process ID = %d\n", thread[2], pthread_self(), syscall(SYS_gettid), getpid());

    fptr = fopen(data->logfile, "a+");
    if (fptr == NULL) {
        perror("File open error_2:");
    }
    char print_msg[512];
    snprintf(print_msg, 512, "Timestamp: %lu *** Child_Thread2: Thread des = %ld, PID = %ld, Thread ID = %ld, Process ID = %d\n", (unsigned long)time(NULL), thread[2], pthread_self(), syscall(SYS_gettid), getpid());
    fwrite(print_msg, sizeof(char), strlen(print_msg), fptr);
    fclose(fptr);

    t_delay(100000000);

    while(1) {

    /*fptr = fopen(data->logfile, "a+");
    if (fptr == NULL) {
        perror("File open error2:");
    }*/
    
        utilization = get_CPU_UTIL_time();

       /* printf("Total CPU Utilization time = %Lf\n", utilization);
        snprintf(write_buffer, 512, "TImestamp: %lu *** Total CPU Utilization time = %Lf\n", (unsigned long)time(NULL), utilization);

        fwrite(write_buffer, sizeof(char), strlen(write_buffer), fptr);

    fclose(fptr);*/
    }
}


void *master_thread(void *arg)
{
    struct thread_data *data = (struct thread_data *)arg;
    FILE *fptr;

    printf("In master thread having Thread des = %ld, PID = %ld, Thread ID = %ld, Process ID = %d\n", thread[0], pthread_self(), syscall(SYS_gettid), getpid());
    
    fptr = fopen(data->logfile, "a+");
    if (fptr == NULL) {
        perror("File open error3:");
    }
    char print_msg[512];
    snprintf(print_msg, 512, "Timestamp: %lu *** Master_Thread: Thread des = %ld, PID = %ld, Thread ID = %ld, Process ID = %d\n", (unsigned long)time(NULL), thread[0], pthread_self(), syscall(SYS_gettid), getpid());
    fwrite(print_msg, sizeof(char), strlen(print_msg), fptr);
    fclose(fptr);



    memset(print_msg, 0, sizeof(print_msg));
    fptr = fopen(data->logfile, "a+");
    if (fptr == NULL) {
        perror("File open error_3:");
    }

    snprintf(print_msg, 512, "Timestamp: %lu *** Master_Thread: Master thread exiting", (unsigned long)time(NULL));
    fwrite(print_msg, sizeof(char), strlen(print_msg), fptr);
    fclose(fptr);

    pthread_cancel(thread[0]);
}

int main(int argc, char **argv)
{

    if ((argc < 2) || (argc > 2)) {
		printf("Usage --> ./[%s] [LOG FILE NAME]\n", argv[0]);		//Should have a port number > 5000
		exit(EXIT_FAILURE);
	}

    t_data data;
    strcpy(data.readfile, "gdb.txt");
    strcpy(data.logfile, argv[1]);
    strcpy(file_name, argv[1]);

    printf("Filename = %s\n", data.logfile);

    pthread_attr_t attr;
    int rc = 0;

    struct 	sigevent sev;
	sev.sigev_notify = SIGEV_THREAD; 
	sev.sigev_notify_function = &my_timer_handler;
	sev.sigev_notify_attributes = NULL;
	sev.sigev_value.sival_ptr = &timer_id;


	if(timer_create(CLOCK_REALTIME, &sev, &timer_id) != 0)
	{
		perror("Timer:");
        exit(EXIT_FAILURE);
	}

    if((rc = pthread_create(&thread[0], NULL, master_thread, (void *)&data)) != 0)  		
    {
        perror("Thread create:");
        exit(EXIT_FAILURE);
    }

    if((rc = pthread_create(&thread[1], NULL, child_thread1, (void *)&data)) != 0)
    {
        perror("Thread create:");
        exit(EXIT_FAILURE);
    }

    if((rc = pthread_create(&thread[2], NULL, child_thread2, (void *)&data)) != 0)
    {
        perror("Thread create:");
        exit(EXIT_FAILURE);
    }

    pthread_join(thread[0],NULL);
    pthread_join(thread[1],NULL);
    pthread_join(thread[2],NULL);

    return 0;
}
