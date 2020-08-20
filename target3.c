#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int foo(char *buf) {
    if (buf[0] != 'A')
        return 0;
    if (buf[1] != 'B')
        return 0;
    if (buf[2] != 'C')
        return 0;
    if (buf[3] != 'D')
        return 0;
    return 1; 
}

int main(int ac, char **av) {
    int fd;
    char *buf;

    if (!(buf = malloc(32)))
        return -1;
    
    fd = open("./file.txt", O_RDONLY);
    read(fd, buf, 32);
    close(fd);

    foo(buf);
}