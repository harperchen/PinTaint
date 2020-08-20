
#include <stdlib.h>

int main(int ac, char **av) {
    char *buf;
    char c;

    if(!(buf = malloc(32)))
        return -1;
   
    c = buf[0];
    free(buf);
    
    c = buf[0];
    buf = malloc(32);
    
    c = buf[0];
}