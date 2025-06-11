#include "../include/terminal.h"
#include "../include/output.h"
#include "../include/buffer.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Declared in terminal.h, defined here
extern struct editorConfig E;

// draw ~ like in vim in each row
void editorDrawRows(struct abuf *ab)
{
    int y;

    for (y = 0; y < E.screenrows; y++)
    {
        if (y >= E.numrows)
        {
            if (E.numrows == 0 && y == E.screenrows / 3)
            {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome), "Kirby editor -- version %s", KILO_VERSION);
                // snprintf() écrit dans ce buffer une version formatée de : "Kirby editor -- version 0.0.1" (ou autre version).
                // snprintf() est utilisé ici pour éviter de dépasser la taille du buffer (welcome[])
                // snprintf more sure than sprintf

                // si on depasse le nombre de col
                // on raccouric le message
                if (welcomelen > E.screencols)
                    welcomelen = E.screencols;

                /*** center the text ***/
                int padding = (E.screencols - welcomelen) / 2;
                //~ for the first char
                if (padding)
                {
                    abAppend(ab, "~", 1);
                    padding--;
                }

                while (padding--)
                    abAppend(ab, " ", 1);
                abAppend(ab, welcome, welcomelen);
            }
            else
            {
                // write(STDOUT_FILENO,"~",1);
                abAppend(ab, "~", 1);
            }
        }
        else
        {
            int len = E.row[y].size;
            if(len > E.screencols) len = E.screencols;
            abAppend(ab,E.row[y].chars,len);
        }

        // The k commande Erase In Line erases part of the current line
        // 0 default is to erase line left to the cursor
        abAppend(ab, "\x1b[K", 3);
        // if this is the last line we don't make \n
        if (y < E.screenrows - 1)
        {
            // write(STDOUT_FILENO,"\r\n",2);
            abAppend(ab, "\r\n", 2);
        }
    }
}

void editorRefreshScreen()
{
    // apprend buffer initialization
    struct abuf ab = ABUF_INIT;
    // h and l are for set mode & reset mode
    // are used to turn on and turn off various terminal features or “modes”.
    // 25 hiding/showing cursor feature
    abAppend(&ab, "\x1b[?25l", 6);

    // The first byte is \x1b, which is the escape character, or 27 in decimal
    //[2J means to clean all screan
    //  4 means that we pass 4 bytes
    //  write(STDOUT_FILENO, "\x1b[2J", 4);

    // abAppend(&ab,"\x1b[2J",4);

    // This escape sequence positions the cursor using the 'H' command (Cursor Position).
    // Format: ESC[row;colH (e.g. "\x1b[12;40H" moves the cursor to row 12, column 40).
    // Rows and columns start at 1 (not 0). If no arguments are given (just "\x1b[H"),
    // it defaults to row 1, column 1 (top-left corner).
    // write(STDOUT_FILENO, "\x1b[H", 3);

    abAppend(&ab, "\x1b[H", 3);

    editorDrawRows(&ab);

    // placer le curseur au bon endroit 
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E.cy + 1, E.cx + 1);
    abAppend(&ab, buf, strlen(buf));

    // The \x1b[?25h escape sequence is used to show the cursor in the terminal.
    abAppend(&ab, "\x1b[?25h", 6);

    write(STDOUT_FILENO, ab.b, ab.len);
    abFree(&ab);
}