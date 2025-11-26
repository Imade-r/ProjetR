#include "map.h"
#include "assets.h"
#include <math.h>

EnvPlatform platforms[MAX_PLATFORMS];
int platformCount = 0;
Destructible destructibles[MAX_DESTRUCTIBLES];
int destructibleCount = 0;

float RandomMapFloat(float min, float max) { return min + (float)GetRandomValue(0, 10000) / 10000.0f * (max - min); }

void InitMap() {
    platformCount = 0;
    platforms[platformCount++] = (EnvPlatform){ (Rectangle){-500, 600, 3000, 400}, COL_GOLD, false }; 
    platforms[platformCount++] = (EnvPlatform){ (Rectangle){200, 400, 400, 40}, COL_GOLD, true }; 
    platforms[platformCount++] = (EnvPlatform){ (Rectangle){800, 300, 400, 40}, COL_GOLD, true };
    for(int i=0; i<5; i++) { destructibles[i] = (Destructible){ (Rectangle){RandomMapFloat(300, 1500), 400, 50, 50}, 30, true, WHITE }; }
}

void DrawDecor(Camera2D cam) {
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COL_SKY_TOP, COL_SKY_BOT);
    float parallaxScales[5] = { 0.0f, 0.1f, 0.3f, 0.5f, 0.8f }; 
    float scale = 3.0f; 
    for(int i=0; i<5; i++) {
        if(texBg[i].id <= 0 || texBg[i].width <= 0) continue;
        float xOffset = -(cam.target.x * parallaxScales[i]);
        float imgW = texBg[i].width * scale;
        float modX = fmodf(xOffset, imgW);
        if(modX > 0) modX -= imgW;
        for(int k=0; k<3; k++) DrawTextureEx(texBg[i], (Vector2){modX + imgW*k, 0}, 0.0f, scale, WHITE);
        DrawTextureEx(texBg[i], (Vector2){modX - imgW, 0}, 0.0f, scale, WHITE);
    }
}

void DrawPlatformStylish(EnvPlatform* p) {
    Rectangle r = p->rect;
    if (texTileDirt.id > 0) {
        DrawTexturePro(texTileDirt, (Rectangle){0,0,(float)texTileDirt.width,(float)texTileDirt.height}, r, (Vector2){0,0}, 0, WHITE);
        DrawTexturePro(texTileGrass, (Rectangle){0,0,(float)texTileGrass.width,(float)texTileGrass.height}, (Rectangle){r.x, r.y - 5, r.width, 20}, (Vector2){0,0}, 0, WHITE);
    } else {
        DrawRectangleRec(r, BROWN);
        DrawRectangleLinesEx(r, 2, BLACK);
    }
}

void DrawMap() {
    for(int i=0; i<platformCount; i++) DrawPlatformStylish(&platforms[i]);
    for(int i=0; i<MAX_DESTRUCTIBLES; i++) if(destructibles[i].active) {
        if(texObjCrate.id > 0) DrawTexturePro(texObjCrate, (Rectangle){0,0,texObjCrate.width,texObjCrate.height}, destructibles[i].rect, (Vector2){0,0}, 0, WHITE);
        else DrawRectangleRec(destructibles[i].rect, ORANGE);
    }
}
