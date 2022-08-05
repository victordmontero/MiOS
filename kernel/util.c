
#include <util.h>

void memory_copy(char* src, char* dest, int no_bytes)
{
    int i;
    for (i = 0; i < no_bytes; i++)
    {
        *(dest+i) = *(src+i);
    }
    
}