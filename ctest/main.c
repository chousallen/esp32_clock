#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint16_t buff_o[2] = {0};

int main()
{
    int16_t x = 0xffff;
    printf("x = %u\n", x);
    return 0;
    
}