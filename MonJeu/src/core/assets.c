#include "assets.h"
#include <stdio.h>

Texture2D texBg[5];         
Texture2D texCowboyIdle, texCowboyRun, texCowboyAttack;  
Texture2D texTileDirt, texTileGrass, texObjCrate, texObjCactus;     
Texture2D texGuns[12], texAmmo[3];
bool assetsLoaded = false;

void InitAssets() {
    texBg[0] = LoadTexture("assets/Backgrounds/background_valley-Sheet1.png");
    texBg[1] = LoadTexture("assets/Backgrounds/background_valley-Sheet2.png");
    texBg[2] = LoadTexture("assets/Backgrounds/background_valley-Sheet3.png");
    texBg[3] = LoadTexture("assets/Backgrounds/background_valley-Sheet4.png");
    texBg[4] = LoadTexture("assets/Backgrounds/background_valley-Sheet5.png");

    texCowboyIdle = LoadTexture("assets/Sprites/idle/cowboy_idle_spritesheet.png");
    texCowboyRun = LoadTexture("assets/Sprites/walk/cowboy_walk_right-spritesheet.png");
    texCowboyAttack = LoadTexture("assets/Sprites/attack & reload/cowboy_attack_right-spritesheet.png");

    texTileDirt = LoadTexture("assets/tileset/Tile/2.png");
    texTileGrass = LoadTexture("assets/tileset/Tile/1.png");
    texObjCrate = LoadTexture("assets/tileset/Objects/Crate.png");
    texObjCactus = LoadTexture("assets/tileset/Objects/Cactus (1).png");

    texGuns[COLT] = LoadTexture("assets/Guns/Revolver.png"); 
    texGuns[WINCHESTER] = LoadTexture("assets/Guns/M24.png"); 
    texGuns[SHOTGUN] = LoadTexture("assets/Guns/SawedOffShotgun.png");
    texGuns[AK47] = LoadTexture("assets/Guns/AK47.png");
    texGuns[GATLING] = LoadTexture("assets/Guns/MP5.png");
    texGuns[M4A1] = LoadTexture("assets/Guns/M15.png");
    texGuns[RPG] = LoadTexture("assets/Guns/AK47.png");
    
    texAmmo[0] = LoadTexture("assets/Bullets/PistolAmmoBig.png");
    texAmmo[1] = LoadTexture("assets/Bullets/RifleAmmoBig.png");
    texAmmo[2] = LoadTexture("assets/Bullets/ShotgunShellBig.png");

    if (texCowboyIdle.id > 0) assetsLoaded = true;
}

void UnloadAssets() {
    for(int i=0; i<5; i++) UnloadTexture(texBg[i]);
    UnloadTexture(texCowboyIdle); UnloadTexture(texCowboyRun); UnloadTexture(texCowboyAttack);
    UnloadTexture(texTileDirt); UnloadTexture(texTileGrass);
    UnloadTexture(texObjCrate); UnloadTexture(texObjCactus);
    for(int i=0; i<12; i++) if(texGuns[i].id > 0) UnloadTexture(texGuns[i]);
    for(int i=0; i<3; i++) UnloadTexture(texAmmo[i]);
}
