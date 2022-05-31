#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "maps.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

int maps_load(Map maps[], char *maps_filename, int max_maps) {
        FILE *f;
        /* Try and open the given file */
        if (!(f = fopen(maps_filename, "r")))
                return -1;

        /* Setup temporary buffer for holding a map */
        char *buf[MAX_MAP_HEIGHT];
        for (int i = 0; i < MAX_MAP_HEIGHT; i++) {
                buf[i] = malloc(MAX_MAP_WIDTH * sizeof(char *));
        }

        size_t len;
        int n;
        /* Read up to 'max_maps' many maps */
        for (n = 0; n < max_maps && !feof(f); ) {
                maps[n].id = n;
                maps[n].width = 0;
                maps[n].height = 0;
                /* Read lines into buffer */
                while (maps[n].height < MAX_MAP_HEIGHT) {
                        getline(&(buf[maps[n].height]), &len, f);
                        /* Stop when empty line found */
                        int l = strlen(buf[maps[n].height]);
                        if (l <= 1)
                                break;
                        /* Skip any lines not starting with '#' or ' '.
                           In other words, comment lines must start with something else. */
                        if (buf[maps[n].height][0] != '#' &&
                            buf[maps[n].height][0] != ' ')
                                continue;
                        /* Recalculate the width of the map */
                        maps[n].width = MAX(l, maps[n].width);
                        maps[n].height++;
                }
                /* Skip empty maps */
                if (maps[n].height == 0)
                        continue;

                /* Copy buffer into map */
                maps[n].data = malloc(maps[n].height * sizeof(char *));
                for (int i = 0; i < maps[n].height; i++) {
                        maps[n].data[i] = malloc((maps[n].width + 1) * sizeof(char));
                        strcpy(maps[n].data[i], buf[i]);
                }
                n++;
        }

        fclose(f);

        /* Free buffer memory */
        for (int i = 0; i < MAX_MAP_HEIGHT; i++)
                free(buf[i]);

        return n;
}

void maps_free(Map maps[], int total_maps) {
        for (int n = 0; n < total_maps; n++) {
                for (int i = 0; i < maps[n].height; i++) {
                        free(maps[n].data[i]);
                }
                free(maps[n].data);
        }
}

void maps_shuffle(Map maps[], int total_maps) {
        if (total_maps <= 1)
                return;
        
        /* Seed RNG using system time */
        struct timeval tv;
        gettimeofday(&tv, NULL);
        int usec = tv.tv_usec;
        srand48(usec);

        for (unsigned int i = total_maps - 1; i > 0; i--) {
                unsigned int j = (unsigned int) (drand48() * (i + 1));
                Map temp = maps[j];
                maps[j] = maps[i];
                maps[i] = temp;
        }
}
