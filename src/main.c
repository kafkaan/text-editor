#include "../include/terminal.h"
#include "../include/input.h"
#include "../include/output.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


/*
    editorConfig structure holds the global state of the editor.
    It contains the cursor position, screen size, and original terminal settings.
*/
struct editorConfig E;


int main(int argc,char *argv[]) {
    enableRawMode();
    initEditor();
    if(argc >= 2)
    {
        editorOpen(argv[1]);
    }

    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
        
    }
    return 0;
}