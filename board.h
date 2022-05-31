#ifndef _BOARD_H
#define _BOARD_H

#include "screen.h"

typedef enum {
        FLOOR = 0,      // Empty floor
        FLOOR_TARGET,   // Target square
        BOX,            // Box
        BOX_TARGET,     // Box on target square
        PLAYER,         // Player
        PLAYER_TARGET,  // Player on target square
        WALL,           // Wall
} Tile;

typedef enum {
        UP,
        DOWN,
        RIGHT,
        LEFT,
} Direction;

typedef struct {
        int x;
        int y;
} Point;

typedef struct {
        int width;
        int height;
        Point player;
        Tile *tiles;
} Board;

/* Initialize a board with given width and height */
Board *board_init(int, int);
/* Free the given board */
void board_free(Board *);

/* Set the (x, y) tile on the given board */
void board_set(Board *, Tile, int, int);
/* Get the (x, y) tile on the given board */
Tile board_get(Board *, int, int);

/* Check whether the given board has been solved */
int board_solved(Board *);

/* Try to move the player legally in the given direction, returns 1 if successful */
int board_player_move(Board *, Direction);

/* Load an ASCII map into the given board */
void board_load(Board *, char **);

/* Draw the given board to a screen buffer, with specified offset */
void draw_to_screen(Board *, Screen *, int, int);

#endif
