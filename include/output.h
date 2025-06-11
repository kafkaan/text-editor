#ifndef OUTPUT_H
#define OUTPUT_H

#include "buffer.h"

void editorRefreshScreen(void);
// draw ~ like in vim in each row
void editorDrawRows(struct abuf *ab);

#endif