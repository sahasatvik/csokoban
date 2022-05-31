#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "maps.h"
#include "board.h"
#include "screen.h"

#define MAX_MAPS        1000
#define MAX_FILENAME    100
#define MAPS_DEFAULT    "levels.txt"

/* Configuration for startup */
struct Config {
        int map_number;
        int shuffle;
        char maps_filename[MAX_FILENAME];
} config;

Map maps[MAX_MAPS];

/* Parse command line arguments into config */
void parse_args(struct Config *, int, char * const[]);

/* Show usage/help */
void usage();
void usage_keys();

int main(int argc, char * const argv[]) {
        
        /* Set default configuration options */
        config.map_number = 0;
        config.shuffle = 0;
        strcpy(config.maps_filename, MAPS_DEFAULT);
        /* Parse command line arguments */
        parse_args(&config, argc, argv);
        
        /* Load maps from file */
        int total_maps = maps_load(maps, config.maps_filename, MAX_MAPS);
        if (total_maps < 0) {
                printf("Could not open file %s!\n", config.maps_filename);
                exit(1);
        }
        
        /* Set starting map number */
        int map_number = config.map_number;
        if (map_number < 0 || map_number >= total_maps) {
                printf("Selected map number out of bounds! Must be between 1 and %d.\n", total_maps);
                exit(1);
        }

        /* Shuffle maps */
        if (config.shuffle) {
                maps_shuffle(maps, total_maps);
        }

        /* Get terminal dimensions */
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        int rows = w.ws_row;
        int cols = w.ws_col;

        /* Set termios flags for better input */
        struct termios term_orig_attr, term_new_attr;
        
        tcgetattr(fileno(stdin), &term_orig_attr);
        memcpy(&term_new_attr, &term_orig_attr, sizeof(term_new_attr));
        term_new_attr.c_lflag &= ~(ICANON | ECHO);
        term_new_attr.c_cc[VMIN] = 1;
        term_new_attr.c_cc[VTIME] = 0; 
        tcsetattr(fileno(stdin), TCSANOW, &term_new_attr);
        
        printf(ANSI_CLEAR);
        printf(ANSI_HIDE_CUR);

        /* Initialize screen buffers */
        Screen *screen = screen_init(cols / 2, rows);
        Screen *screen_b = screen_init(cols / 2, rows);
        
        /* Setup the playing board */
        Board *board;
        int offset_row;
        int offset_col;

        /* Load starting map into a board */
        board = board_init(maps[map_number].width, maps[map_number].height);
        board_load(board, maps[map_number].data);
        /* Calculate offsets to centre the board on screen */
        offset_row = (rows - maps[map_number].height) / 2;
        offset_col = (cols - maps[map_number].width * 2) / 4;

        /* Initialize game flags */
        int steps = 0;
        int solved = 0;
        int finished_all = 0;

        /* Get input character by character */
        int c = '\0';
        while (c != EOF) {
                /* Check whether the board is solved */
                solved = board_solved(board);

                /* Reset the screen buffer and draw the board */
                screen_reset(screen);
                draw_to_screen(board, screen, offset_row, offset_col);

                /* Display the map number and steps taken */
                char buf[100];
                sprintf(buf, "Map : %d", map_number + 1);
                if (config.shuffle) {
                        sprintf(buf + strlen(buf), " (#%d)", maps[map_number].id + 1);
                }
                screen_set_str(screen, buf, solved? GREEN : WHITE, BLACK, offset_col, offset_row + maps[map_number].height);
                sprintf(buf, "Step : %d", steps);
                screen_set_str(screen, buf, WHITE, BLACK, offset_col, offset_row - 1);

                /* Flush the screen buffer to the terminal.
                   This only pushes the characters that have changed from the previous buffer. */
                screen_display(screen, screen_b);

                /* Swap the current and old screen buffers */
                Screen *temp = screen;
                screen = screen_b;
                screen_b = temp;
                
                /* If solved, wait and load the next map */
                if (solved) {
                        sleep(1);
                        map_number++;
                        if (map_number >= total_maps) {
                                finished_all = 1;
                                break;
                        }
                        board_free(board);
                        board = board_init(maps[map_number].width, maps[map_number].height);
                        board_load(board, maps[map_number].data);
                        offset_row = (rows - maps[map_number].height) / 2;
                        offset_col = (cols - maps[map_number].width * 2) / 4;
                        steps = 0;
                        continue;
                }

                /* Handle input */
                switch (c = getc(stdin)) {
                        case 'q':
                                /* Quit */
                                c = EOF;
                                break;
                        case 'r':
                                /* Reset current map */
                                board_load(board, maps[map_number].data);
                                steps = 0;
                                break;
                        case '>':
                                /* Load next map */
                                if (map_number < total_maps - 1) {
                                        map_number++;
                                        board_free(board);
                                        board = board_init(maps[map_number].width, maps[map_number].height);
                                        board_load(board, maps[map_number].data);
                                        offset_row = (rows - maps[map_number].height) / 2;
                                        offset_col = (cols - maps[map_number].width * 2) / 4;
                                        steps = 0;
                                }
                                break;
                        case '<':
                                /* Load previous map */
                                if (map_number > 0) {
                                        map_number--;
                                        board_free(board);
                                        board = board_init(maps[map_number].width, maps[map_number].height);
                                        board_load(board, maps[map_number].data);
                                        offset_row = (rows - maps[map_number].height) / 2;
                                        offset_col = (cols - maps[map_number].width * 2) / 4;
                                        steps = 0;
                                }
                                break;
                        case 'h':
                        case '4':
                                steps += board_player_move(board, LEFT);
                                break;
                        case 'j':
                        case '2':
                                steps += board_player_move(board, DOWN);
                                break;
                        case 'k':
                        case '8':
                                steps += board_player_move(board, UP);
                                break;
                        case 'l':
                        case '6':
                                steps += board_player_move(board, RIGHT);
                                break;
                }

        }
        if (feof(stdin))
                clearerr(stdin);
        
        /* Free resources */
        board_free(board);
        screen_free(screen);

        int last_id = maps[map_number].id;
        maps_free(maps, total_maps);

        /* Clear screen */
        printf(ANSI_GOTO, 0, 0);
        printf(ANSI_CLEAR);
        printf(ANSI_SHOW_CUR);

        /* Reset termios flags */
        tcsetattr(fileno(stdin), TCSANOW, &term_orig_attr);

        /* Leave message */
        if (finished_all) {
                printf("That's all, folks!\n");
        } else if (config.shuffle) {
                printf("You were on map %d (#%d).\n", map_number + 1, last_id + 1);
        } else {
                printf("You were on map %d.\n", map_number + 1);
        }

        return 0;
}

void parse_args(struct Config *config, int argc, char * const argv[]) {
        int c;
        while ((c = getopt(argc, argv, "hsl:n:")) != -1) {
                switch(c) {
                        case 'h':
                                printf("The game Sokoban, implemented in C for the terminal.\n");
                                printf("\n");
                                usage();
                                printf("\n");
                                usage_keys();
                                exit(0);
                                break;
                        case 's':
                                config->shuffle = 1;
                                break;
                        case 'l':
                                strcpy(config->maps_filename, optarg);
                                break;
                        case 'n':
                                config->map_number = atoi(optarg) - 1;
                                break;
                        case ':':
                                printf("%s: option requires an argument -- %c\n", argv[0], optopt);
                        case '?':
                                usage();
                                exit(1);
                                break;
                }
        }
}

void usage() {
        printf("usage: csokoban [-hs] [-l mapfile] [-n mapnumber]\n");
        printf("    -h  Show this help message\n");
        printf("    -s  Shuffle maps\n");
        printf("    -l  Load puzzles from 'mapfile'\n");
        printf("    -n  Start from map 'mapnumber'\n");
}

void usage_keys() {
        printf("keys:\n");
        printf("    h j k l     Move the player character left/down/up/right\n");
        printf("    4 2 8 6     Same, using the numpad keys\n");
        printf("          q     Quit\n");
        printf("          r     Restart the current map\n");
        printf("        > <     Skip to the next/previous map\n");

}
