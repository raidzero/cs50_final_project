#include <stdlib.h>
#include <string.h>

// string.h strdup apparently doesnt exist in ansi C so here's mine
char* StringDup(const char* s)
{
    size_t sLen = strlen(s);
    char* res = malloc(sLen + 1 * sizeof(char));
    
    int i = 0;
    for (i = 0; i < sLen; i++)
    {
        res[i] = s[i];
    }
    res[i] = '\0';
    
    return res;
}
