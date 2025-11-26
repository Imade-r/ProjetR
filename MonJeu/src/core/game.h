#ifndef GAME_H
#define GAME_H

#include "types.h"

extern Player players[MAX_PLAYERS];
extern int playerCount;
extern int winnerId;
extern float screenShake;
extern Camera2D camera;

void InitGame();
void UpdateGame(float dt);
void DrawGame();

#endif
