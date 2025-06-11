#include "../include/buffer.h"

#include <stdlib.h>
#include <string.h>


/// @brief Append buffer structure
/// @details  - b : buffer
///           - len : length of the buffer
/// @note  This structure is used to store the buffer and its length
/// @param ab 
/// @param s 
/// @param len 
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
