#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    printf("/n/nStarting the user space program\n");
    clock_t t;

#ifdef VALID_INPUT
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
#endif

#ifdef INVALID_INPUT_BUFFER
    int32_t *src;
    int32_t dst[4];
    int32_t len = 4;
    int32_t i = 0;
#endif

#ifdef INVALID_INPUT_LENGTH
    int32_t src[4];
    int32_t dst[4];
    int32_t len = -1;
    int32_t i = 0;
#endif

#ifdef INVALID_OUTPUT_BUFFER
    int32_t src[4];
    int32_t *dst;
    int32_t len = 4;
    int32_t i = 0;
#endif

    t = clock();
    long int status = syscall(398, src, dst, len);
    t = (clock() - t);
    printf("Total time taken for system call -> %f seconds\n", ((double)t/CLOCKS_PER_SEC));

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

    return 0;
}
