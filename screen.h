#ifndef _TERM_SCREEN_H
#define _TERM_SCREEN_H

#define ANSI_RESET      "\033[0m"
#define ANSI_CLEAR      "\033[2J"
#define ANSI_HOME       "\033[H"
#define ANSI_GOTO       "\033[%d;%dH"
#define ANSI_BOLD       "\033[1m"
#define ANSI_HIDE_CUR   "\033[?25l"
#define ANSI_SHOW_CUR   "\033[?25h"

#define ANSI_FG_BLACK   "\033[30m"
#define ANSI_FG_RED     "\033[31m"
#define ANSI_FG_GREEN   "\033[32m"
#define ANSI_FG_YELLOW  "\033[33m"
#define ANSI_FG_BLUE    "\033[34m"
#define ANSI_FG_MAGENTA "\033[35m"
#define ANSI_FG_CYAN    "\033[36m"
#define ANSI_FG_WHITE   "\033[37m"

#define ANSI_BG_BLACK   "\033[40m"
#define ANSI_BG_RED     "\033[41m"
#define ANSI_BG_GREEN   "\033[42m"
#define ANSI_BG_YELLOW  "\033[43m"
#define ANSI_BG_BLUE    "\033[44m"
#define ANSI_BG_MAGENTA "\033[45m"
#define ANSI_BG_CYAN    "\033[46m"
#define ANSI_BG_WHITE   "\033[47m"

typedef enum {
        BLACK = 0,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE,
} Color;

typedef struct {
        char item[2];   // Each pixel consists of 2 characters on screen
        Color fg;
        Color bg;
} Pixel;

typedef struct {
        int width;
        int height;
        Pixel *pixels;
} Screen;

/* Initialize a screen buffer with given width and height */
Screen *screen_init(int, int);
/* Free the given screen buffer */
void screen_free(Screen *);
/* Reset the given screen buffer to black spaces */
void screen_reset(Screen *);

/* Set the (x, y) pixel on the given screen buffer */
void screen_set(Screen *, char[2], Color, Color, int, int);
/* Get the (x, y) pixel on the given screen buffer */
Pixel *screen_get(Screen *, int, int);

/* Write a string of text to the given screen buffer */
void screen_set_str(Screen *, char *, Color, Color, int, int);

/* Flush the screen buffer contents to the terminal,
   using differences from the previous screen buffer */
void screen_display(Screen *, Screen *);

#endif
