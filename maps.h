#ifndef _MAPS_H
#define _MAPS_H

#define MAX_MAP_WIDTH   100
#define MAX_MAP_HEIGHT  100

typedef struct {
        int id;
        int width;
        int height;
        char **data;
} Map;

/* Load maps from specified file into given array */
int maps_load(Map[], char *, int);
/* Free map data */
void maps_free(Map[], int);

/* Shuffle maps using Fisher-Yates */
void maps_shuffle(Map[], int);

#endif
