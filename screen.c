#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "screen.h"

/* ANSI color codes indexed by the 'Color' enum */ 
char *colormap[8][8] = {
        {
                ANSI_FG_BLACK,
                ANSI_FG_RED,
                ANSI_FG_GREEN,
                ANSI_FG_YELLOW,
                ANSI_FG_BLUE,
                ANSI_FG_MAGENTA,
                ANSI_FG_CYAN,
                ANSI_FG_WHITE,
        },
        {
                ANSI_BG_BLACK,
                ANSI_BG_RED,
                ANSI_BG_GREEN,
                ANSI_BG_YELLOW,
                ANSI_BG_BLUE,
                ANSI_BG_MAGENTA,
                ANSI_BG_CYAN,
                ANSI_BG_WHITE,
        },
};

Screen *screen_init(int width, int height) {
        Screen *screen = malloc(sizeof(Screen));
        screen->width = width;
        screen->height = height;
        screen->pixels = calloc(width * height, sizeof(Pixel));
        return screen;
}

void screen_free(Screen *screen) {
        free(screen->pixels);
        free(screen);
}

void screen_reset(Screen *screen) {
        for (int y = 0; y < screen->height; y++) {
                for (int x = 0; x < screen->width; x++) {
                        screen->pixels[screen->height * x + y].item[0] = ' ';
                        screen->pixels[screen->height * x + y].item[1] = ' ';
                        screen->pixels[screen->height * x + y].fg = WHITE;
                        screen->pixels[screen->height * x + y].bg = BLACK;
                }
        }
}

void screen_set(Screen *screen, char c[2], Color fg, Color bg, int x, int y) {
        if (x < 0 || y < 0 || x >= screen->width || y >= screen->height)
                return;
        screen->pixels[screen->height * x + y].item[0] = c[0];
        screen->pixels[screen->height * x + y].item[1] = c[1];
        screen->pixels[screen->height * x + y].fg = fg;
        screen->pixels[screen->height * x + y].bg = bg;
}

Pixel *screen_get(Screen *screen, int x, int y) {
        if (x < 0 || y < 0 || x >= screen->width || y >= screen->height)
                return NULL;
        return &(screen->pixels[screen->height * x + y]);
}

void screen_set_str(Screen *screen, char *str, Color fg, Color bg, int x, int y) {
        int len = strlen(str);
        /* Write two characters per pixel */
        for (int i = 0; i < len; i += 2)
                screen_set(screen, str + i, fg, bg, x + i / 2, y);
        /* If the last character is alone on a pixel, blank the next character */
        if (len % 2) {
                char buf[2];
                buf[0] = str[len - 1];
                buf[1] = ' ';
                screen_set(screen, buf, fg, bg, x + len / 2, y);
        }
}

void screen_display(Screen *screen, Screen *old) {
        printf(ANSI_RESET);
        for (int y = 0; y < screen->height; y++) {
                for (int x = 0; x < screen->width; x++) {
                        Pixel *p1 = screen_get(screen, x, y);
                        Pixel *p2 = screen_get(old, x, y);
                        /* Skip if the pixel hasn't changed from the old buffer */
                        if (p1->item[0] == p2->item[0] 
                                && p1->item[1] == p2->item[1]
                                && p1->fg == p2->fg 
                                && p1->bg == p2->bg)
                                continue;
                        printf(ANSI_GOTO, screen->height - y, 2 * x + 1);
                        printf(colormap[0][p1->fg]);
                        printf(colormap[1][p1->bg]);
                        printf("%c%c" ANSI_RESET, p1->item[0], p1->item[1]);
                }
        }
        fflush(stdout);
}
