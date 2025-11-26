#ifndef MAP_H
#define MAP_H

#include "config.h"
#include "types.h"

extern EnvPlatform platforms[MAX_PLATFORMS];
extern int platformCount;
extern Destructible destructibles[MAX_DESTRUCTIBLES];
extern int destructibleCount;

void InitMap();
void DrawDecor(Camera2D cam);
void DrawMap();

#endif
