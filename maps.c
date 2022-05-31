#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "maps.h"

int maps_load(Map maps[], char *maps_filename, int max_maps) {
        FILE *f;
        /* Try and open the given file */
        if (!(f = fopen(maps_filename, "r")))
                return -1;

        size_t len;
        int n;
        /* Read up to 'max_maps' many maps */
        for (n = 0; n < max_maps && !feof(f); n++) {
                /* Read the header line for the current map, and extract the width and height */
                fscanf(f, "; %d %d\n", &(maps[n].width), &(maps[n].height));

                /* Allocate and read 'height' lines */
                maps[n].data = malloc(maps[n].height * sizeof(char *));
                for (int i = 0; i < maps[n].height; i++) {
                        getline(&(maps[n].data[i]), &len, f);
                }

                /* Skip mandatory empty line */
                fscanf(f, "\n");
                maps[n].id = n;
        }

        fclose(f);
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
