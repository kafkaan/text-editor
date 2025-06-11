#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>

/// @brief Cette macro CTRL_KEY permet de générer les codes des touches Ctrl+Lettre.
// Elle applique un "ET bit à bit" entre le caractère k et 0x1F (00011111 en binaire),
// ce qui supprime les 3 bits de poids fort (bits 5, 6, 7) et garde uniquement les 5 bits faibles.
// Cela simule ce que fait la touche Ctrl dans un terminal :
// Par exemple, 'A' = 0x41 → 0x41 & 0x1F = 0x01 (Ctrl+A)
//              'C' = 0x43 → 0x43 & 0x1F = 0x03 (Ctrl+C)
// Cela permet d'intercepter des raccourcis clavier Ctrl+Lettre facilement.
#define CTRL_KEY(k) ((k) & 0x1f)

#define KILO_VERSION "0.0.1"

/// @brief By setting the first constant in the enum to 1000, the rest of the constants get incrementing values of 1001, 1002, 1003, and so on.
enum editorKey
{
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN,

};

/// @brief erow stands for “editor row”, and stores a line of text as a pointer to the dynamically-allocated character data and a length
typedef struct erow 
{
    int size;
    char *chars;
} erow;


/// The editorConfig structure holds the global state of the editor.
/// It contains the cursor position, screen size, and original terminal settings.
/// It is used to manage the editor's state and configuration.
/// @brief  Structure de configuration de l'éditeur
/// @details  - cx : position x du curseur
///           - cy : position y du curseur
///           - screenrows : nombre de lignes de l'écran
///           - screencols : nombre de colonnes de l'écran
///           - orig_termios : structure termios d'origine pour restaurer les paramètres du terminal
/// @note  Cette structure est utilisée pour stocker l'état de l'éditeur et les paramètres du terminal
struct editorConfig
{
    // cursor coordinates
    int cx, cy;
    int screenrows;
    int screencols;
    int numrows;
    erow *row;
    struct termios orig_termios;
};

void die(const char *s);
void initEditor();


void enableRawMode(void);

//disable raw mode
void disableRawMode(void);
//get window size
int getWindowSize(int *rows, int *cols);

int getCursorPosition(int *rows, int *cols);

/*** file i/o  ***/
void editorOpen();

#endif