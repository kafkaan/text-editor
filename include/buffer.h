#ifndef BUFFER_H
#define BUFFER_H

/*** Represents an empty buffer intiialization ***/
#define ABUF_INIT {NULL, 0}


/*** appennd buffer structure ***/
struct abuf
{
    char *b;
    int len;
};

// for append buffer / avoid bunch of small write() every time we refresh the screen / one write to make sure the whole screen updates at once
void abAppend(struct abuf *ab, const char *s, int len);

void abFree(struct abuf *ab);

#endif