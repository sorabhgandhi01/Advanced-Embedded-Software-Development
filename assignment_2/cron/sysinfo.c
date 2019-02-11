#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int mysyscall()
{
    printf("/n/nStarting the user space program\n");
    clock_t t;

    srandom(time(NULL));
    int32_t len = (int32_t)(random() % 256);

    int32_t src[len];
    int32_t dst[len];
    int32_t i = 0;

    printf("Printing the unsorted array in User-Space\n");

    for (i = 0; i < len; i++) {
        src[i] = (int32_t)(random() % 1000);
        printf("Element number %d in User-Space memory %p = %d\n", i, &src[i], src[i]);
    }

    t = clock();
    long int status = syscall(398, src, dst, len);
    t = (clock() - t);
    printf("Total time taken for system call -> %f\n", ((double)t/CLOCKS_PER_SEC));

    if (status == 0) {
        printf("Success\n");

        printf("Printing the sorted array in User-Space\n");

        for (i = 0; i < len; i++) {
            printf("Element number %d in User-Space memory %p = %d\n", i, &src[i], src[i]);
        }
    }
    else {
        perror("System call error:");
    }

    printf("*******************************************************************************\n\n");

    return 0;
}

int main()
{
    struct timeval t;
    time_t time_now;
    printf("The Current Process ID: %d\n", getpid());
    printf("The Current User ID: %d\n", getuid());

    gettimeofday(&t, NULL);

    time_now = t.tv_sec;

    printf("The Current Date and time: %s\n", ctime(&time_now));

    mysyscall();

    return 0;
}
