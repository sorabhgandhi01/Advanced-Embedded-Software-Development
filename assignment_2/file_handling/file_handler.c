#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    if ((argc < 3) || (argc > 3)) {
        printf("Usage --> ./[%s] [FILE NAME] [Enter a String]\n", argv[0]);
        exit(-1);
    }

    FILE *fd;
    ssize_t w_byte;
    char sample_char[1] = "a";
    char samp[20];
    int ch;

    fd = fopen(argv[1], "w+");
    if (fd == NULL)
        perror("File Open error\n");

    fclose(fd);

    fd = fopen(argv[1], "w");
    if (fd == NULL)
        perror("File Open error\n");

    w_byte = fwrite(sample_char, 1, 1, fd);
    if (w_byte == -1)
        perror("File Write error\n");

    fclose(fd);

    fd - fopen(argv[1], "a");
    if (w_byte == -1)
        perror("File Write error\n");

    char *string = (char *) malloc(strlen(argv[2]) * sizeof(char));
    sprintf(string, "%s", argv[2]);
    printf("String = %s\n", argv[2]);

    w_byte = fwrite(string, 1, strlen(string), fd);
    if (w_byte == -1)
        perror("File Write error\n");

    fflush(stdin);

    fclose(fd);

    fd - fopen(argv[1], "r");
    if (w_byte == -1)
        perror("File Write error\n");

    ch = getc(fd);
    printf("%c\n", (char)ch);

    while(fgets(samp, 20, fd)) {
        printf("%s\n", samp);
    }   

    fclose(fd);

    free(string);
}
