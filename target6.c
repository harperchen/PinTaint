#include <stdlib.h>
#include <stdio.h>

int main(int ac, char **av)
{
    char c, *buf;
    
    if (!(buf = malloc(32)))
        return -1;
        
    c = buf[0];
    printf("%x\n", c);
}