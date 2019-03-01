#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define NUM_THREAD 3

pthread_t thread[NUM_THREAD];

typedef struct thread_data
{
	char readfile[24];
    char logfile[24];
} t_data;
