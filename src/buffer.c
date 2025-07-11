#include "../include/buffer.h"

#include <stdlib.h>
#include <string.h>


/// @brief Append buffer structure
/// @note  This structure is used to store the buffer and its length
/// @param ab used to store the buffer and its length
/// @param s used to store the string to append
/// @param len used to store the length of the string to append
void abAppend(struct abuf *ab, const char *s, int len)
{
    char *new = realloc(ab->b, ab->len + len);

    if (new == NULL)
        return;
    memcpy(&new[ab->len], s, len);
    
    ab->b = new;
    ab->len += len;
}

void abFree(struct abuf *ab)
{
    free(ab->b);
}
