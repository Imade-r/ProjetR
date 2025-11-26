#include "menu.h"
#include "../core/assets.h"

void DrawMenu(bool assetsLoaded) {
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COL_SKY_TOP, COL_SKY_BOT);
    DrawText("BLAST BRAWL", SCREEN_WIDTH/2 - 200, 200, 80, COL_GOLD);
    DrawText("PRESS ENTER", SCREEN_WIDTH/2 - 100, 400, 30, WHITE);
    if(!assetsLoaded) DrawText("WARNING: ASSETS NOT FOUND", 10, 10, 20, RED);
}
