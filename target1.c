#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

void foo(char *buf) {
    char a;
    a = buf[0];
    a = buf[4];
    a = buf[8];
    a = buf[10];

    buf[5]  = 't';
    buf[10] = 'e';
    buf[20] = 's';
    buf[30] = 't';
}

int main(int ac, char **av) {
    int fd;
    char *buf;

    if (!(buf = malloc(256)))
        return -1;
    
    fd = open("./file.txt", O_RDONLY);
    read(fd, buf, 256);
    close(fd);

    foo(buf);
}