#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"

// Textures globales accessibles partout
extern Texture2D texBg[5];         
extern Texture2D texCowboyIdle, texCowboyRun, texCowboyAttack;  
extern Texture2D texTileDirt, texTileGrass, texObjCrate, texObjCactus;     
extern Texture2D texGuns[12], texAmmo[3];
extern bool assetsLoaded;

void InitAssets();
void UnloadAssets();

#endif
