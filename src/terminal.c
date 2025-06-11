#include "../include/terminal.h"

#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
extern struct editorConfig E;

/// @brief prints an error message and exits the program
/// @param s
void die(const char *s)
{
    // c library functions that fail will set the global errno variable
    // perror looks looks at the global errno and prints a desciprtive error message
    perror(s);
    // exit program with status of 1 whichi indicates failure
    exit(1);
}

/// @brief Initializes the editor state
/// @details Sets the cursor position to (0, 0) and gets the window size
/// @return void
/// @note This function is called at the beginning of the program to set up the editor state
void initEditor()
{
    E.cx = 0;
    E.cy = 0;
    E.numrows = 0;
    E.row = NULL;

    if (getWindowSize(&E.screenrows, &E.screencols) == -1)
        die("getWindowSize");
}

/// @brief Disables raw mode
/// @details tcsetattr() is used to set the terminal attributes
/// @param void
/// @return void
/// @note This function is called at the end of the program to restore the terminal state
/// @note The function is registered with atexit() to ensure it is called when the program exits
void disableRawMode()
{
    // TCIFLUSH : flushes the input buffer
    if (tcsetattr(STDIN_FILENO, TCIFLUSH, &E.orig_termios) == -1)
        die("tcsetattr");
}

/**
 * Active le "raw mode" sur le terminal :
 * Ce mode désactive les traitements automatiques de l'entrée et sortie,
 * permettant de lire les touches clavier une par une sans buffer ni écho.
 *
 * Détail des flags utilisés :
 *
 * - c_oflag (output flags)
 *   - OPOST : désactivé pour éviter le post-traitement de la sortie (ex : ajout automatique de \r).
 *
 * - c_iflag (input flags)
 *   - IXON   : désactive Ctrl-S / Ctrl-Q (contrôle du flux).
 *   - ICRNL  : empêche la conversion CR (carriage return) → NL (newline).
 *   - BRKINT : empêche le signal d'interruption en cas de caractère "break".
 *   - INPCK  : désactive la vérification de parité sur l'entrée.
 *   - ISTRIP : désactive le masquage des 8 bits (permet les caractères 8 bits).
 *
 * - c_lflag (local flags)
 *   - ECHO   : empêche l'affichage automatique des caractères tapés.
 *   - ICANON : désactive le mode canonique (lecture ligne par ligne → lecture caractère par caractère).
 *   - ISIG   : désactive les signaux Ctrl-C (SIGINT), Ctrl-Z (SIGTSTP), etc.
 *   - IEXTEN : désactive Ctrl-V (traitement des séquences d’entrée étendues).
 *
 * - c_cflag (control flags)
 *   - CS8 : active un mode 8 bits par caractère (standard pour UTF-8).
 *
 * - c_cc (control characters)
 *   - VMIN = 0  : permet à read() de retourner immédiatement s’il n’y a rien à lire.
 *   - VTIME = 1 : attend jusqu’à 0.1 seconde (100ms) pour recevoir des données.
 *
 * Remarques :
 * - La configuration d’origine est sauvegardée dans orig_termios pour pouvoir la restaurer.
 * - disableRawMode() est automatiquement appelé à la fin du programme via atexit().
 */
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
        die("tcgetattr");
    // atexit() comes from <stdlib.h>. We use it to register our disableRawMode() function to be called automatically when the program exits
    atexit(disableRawMode);

    struct termios raw = E.orig_termios;

    raw.c_oflag &= ~(OPOST);

    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);

    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    raw.c_cflag |= (CS8);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    // TCSAFLUSH argument specifies when to apply the change: in this case, it waits for all pending output to be written to the terminal
    if (tcsetattr(STDIN_FILENO, TCIFLUSH, &raw) == -1)
        die("tcsetattr");
}

/// @brief Get the window size
/// @details Uses ioctl() to get the window size and stores it in the provided pointers rows and cols
/// @param rows
/// @param cols
/// @return
int getWindowSize(int *rows, int *cols)
{
    struct winsize ws;
    // terminal input output control get window size
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        // C AND B commans to stop the cursor from going past the edge of screen
        //  Déplacer le curseur de 999 colonnes à droite et 999 lignes en bas sans depasser les bords
        // ioctl() isn’t guaranteed to be able to request the window size on all systems, so we are going to provide a fallback method of getting the window size.
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
            return -1;
        return getCursorPosition(rows, cols);
    }
    else
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

/// @brief Get the cursor position
/// @details Uses a special escape sequence to ask the terminal for the cursor position
/// @details The terminal responds with a string like "\x1b[24;80R" where 24 is the row and 80 is the column
/// @details The function reads the response and extracts the row and column numbers
/// @param rows
/// @param cols
/// @return
int getCursorPosition(int *rows, int *cols)
{
    // Buffer pour stocker la réponse du terminal
    char buf[32];
    // Index pour lire caractère par caractère
    unsigned int i = 0;

    // Envoie la séquence d'échappement "\x1b[6n" au terminal.
    // Cela demande au terminal : "Où est le curseur actuellement ?"
    // Le terminal répondra avec quelque chose comme "\x1b[24;80R"
    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
        return -1;

    // Lit la réponse du terminal un caractère à la fois dans le buffer
    while (i < sizeof(buf) - 1)
    {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
            break;
        if (buf[i] == 'R')
            break;
        i++;
    }
    buf[i] = '\0';

    // 🔎 À des fins de débogage : affiche la réponse du terminal sans le caractère d'échappement initial
    // &buf[1] saute le '\x1b' (ESC), qui pourrait fausser l'affichage du terminal
    // printf("\r\n&buf[1] : '%s'\r\n", &buf[1]);

    if (buf[0] != '\x1b' || buf[1] != '[')
        return -1;

    // Utilise sscanf pour extraire les deux entiers (ligne et colonne) de la forme "row;col"
    // Exemple : à partir de "\x1b[24;80R", on récupère rows=24 et cols=80
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
        return -1;

    return 0;
}

/*** ROW OPERATIONS */
void editorAppendRow(char *s, size_t len)
{

    E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));

    int at = E.numrows;

    E.row[at].size = len;
    E.row[at].chars = malloc(len + 1);
    memcpy(E.row[at].chars, s, len);
    E.row[at].chars[len] = '\0';
    E.numrows++;
}

void editorOpen(char *filename)
{
    /*
    char *line = "Hello world!";
    ssize_t linelen = 13;

    E.erow.size = linelen;
    E.erow.chars = malloc(linelen + 1);
    memcpy(E.erow.chars, line, linelen);
    E.erow.chars[linelen] = '\0';
    E.numrows = 1;
    */

    FILE *fp = fopen(filename, "r");
    if (!fp)
        die("fopen");

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    while ((linelen = getline(&line, &linecap, fp)) != -1)
    {
        while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
        {
            linelen--;
        }

        editorAppendRow(line, linelen);
    }

    free(line);
    fclose(fp);
}