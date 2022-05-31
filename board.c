#include <stdlib.h>

#include "board.h"
#include "screen.h"

/* Direction deltas indexed by the 'Direction' enum */
Point direction_delta[] = {
        { .x =  0, .y =  1 },
        { .x =  0, .y = -1 },
        { .x =  1, .y =  0 },
        { .x = -1, .y =  0 },
};

/* Tile appearances as Pixel objects, indexed by the 'Tile' enum */
Pixel tilemap[] = {
        { .item = "  ", .fg = WHITE,    .bg = BLACK  },
        { .item = "[]", .fg = RED,      .bg = BLACK  },
        { .item = "  ", .fg = WHITE,    .bg = YELLOW },
        { .item = "[]", .fg = RED,      .bg = GREEN  },
        { .item = "  ", .fg = WHITE,    .bg = WHITE  },
        { .item = "[]", .fg = RED,      .bg = WHITE  },
        { .item = "%%", .fg = BLUE,     .bg = BLACK  },
};

Board *board_init(int width, int height) {
        Board *board = malloc(sizeof(Board));
        board->width = width;
        board->height = height;
        board->tiles = calloc(width * height, sizeof(Tile));
        return board;
}

void board_free(Board *board) {
        free(board->tiles);
        free(board);
}

void board_set(Board *board, Tile t, int x, int y) {
        if (x < 0 || y < 0 || x >= board->width || y >= board->height)
                return;
        board->tiles[board->height * x + y] = t;
}

Tile board_get(Board *board, int x, int y) {
        if (x < 0 || y < 0 || x >= board->width || y >= board->height)
                return WALL;
        return board->tiles[board->height * x + y];
}

int board_solved(Board *board) {
        for (int i = 0; i < board->width; i++) {
                for (int j = 0; j < board->height; j++) {
                        Tile t = board_get(board, i, j);
                        /* Make sure that there are no target squares without boxes on them */
                        if (t == FLOOR_TARGET || t == PLAYER_TARGET)
                                return 0;
                }
        }
        return 1;
}

int board_player_move(Board *board, Direction d) {
        /* Calculate the position to which the player potentially moves */
        Point player_new;
        player_new.x = board->player.x + direction_delta[d].x;
        player_new.y = board->player.y + direction_delta[d].y;

        /* Get the tiles on the player's current and potential positions */
        Tile p = board_get(board, board->player.x, board->player.y);
        Tile t = board_get(board, player_new.x, player_new.y);

        /* If the player's new position is unoccupied, simply move */
        if (t == FLOOR || t == FLOOR_TARGET) {
                /* Use some bit arithmetic to preserve the positions of the target squares.
                   The last bit represents the presence of a target square. */
                board_set(board, FLOOR  | (p & 1), board->player.x, board->player.y);
                board_set(board, PLAYER | (t & 1), player_new.x, player_new.y);
                board->player.x = player_new.x;
                board->player.y = player_new.y;
                return 1;
        }
        /* If the player's new position is occupied by a box, try to push it */
        if (t == BOX || t == BOX_TARGET) {
                /* Calculate the position to which the box potentially moves */
                Point box_new;
                box_new.x = player_new.x + direction_delta[d].x;
                box_new.y = player_new.y + direction_delta[d].y;

                /* Get the tile on the box's potential position */
                Tile b = board_get(board, box_new.x, box_new.y);

                /* If the box's new position is unoccupied, move */
                if (b == FLOOR || b == FLOOR_TARGET) {
                        board_set(board, FLOOR  | (p & 1), board->player.x, board->player.y);
                        board_set(board, PLAYER | (t & 1), player_new.x, player_new.y);
                        board_set(board, BOX    | (b & 1), box_new.x, box_new.y);
                        board->player.x = player_new.x;
                        board->player.y = player_new.y;
                        return 1;
                }
        }
        return 0;
}

void board_load(Board *board, char **map) {
        for (int i = 0; i < board->width; i++) {
                for (int j = 0; j < board->height; j++) {
                        Tile t;
                        /* Read character in a way that preserves orientation,
                           and map it to a tile */
                        switch(map[board->height - j - 1][i]) {
                                case ' ':
                                case '_':
                                        t = FLOOR;
                                        break;
                                case '#':
                                        t = WALL;
                                        break;
                                case '$':
                                        t = BOX;
                                        break;
                                case '%':
                                        t = BOX_TARGET;
                                        break;
                                case '.':
                                case 'x':
                                case 'X':
                                        t = FLOOR_TARGET;
                                        break;
                                case '@':
                                        t = PLAYER;
                                        board->player.x = i;
                                        board->player.y = j;
                                        break;
                                case '&':
                                        t = PLAYER_TARGET;
                                        board->player.x = i;
                                        board->player.y = j;
                                        break;
                        }
                        board_set(board, t, i, j);
                }
        }
}



void draw_to_screen(Board *board, Screen *screen, int offset_row, int offset_col) {
        for (int i = 0; i < board->width; i++) {
                for (int j = 0; j < board->height; j++) {
                        Tile t = board_get(board, i, j);
                        /* Use the tilemap to push the corresponding Pixel to the buffer */
                        screen_set(screen, tilemap[t].item, tilemap[t].fg, tilemap[t].bg, offset_col + i, offset_row + j);
                }
        }
}
