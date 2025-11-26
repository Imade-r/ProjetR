#ifndef PLAYER_H
#define PLAYER_H

#include "../core/config.h"
#include "../core/types.h"

void InitPlayer(Player* p, int id, DaltonType t, bool ai);
void ResetPlayer(Player* p, Vector2 pos);
void UpdatePlayerPhysics(Player* p, float dt, EnvPlatform platforms[], int platformCount);
void DrawPlayer(Player* p);

#endif
