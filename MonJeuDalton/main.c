/*******************************************************************************************
*
* BLAST BRAWL : ÉDITION "SILENT WESTERN" (CORRECTIF FINAL COMPILATION)
* -------------------------------------------------------------------
* Auteur : Gemini
*
* CORRECTIFS :
* - Ajout des particules manquantes : SHOCKWAVE et DROOL.
* - Ajout des couleurs manquantes : COL_WOOD_LIGHT et COL_METAL.
* - Le jeu compile maintenant sans erreur 'undeclared'.
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

// ============================================================================
// --- 1. CONSTANTES ---
// ============================================================================

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define TARGET_FPS 60

// --- ÉCHELLE ET PHYSIQUE ---
#define PLAYER_SCALE 1.65f 
#define BASE_WIDTH 40.0f
#define BASE_HEIGHT 60.0f

#define GRAVITY 0.95f
#define FRICTION 0.82f
#define AIR_FRICTION 0.96f
#define JUMP_FORCE -22.0f
#define MOVE_SPEED_BASE 9.0f 
#define WALL_SLIDE_SPEED 4.0f
#define WALL_JUMP_FORCE_X 18.0f
#define WALL_JUMP_FORCE_Y -22.0f
#define BLAST_ZONE 500.0f
#define MAX_JUMPS 2

// Game Feel
#define COYOTE_TIME 0.1f
#define JUMP_BUFFER 0.15f
#define HITSTOP_LIGHT 0.05f
#define HITSTOP_MEDIUM 0.10f
#define HITSTOP_HEAVY 0.15f 
#define HITSTOP_EXTREME 0.30f
#define SQUASH_LAND 0.2f    
#define SQUASH_JUMP 0.2f    
#define SQUASH_SPEED 15.0f  
#define TRANSITION_SPEED 3.0f 

// Combat
#define PARRY_WINDOW 0.25f 
#define SHIELD_MAX 100.0f
#define SHIELD_REGEN 0.5f
#define DODGE_DURATION 0.3f
#define DODGE_COOLDOWN 1.5f
#define DODGE_SPEED 20.0f
#define ULT_MAX 100.0f
#define COMBO_RESET_TIME 1.5f
#define LUCKY_LUKE_INTERVAL 40.0f
#define MATCH_TIME 180.0f 
#define LASSO_MAX_DIST 600.0f

// Limites
#define MAX_PARTICLES 6000
#define MAX_PROJECTILES 300
#define MAX_ITEMS 20
#define MAX_PLATFORMS 50
#define MAX_DESTRUCTIBLES 20
#define MAX_PLAYERS 4
#define MAX_CLOUDS 30
#define MAX_CACTI 15
#define MAX_MOUNTAINS 8

// --- PALETTE ---
const Color COL_SKY_TOP = { 100, 149, 237, 255 };    
const Color COL_SKY_BOT = { 255, 228, 196, 255 };    
const Color COL_SUN = { 255, 215, 0, 255 };          
const Color COL_SAND_LIGHT = { 238, 214, 175, 255 }; 
const Color COL_SAND_DARK = { 193, 154, 107, 255 };  
const Color COL_WOOD_MAIN = { 139, 69, 19, 255 };    
const Color COL_WOOD_DETAIL = { 80, 40, 10, 255 };   
const Color COL_WOOD_LIGHT = { 160, 82, 45, 255 };   // Ajouté
const Color COL_METAL = { 112, 128, 144, 255 };      // Ajouté
const Color COL_ROCK = { 178, 34, 34, 255 };         

const Color COL_SMASH_BG_TOP = { 20, 10, 40, 255 };  
const Color COL_SMASH_BG_BOT = { 60, 20, 80, 255 };  
const Color COL_TILE_BG = { 255, 250, 240, 255 };    
const Color COL_TILE_BORDER = { 60, 30, 10, 255 };
const Color COL_READY_BAR = { 220, 20, 60, 220 };    

const Color COL_P1 = { 220, 20, 60, 255 };   
const Color COL_P2 = { 30, 144, 255, 255 };  
const Color COL_P3 = { 50, 205, 50, 255 };   
const Color COL_P4 = { 255, 215, 0, 255 };   

const Color COL_DALTON_YELLOW = { 255, 223, 0, 255 }; 
const Color COL_DALTON_STRIPE = { 20, 20, 20, 255 };  
const Color COL_JEANS = { 70, 130, 180, 255 };        
const Color COL_SKIN = { 255, 224, 189, 255 };        
const Color COL_GOLD = { 255, 215, 0, 255 };
const Color COL_PARRY = { 0, 255, 255, 255 };
const Color COL_FLASH_CYAN = { 0, 255, 255, 150 }; 
const Color COL_CYAN_SOLID = { 0, 255, 255, 255 }; 

// ============================================================================
// --- 2. DÉFINITIONS DES TYPES ---
// ============================================================================

typedef enum { MENU, MAP_SELECT, CHAR_SELECT, OPTIONS, COUNTDOWN, GAMEPLAY, ENDING, PAUSE } GameState;
typedef enum { JOE, WILLIAM, JACK, AVERELL } DaltonType;
typedef enum { COLT, WINCHESTER, SHOTGUN, DYNAMITE, GATLING, AK47, M4A1, MINIGUN, RPG, LASSO, BARREL, WEAPON_NONE } WeaponType;
typedef enum { PRISON, DESERT, TOWN, MINE, TRAIN } MapType;
// Ajout SHOCKWAVE et DROOL
typedef enum { CIRCLE, SQUARE, SPARK, SMOKE, MUZZLE, RING, STAR, SHELL, TUMBLEWEED, EXPLOSION, CONFETTI, FLASH, ICON_WEAPON, IMPACT_CIRCLE, SHOCKWAVE, DROOL } ParticleType;

typedef struct {
    bool left, right, up, down, jump, jumpPressed, shoot, shield, shieldPressed, taunt;
} InputState;

typedef struct {
    const char* name; const char* desc; Color skyTop; Color skyBottom; float width; float height;
} MapInfo;

typedef struct {
    char name[32]; int damage; float fireRate; int ammoMax; float recoil; float knockback; float speed; float hitstop; Color color;
} WeaponData;

typedef struct { bool enabled; } AudioManager;

typedef struct {
    Vector2 position; Vector2 velocity; Color color; ParticleType type;
    float size; float startSize; float life; float maxLife; float rotation;
    int data; 
    bool active; 
} Particle;

typedef struct {
    Rectangle rect; Color color; bool canDropThrough; bool moving; Vector2 startPos, endPos; float moveSpeed, moveOffset;
    float landShake; 
} EnvPlatform;

typedef struct {
    Rectangle rect; float health; bool active; Color color;
} Destructible;

typedef struct {
    int speed, weight, power, jump;
} DaltonStats;

typedef struct {
    int id; Rectangle rect; Vector2 velocity; DaltonType type; bool isAI; int aiLevel; bool isMaDalton; bool isReady;
    float moveSpeed, jumpForce, weight; int stocks; float damagePercent; 
    bool isGrounded, facingRight, isDead, onWall; 
    int jumpCount; 
    float coyoteTimer, jumpBuffer; float shootCooldown, invincibilityTimer; int hitStunFrames;
    float shieldHealth; bool shieldBroken, isShielding, isParrying; float parryTimer;
    bool isDodging; float dodgeTimer, dodgeCooldown; 
    int kills; int parries; int comboCount; float comboTimer; 
    float ultCharge; bool ultActive; float ultTimer;
    
    // Anim
    float runAnimTimer; 
    float gunRecoilAnim;
    
    Color color; 
    WeaponType currentWeapon; int ammo; 
    InputState inputs; 
} Player;

typedef struct {
    Vector2 pos; Vector2 vel; int ownerId; WeaponType type; int damage; float life; bool active; bool reflected;
    float fuseTimer; float angle;
    Vector2 startPos; 
    bool attached; 
    int targetId; 
} Projectile;

typedef struct {
    Vector2 pos; WeaponType type; bool active; Rectangle rect;
    bool isFalling; float swingAngle;
} GameItem;

typedef struct { Vector2 pos; float speed; float size; } Cloud;
typedef struct { Vector2 pos; float size; Color col; float swayOffset; } Cactus;
typedef struct { Vector2 pos; float width; float height; Color color; float parallaxFactor; } Mountain;

// ============================================================================
// --- 3. PROTOTYPES ---
// ============================================================================

float RandomFloat(float min, float max);
int RandomInt(int min, int max);
Color GetPlayerColor(int id);
WeaponData GetWeaponData(WeaponType type);
DaltonStats GetStatValues(DaltonType type);
const char* GetQuote(DaltonType type);

void InitAudio();
void PlayAudio(const char* key);

void SpawnParticle(Vector2 pos, Vector2 vel, Color col, ParticleType type, float size, float life, int data);
void CreateExplosion(Vector2 pos, Color color, int intensity);
void UpdateParticles(float dt);
void DrawParticles();
void ClearParticles();

void InitDecor();
void UpdateDecor(float dt);
void DrawRantanplan();
void DrawDecor(Camera2D cam); 
void DrawSmashBackground();
void DrawPlatformStylish(EnvPlatform* p);
void DrawWeaponMini(WeaponType type, Vector2 pos, float size, float angle); 

void StartTransition(GameState target);
void StartTearTransition(GameState target);
bool UpdateTransition(float dt, GameState* currentState);
void DrawTransition();

// Drawing Clean
void DrawDaltonFace(float x, float y, float w, float h, DaltonType type);
void DrawDaltonInGame(Player* p, float x, float y, float h);
void DrawDaltonCleanBody(float x, float y, float h, DaltonType type, bool facingRight, float runFrame, bool isInvincible);
void DrawMenuButton(const char* text, float x, float y, float w, float h, bool selected);
void DrawCharacterTile(DaltonType type, float x, float y, float w, float h, bool highlighted);
void DrawPlayerDock(Player* p, float x, float y, float w, float h);
void DrawStatBar(const char* label, int val, int yOff, float barX, float barY, float barW);
void DrawHUD(Player* p, int i);
void DrawLuckyLukeEvent();
void DrawItemVisual(GameItem* item);

// Logic
void LoadMap(MapType t);
Vector2 GetSafeSpawn(int index, float mapW, float mapH);
void InitPlayer(Player* p, int id, DaltonType t, bool ai, int level);
void ResetPlayer(Player* p, Vector2 pos);
void SpawnProjectile(Vector2 pos, Vector2 vel, int ownerId, WeaponType type, int dmg);
void SpawnItem(float mapW);
void AddPlayers(int count);
void ResetRound();
void UpdateGameplay(float dt, float updateDt);
void SpawnDestructible(float mapW);
void UpdateGameCamera(int aliveCount);

// ============================================================================
// --- 4. VARIABLES GLOBALES ---
// ============================================================================

MapInfo mapInfos[5] = {
    {"PRISON DE YUMA", "Attention aux barreaux !", (Color){20, 20, 40, 255}, (Color){50, 50, 80, 255}, 2000, 1200},
    {"CANYON MORTEL", "Chaud devant !", (Color){135, 206, 235, 255}, (Color){255, 160, 122, 255}, 2200, 1200},
    {"DAISY TOWN", "Ville sans loi.", (Color){100, 200, 255, 255}, (Color){200, 230, 255, 255}, 2400, 1000},
    {"MINE D'OR", "Sombre et riche.", (Color){10, 10, 10, 255}, (Color){60, 40, 20, 255}, 2000, 1200},
    {"UNION PACIFIC", "Le train ne s'arrête pas.", (Color){100, 150, 255, 255}, (Color){255, 255, 200, 255}, 3000, 1000}
};

AudioManager audio;
Particle particles[MAX_PARTICLES];
EnvPlatform platforms[MAX_PLATFORMS];
int platformCount = 0;
Destructible destructibles[MAX_DESTRUCTIBLES];
int destructibleCount = 0;
Player players[MAX_PLAYERS];
int playerCount = 2;
Projectile projectiles[MAX_PROJECTILES];
GameItem items[MAX_ITEMS];
Cloud clouds[MAX_CLOUDS];
Cactus cacti[MAX_CACTI];
Mountain mountains[MAX_MOUNTAINS];
float sunY = 0;

GameState currentState = MENU;
int selectedMapId = 0;
int menuSelection = 0;
int optionsSelection = 0;
bool trainingMode = false;
bool fullscreen = false;
bool audioEnabled = true;

float gameTimer = 0;
float screenShake = 0;
float hitStop = 0;
float timeScale = 1.0f;
float spawnTimer = 0;
float luckyLukeTimer = 0;
float charSelectTimer = 0;
int winnerId = 0;
float suddenDeathShrink = 0;

float rantanplanX = -300, rantanplanDir = 1;
float tumbleweedTimer = 0;

Camera2D camera = { 0 };
Camera2D miniMapCam = { 0 };

float transAlpha = 0.0f;
bool transFadingIn = false, transActive = false, transTearing = false;
float transTearProgress = 0.0f;
GameState transTargetState;
float parryFlashAlpha = 0.0f; 

// ============================================================================
// --- 5. IMPLÉMENTATION ---
// ============================================================================

float RandomFloat(float min, float max) { return min + (float)GetRandomValue(0, 10000) / 10000.0f * (max - min); }
int RandomInt(int min, int max) { return GetRandomValue(min, max); }

Color GetPlayerColor(int id) {
    switch(id) {
        case 1: return COL_P1; case 2: return COL_P2; case 3: return COL_P3; case 4: return COL_P4; default: return WHITE;
    }
}

WeaponData GetWeaponData(WeaponType type) {
    switch (type) {
        case COLT: return (WeaponData){ "COLT .45", 6, 0.30f, 6, 5.0f, 6.0f, 40.0f, HITSTOP_LIGHT, COL_GOLD };
        case WINCHESTER: return (WeaponData){ "WINCHESTER", 12, 0.8f, 8, 10.0f, 9.0f, 50.0f, HITSTOP_MEDIUM, BROWN };
        case SHOTGUN: return (WeaponData){ "FUSIL A POMPE", 4, 1.0f, 2, 15.0f, 7.0f, 35.0f, HITSTOP_HEAVY, DARKGRAY }; 
        case DYNAMITE: return (WeaponData){ "DYNAMITE", 35, 0.5f, 3, 5.0f, 25.0f, 12.0f, HITSTOP_EXTREME, RED };
        case GATLING: return (WeaponData){ "GATLING", 3, 0.08f, 100, 2.0f, 2.0f, 45.0f, HITSTOP_LIGHT, DARKGRAY }; 
        case AK47: return (WeaponData){ "AK-47", 6, 0.12f, 30, 3.0f, 4.0f, 40.0f, HITSTOP_LIGHT, DARKBROWN };
        case M4A1: return (WeaponData){ "M4A1", 5, 0.10f, 30, 2.0f, 4.0f, 40.0f, HITSTOP_LIGHT, BLACK };
        case MINIGUN: return (WeaponData){ "MINIGUN", 2, 0.05f, 100, 1.0f, 2.0f, 50.0f, HITSTOP_LIGHT, DARKGRAY };
        case RPG: return (WeaponData){ "BAZOOKA", 40, 1.5f, 2, 20.0f, 20.0f, 25.0f, HITSTOP_EXTREME, GREEN };
        case LASSO: return (WeaponData){ "LASSO", 0, 1.0f, -1, 0.0f, -15.0f, 30.0f, HITSTOP_MEDIUM, ORANGE }; 
        case BARREL: return (WeaponData){ "TONNEAU", 30, 0.5f, 1, 0.0f, 30.0f, 10.0f, HITSTOP_HEAVY, BROWN }; 
        default: return (WeaponData){ "MAINS NUES", 0, 0, 0, 0, 0, 0, 0, WHITE };
    }
}

DaltonStats GetStatValues(DaltonType type) {
    switch(type) { 
        case JOE: return (DaltonStats){7, 2, 4, 6}; 
        case WILLIAM: return (DaltonStats){5, 4, 4, 5}; 
        case JACK: return (DaltonStats){4, 5, 5, 4}; 
        case AVERELL: return (DaltonStats){3, 6, 6, 3}; 
    } 
    return (DaltonStats){0,0,0,0};
}

const char* GetQuote(DaltonType type) { return ""; } 

void InitAudio() { InitAudioDevice(); if (IsAudioDeviceReady()) { audio.enabled = true; } else { audio.enabled = false; } }
void PlayAudio(const char* key) { if (!audio.enabled) return; }

// PARTICULES V2 (SANS TEXTE)
void SpawnParticle(Vector2 pos, Vector2 vel, Color col, ParticleType type, float size, float life, int data) {
    for(int i=0; i<MAX_PARTICLES; i++) {
        if(!particles[i].active) {
            particles[i].active = true; particles[i].position = pos; particles[i].velocity = vel; particles[i].color = col; particles[i].type = type;
            particles[i].size = size; particles[i].startSize = size; particles[i].life = life; particles[i].maxLife = life; particles[i].rotation = RandomFloat(0, 360);
            particles[i].data = data;
            return;
        }
    }
}

void CreateExplosion(Vector2 pos, Color color, int intensity) {
    int count = intensity / 2;
    SpawnParticle(pos, (Vector2){0,0}, WHITE, EXPLOSION, (float)intensity * 2.0f, 0.2f, 0);
    for(int i=0; i<count; i++) {
        float ang = RandomFloat(0, 360) * DEG2RAD; float spd = RandomFloat(5, 20);
        SpawnParticle(pos, (Vector2){cosf(ang)*spd, sinf(ang)*spd}, color, SPARK, RandomFloat(3,8), 0.4f, 0);
    }
    SpawnParticle(pos, (Vector2){0,-10}, GRAY, SMOKE, 40, 0.6f, 0);
}

void UpdateParticles(float dt) {
    for(int i=0; i<MAX_PARTICLES; i++) {
        if(!particles[i].active) continue;
        particles[i].position.x += particles[i].velocity.x * dt; particles[i].position.y += particles[i].velocity.y * dt;
        particles[i].life -= dt; 

        if (particles[i].type == SMOKE) {
            particles[i].size += 30.0f * dt; particles[i].velocity.y -= 30.0f * dt; 
            particles[i].color.a = (unsigned char)(255 * (particles[i].life / particles[i].maxLife));
            particles[i].rotation += 100.0f * dt;
        } else if (particles[i].type == EXPLOSION) {
            particles[i].size += 150.0f * dt;
            particles[i].color.a = (unsigned char)(255 * (particles[i].life / particles[i].maxLife));
        } else if (particles[i].type == TUMBLEWEED) {
            particles[i].rotation += particles[i].velocity.x * 2.0f * dt;
        } else if (particles[i].type == CONFETTI) {
            particles[i].velocity.y += 200.0f * dt; 
            particles[i].velocity.x *= 0.95f;
        } else if (particles[i].type == FLASH || particles[i].type == SHOCKWAVE || particles[i].type == RING || particles[i].type == IMPACT_CIRCLE) {
             particles[i].color.a = (unsigned char)(255 * (particles[i].life / particles[i].maxLife));
             particles[i].size += 80.0f * dt;
        } else if (particles[i].type == ICON_WEAPON) {
            particles[i].position.y -= 30.0f * dt; 
            particles[i].color.a = (unsigned char)(255 * (particles[i].life / particles[i].maxLife));
            particles[i].rotation += 180.0f * dt;
        } else if (particles[i].type == DROOL) {
            particles[i].velocity.y += 300.0f * dt;
        }

        if (particles[i].life <= 0 || particles[i].size <= 0.1f) particles[i].active = false;
    }
}

// DESSIN MINI ARME (VISUELLE)
void DrawWeaponMini(WeaponType type, Vector2 pos, float size, float angle) {
    Vector2 center = {size/2, size/4};
    Color c = GetWeaponData(type).color;
    
    if (type == COLT) {
        DrawRectanglePro((Rectangle){pos.x, pos.y, size, size/3}, center, angle, c);
        DrawRectanglePro((Rectangle){pos.x, pos.y+size/4, size/3, size/2}, (Vector2){size/6, 0}, angle+20, BROWN); 
    } else if (type == DYNAMITE) {
        DrawRectanglePro((Rectangle){pos.x, pos.y, size/3, size}, (Vector2){size/6, size/2}, angle, RED);
        if ((int)(GetTime()*20)%2==0) DrawCircleV(pos, size/4, YELLOW);
    } else if (type == GATLING || type == MINIGUN) {
        DrawCircleV(pos, size/2, DARKGRAY);
        DrawCircleLines((int)pos.x, (int)pos.y, size/2, c);
        DrawLineEx(pos, (Vector2){pos.x + cosf(angle*DEG2RAD)*size, pos.y + sinf(angle*DEG2RAD)*size}, 2, c);
    } else {
        DrawRectanglePro((Rectangle){pos.x, pos.y, size, size/2}, center, angle, c);
    }
}

void DrawParticles() {
    for(int i=0; i<MAX_PARTICLES; i++) {
        if(!particles[i].active) continue;
        Color c = particles[i].color;
        
        if (particles[i].type == TUMBLEWEED) {
            DrawCircleLines((int)particles[i].position.x, (int)particles[i].position.y, particles[i].size, BROWN);
        }
        else if (particles[i].type == STAR) {
            DrawPoly(particles[i].position, 5, particles[i].size, particles[i].rotation, c);
        }
        else if (particles[i].type == FLASH) {
            DrawCircleV(particles[i].position, particles[i].size, c);
        }
        else if (particles[i].type == SHOCKWAVE || particles[i].type == RING || particles[i].type == IMPACT_CIRCLE) {
            DrawCircleLines((int)particles[i].position.x, (int)particles[i].position.y, particles[i].size, c);
        }
        else if (particles[i].type == ICON_WEAPON) {
             DrawWeaponMini((WeaponType)particles[i].data, particles[i].position, particles[i].size, particles[i].rotation);
        }
        else if (particles[i].type == DROOL) {
             DrawCircleV(particles[i].position, particles[i].size, BLUE);
        }
        else DrawCircleV(particles[i].position, particles[i].size, c);
    }
}

void ClearParticles() { for(int i=0; i<MAX_PARTICLES; i++) particles[i].active = false; }

// DECOR & TRANSITIONS
void InitDecor() {
    for(int i=0; i<MAX_CLOUDS; i++) clouds[i] = (Cloud){{RandomFloat(0, 3000), RandomFloat(0, 300)}, RandomFloat(5, 20), RandomFloat(80, 150)};
    for(int i=0; i<MAX_CACTI; i++) cacti[i] = (Cactus){{RandomFloat(0, 3000), 0}, RandomFloat(0.8f, 2.0f), (RandomInt(0,1)?DARKGREEN:GREEN), RandomFloat(0, 100)};
    for(int i=0; i<MAX_MOUNTAINS; i++) mountains[i] = (Mountain){{RandomFloat(-500, 3500), 0}, RandomFloat(500, 1200), RandomFloat(300, 800), Fade(COL_ROCK, RandomFloat(0.5f, 1.0f)), RandomFloat(0.1f, 0.5f)};
}

void UpdateDecor(float dt) {
    for(int i=0; i<MAX_CLOUDS; i++) {
        clouds[i].pos.x += clouds[i].speed * dt;
        if (clouds[i].pos.x > 3000) clouds[i].pos.x = -200;
    }
    sunY = sinf((float)GetTime() * 0.1f) * 50.0f;

    rantanplanX += 5.0f * rantanplanDir * dt * 60.0f;
    if (rantanplanX > 3500) { rantanplanDir = -1; }
    if (rantanplanX < -500) { rantanplanDir = 1; }
    
    tumbleweedTimer -= dt;
    if (tumbleweedTimer <= 0) {
        SpawnParticle((Vector2){camera.target.x - SCREEN_WIDTH, SCREEN_HEIGHT - 120}, (Vector2){RandomFloat(200, 400), 0}, BROWN, TUMBLEWEED, 30, 15.0f, 0);
        tumbleweedTimer = RandomFloat(8, 20);
    }
}

// ANIMATION RANTANPLAN
void DrawRantanplan() {
    float x = rantanplanX; float y = SCREEN_HEIGHT + 50; 
    float scale = 3.0f;
    float bounce = sinf(GetTime() * 15.0f) * 10.0f;
    
    DrawRectangle((int)x, (int)(y + bounce), 60*scale, 30*scale, BROWN);
    DrawRectangle((int)(x + (rantanplanDir>0?50*scale:-10*scale)), (int)(y - 20*scale + bounce), 30*scale, 30*scale, BROWN);
    DrawRectangle((int)(x + (rantanplanDir>0?60*scale:-0*scale)), (int)(y - 40*scale + bounce), 5*scale, 20*scale, BLACK);
    float legSwing = sinf(GetTime() * 20.0f) * 20.0f;
    DrawRectangle((int)(x + 10*scale + legSwing), (int)(y + 25*scale + bounce), 10*scale, 20*scale, BROWN); 
    DrawRectangle((int)(x + 40*scale - legSwing), (int)(y + 25*scale + bounce), 10*scale, 20*scale, BROWN);

    // Gueule ouverte + Bave
    if ((int)(GetTime() * 4) % 2 == 0) {
        DrawRectangle((int)(x + (rantanplanDir>0?70*scale:0)), (int)(y - 10*scale + bounce), 10*scale, 10*scale, PINK); 
        if (RandomInt(0,10) < 2) {
             SpawnParticle((Vector2){x + (rantanplanDir>0?80*scale:-10), y - 10*scale + bounce}, (Vector2){0,0}, BLUE, DROOL, 5, 0.5f, 0);
        }
    }
}

void DrawDecor(Camera2D cam) {
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COL_SKY_TOP, COL_SKY_BOT);
    DrawCircle(SCREEN_WIDTH/2, (int)(SCREEN_HEIGHT*0.2f + sunY), 150, COL_SUN);

    for(int i=0; i<MAX_MOUNTAINS; i++) {
        float paraX = mountains[i].pos.x - (cam.target.x * mountains[i].parallaxFactor);
        float paraY = SCREEN_HEIGHT; 
        Vector2 p1 = {paraX - mountains[i].width/2, paraY};
        Vector2 p2 = {paraX + mountains[i].width/2, paraY};
        Vector2 p3 = {paraX, paraY - mountains[i].height};
        DrawTriangle(p3, p1, p2, mountains[i].color);
    }

    for(int i=0; i<MAX_CLOUDS; i++) {
        float cloudX = clouds[i].pos.x - (cam.target.x * 0.2f);
        DrawRectangleRounded((Rectangle){cloudX, clouds[i].pos.y, clouds[i].size*3, clouds[i].size}, 0.8f, 6, (Color){255,255,255,150});
    }

    DrawRectangle(0, SCREEN_HEIGHT*0.8f, SCREEN_WIDTH, SCREEN_HEIGHT*0.2f, COL_SAND_LIGHT); 
}

void DrawSmashBackground() {
    float t = (float)GetTime();
    Color top = COL_SMASH_BG_TOP;
    Color bot = COL_SMASH_BG_BOT;
    
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, top, bot);
    for(int i=0; i<5; i++) {
        DrawTriangle((Vector2){i*300 + t*20, 720}, (Vector2){i*300 + 150 + t*20, 0}, (Vector2){i*300 + 300 + t*20, 720}, Fade(WHITE, 0.05f));
    }
}

void DrawPlatformStylish(EnvPlatform* p) {
    float shakeY = p->landShake; 
    
    DrawRectangleRec((Rectangle){p->rect.x+10, p->rect.y+10 + shakeY, p->rect.width, p->rect.height}, Fade(BLACK, 0.4f));
    DrawRectangleRec((Rectangle){p->rect.x, p->rect.y + shakeY, p->rect.width, p->rect.height}, COL_WOOD_MAIN);
    DrawRectangleLinesEx((Rectangle){p->rect.x, p->rect.y + shakeY, p->rect.width, p->rect.height}, 4, COL_WOOD_DETAIL); 
    
    int planks = (int)(p->rect.width / 50);
    for(int i=1; i<planks; i++) {
        DrawLineEx((Vector2){p->rect.x + i*50, p->rect.y + shakeY}, (Vector2){p->rect.x + i*50, p->rect.y + p->rect.height + shakeY}, 3, COL_WOOD_DETAIL);
        DrawCircle((int)(p->rect.x + i*50 - 5), (int)(p->rect.y + 8 + shakeY), 3, DARKGRAY);
        DrawCircle((int)(p->rect.x + i*50 - 5), (int)(p->rect.y + p->rect.height - 8 + shakeY), 3, DARKGRAY);
    }
    
    DrawRectangle(p->rect.x, p->rect.y + shakeY, p->rect.width, 5, COL_SAND_LIGHT);

    if (p->moving) {
        DrawLineEx((Vector2){p->rect.x, p->rect.y}, (Vector2){p->rect.x, 0}, 2, BLACK);
        DrawLineEx((Vector2){p->rect.x + p->rect.width, p->rect.y}, (Vector2){p->rect.x + p->rect.width, 0}, 2, BLACK);
    }
}

void StartTransition(GameState target) { if(transActive)return; transTargetState=target; transFadingIn=true; transActive=true; transAlpha=0.0f; }
void StartTearTransition(GameState target) { if(transActive)return; transTargetState=target; transTearing=true; transActive=true; transTearProgress=0.0f; }
bool UpdateTransition(float dt, GameState* currentState) {
    if(!transActive) return false;
    if(transTearing) {
        transTearProgress+=dt*2.5f; if(transTearProgress>=1.0f){*currentState=transTargetState; transTearing=false; transActive=false; return true;} return false;
    }
    if(transFadingIn){ transAlpha+=TRANSITION_SPEED*dt; if(transAlpha>=1.0f){transAlpha=1.0f; transFadingIn=false; *currentState=transTargetState; return true;} }
    else{ transAlpha-=TRANSITION_SPEED*dt; if(transAlpha<=0.0f){transAlpha=0.0f; transActive=false;} }
    return false;
}
void DrawTransition() {
    if(transTearing) {
         float split=transTearProgress*SCREEN_WIDTH*0.6f;
         DrawRectangle(0,0,SCREEN_WIDTH/2-(int)split,SCREEN_HEIGHT,BLACK); DrawRectangle(SCREEN_WIDTH/2+(int)split,0,SCREEN_WIDTH/2,SCREEN_HEIGHT,BLACK);
         DrawRectangleLinesEx((Rectangle){(float)SCREEN_WIDTH/2-split-10,0,20,(float)SCREEN_HEIGHT},10,WHITE); DrawRectangleLinesEx((Rectangle){(float)SCREEN_WIDTH/2+split-10,0,20,(float)SCREEN_HEIGHT},10,WHITE);
    } else if(transAlpha>0.0f) DrawRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,Fade(BLACK,transAlpha));
    
    if(parryFlashAlpha > 0.0f) {
        DrawRectangle(0,0,SCREEN_WIDTH, SCREEN_HEIGHT, Fade(COL_FLASH_CYAN, parryFlashAlpha));
    }
}

// ============================================================================
// --- UI & DRAWING ---
// ============================================================================

void DrawMenuButton(const char* text, float x, float y, float w, float h, bool selected) {
    Rectangle rect = { x, y, w, h };
    DrawRectangleRec((Rectangle){x+5, y+5, w, h}, Fade(BLACK, 0.5f));
    if(selected) {
        DrawRectangleRec(rect, COL_WOOD_MAIN);
        DrawRectangleLinesEx(rect, 3, COL_GOLD);
        DrawText(text, (int)(rect.x + rect.width/2 - MeasureText(text, 30)/2), (int)(rect.y + rect.height/2 - 15), 30, WHITE);
    } else {
        DrawRectangleRec(rect, COL_WOOD_DETAIL);
        DrawRectangleLinesEx(rect, 2, BLACK);
        DrawText(text, (int)(rect.x + rect.width/2 - MeasureText(text, 20)/2), (int)(rect.y + rect.height/2 - 10), 20, LIGHTGRAY);
    }
}

void DrawStatBar(const char* label, int val, int yOff, float barX, float barY, float barW) {
    DrawText(label, (int)barX, (int)(barY + yOff), 10, WHITE);
    DrawRectangle((int)(barX + 40), (int)(barY + yOff), (int)(barW-40), 10, DARKGRAY);
    DrawRectangle((int)(barX + 40), (int)(barY + yOff), (int)((barW-40) * (val/7.0f)), 10, COL_GOLD);
}

void DrawDaltonFace(float x, float y, float w, float h, DaltonType type) {
    DrawRectangleRec((Rectangle){x - w/2, y - h/2, w, h}, COL_SKIN);
    DrawRectangle(x - w/2, y - h/2, w, h * 0.25f, BLACK); 
    DrawRectangle(x - w*0.6f, y - h/2 + h*0.25f, w*1.2f, 5, BLACK); 

    float eyeY = y - h*0.1f;
    float eyeSpace = w * 0.2f;
    
    DrawCircle(x - eyeSpace, eyeY, 6, WHITE); 
    DrawCircle(x + eyeSpace, eyeY, 6, WHITE);
    DrawCircle(x - eyeSpace, eyeY, 2, BLACK); 
    DrawCircle(x + eyeSpace, eyeY, 2, BLACK);

    DrawCircle(x, eyeY + 12, 5, (Color){230, 180, 160, 255});

    if(type == JOE) {
        DrawLineEx((Vector2){x - eyeSpace - 8, eyeY - 10}, (Vector2){x, eyeY}, 3, BLACK);
        DrawLineEx((Vector2){x + eyeSpace + 8, eyeY - 10}, (Vector2){x, eyeY}, 3, BLACK);
        DrawRectangle(x - 10, eyeY + 25, 20, 3, BLACK);
    } else if (type == AVERELL) {
        DrawCircle(x, eyeY + 25, 8, BLACK);
    } else {
        DrawRectangle(x - 10, eyeY + 25, 20, 2, BLACK);
        if(type == JACK) DrawRectangle(x - 8, eyeY + 20, 16, 3, BLACK); 
    }
}

void DrawCharacterTile(DaltonType type, float x, float y, float w, float h, bool highlighted) {
    Rectangle r = {x, y, w, h};
    DrawRectangleRec(r, COL_TILE_BG);
    DrawRectangleLinesEx(r, 4, COL_TILE_BORDER);
    float faceSize = w * 0.7f; 
    DrawDaltonFace(x + w/2, y + h/2 + 10, faceSize, faceSize, type);
    if(highlighted) DrawRectangleLinesEx(r, 6, COL_GOLD);
}

void DrawDaltonCleanBody(float x, float y, float h, DaltonType type, bool facingRight, float runFrame, bool isInvincible) {
    float scaleH = h;
    if (type == JOE) scaleH = h * 0.75f;
    else if (type == WILLIAM) scaleH = h * 0.9f;
    else if (type == JACK) scaleH = h * 1.05f;
    else if (type == AVERELL) scaleH = h * 1.25f;

    float w = scaleH * 0.45f; 
    float drawY = y; 

    DrawEllipse(x, y, w*0.9f, 8, Fade(BLACK, 0.2f));

    float legAngle = sinf(runFrame * 10.0f) * 25.0f; 
    if (runFrame == 0) legAngle = 0;

    Vector2 hipL = {x - w*0.25f, drawY - scaleH*0.35f};
    Vector2 footL = {x - w*0.25f + sinf(legAngle*DEG2RAD)*20, drawY};
    DrawLineEx(hipL, footL, w*0.28f, COL_JEANS);
    
    Vector2 hipR = {x + w*0.25f, drawY - scaleH*0.35f};
    Vector2 footR = {x + w*0.25f - sinf(legAngle*DEG2RAD)*20, drawY};
    DrawLineEx(hipR, footR, w*0.28f, COL_JEANS);

    DrawRectangle(x - w/2, drawY - scaleH, w, scaleH * 0.65f, COL_DALTON_YELLOW);
    int stripes = 4;
    float stripeH = (scaleH * 0.65f) / stripes;
    for(int i=0; i<stripes; i++) {
        if(i%2!=0) DrawRectangle(x - w/2, drawY - scaleH + i*stripeH, w, stripeH, COL_DALTON_STRIPE);
    }

    DrawRectangle(x - w/2 - 2, drawY - scaleH*0.35f - 5, w + 4, 8, BROWN);
    DrawRectangle(x - 3, drawY - scaleH*0.35f - 5, 6, 8, GOLD); 

    float headSize = w * 1.1f; 
    DrawDaltonFace(x, drawY - scaleH - headSize*0.4f, headSize, headSize, type);

    if(isInvincible) {
        DrawCircleLines(x, drawY - scaleH/2, scaleH/1.5f, Fade(WHITE, 0.5f));
    }
}

void DrawPlayerDock(Player* p, float x, float y, float w, float h) {
    Rectangle r = {x, y, w, h};
    DrawRectangleRec(r, COL_WOOD_DETAIL);
    DrawRectangleLinesEx(r, 4, COL_WOOD_MAIN);
    
    if(p->isReady) {
        DrawRectangleRec(r, Fade(p->color, 0.1f)); 
    }

    DrawText(TextFormat("P%d", p->id), (int)x + 10, (int)y + 10, 20, p->color);
    
    if(!p->isAI || p->id == 1) { 
        const char* name = (p->type == JOE) ? "JOE" : (p->type == WILLIAM) ? "WILLIAM" : (p->type == JACK) ? "JACK" : "AVERELL";
        DrawText(name, (int)(x + w/2 - MeasureText(name, 40)/2), (int)(y + h - 50), 40, WHITE);
        DaltonStats s = GetStatValues(p->type);
        DrawStatBar("SPD", s.speed, 20, x + w - 100, y + 20, 90);
        DrawStatBar("POW", s.power, 40, x + w - 100, y + 20, 90);
        DrawDaltonCleanBody(x + w/2, y + h - 70, 140, p->type, true, 0, false); 
    } else {
        DrawText("CPU", (int)(x + w/2 - MeasureText("CPU", 30)/2), (int)(y + h/2), 30, GRAY);
    }

    if(p->isReady) {
        DrawText("READY", (int)(x + w/2 - MeasureText("READY", 50)/2), (int)(y + h/2 - 25), 50, COL_GOLD);
    }
}

void DrawDaltonInGame(Player* p, float x, float y, float h) {
    DrawDaltonCleanBody(x, y + p->rect.height, h, p->type, p->facingRight, p->runAnimTimer, p->invincibilityTimer > 0); 

    DrawTriangle((Vector2){x - 12, y - 30}, (Vector2){x + 12, y - 30}, (Vector2){x, y - 15}, p->color);

    float dir = p->facingRight ? 1.0f : -1.0f;
    float w = p->rect.width;
    Vector2 handPos = { x + (w/2 * dir), y + h * 0.55f }; 
    Color weaponCol = GetWeaponData(p->currentWeapon).color;
    
    if (p->currentWeapon == COLT) {
        DrawRectangle((int)handPos.x, (int)handPos.y, 15*dir, 6, weaponCol);
    } else if (p->currentWeapon == DYNAMITE) {
        DrawRectangle((int)handPos.x, (int)handPos.y, 6, 15, RED);
    } else {
        DrawRectangle((int)handPos.x, (int)handPos.y, 25*dir, 10, weaponCol);
    }

    if (p->isShielding) DrawCircleLines((int)x, (int)(y + h/2), h * 0.8f, SKYBLUE);
    if (p->isParrying) {
        DrawCircleLines((int)x, (int)(y + h/2), h * 1.1f, COL_CYAN_SOLID);
        DrawCircleLines((int)x, (int)(y + h/2), h * 1.2f, COL_CYAN_SOLID);
    }
    
    if (p->ultCharge >= ULT_MAX) {
         DrawCircleLines((int)x, (int)(y + h/2), h, GOLD); // Simple Aura
    }
}

// CAISSE D'ARME REDESIGN
void DrawItemVisual(GameItem* item) {
    if(item->isFalling) {
        Vector2 top = {item->pos.x, item->pos.y - 60};
        DrawLineEx((Vector2){top.x - 20, top.y}, item->pos, 1, WHITE);
        DrawLineEx((Vector2){top.x + 20, top.y}, item->pos, 1, WHITE);
        DrawCircleSector(top, 30, 180, 360, 10, WHITE); // Parachute simple
    }
    
    Rectangle boxRect = item->rect;
    DrawRectangleRec(boxRect, COL_WOOD_LIGHT);
    DrawRectangleLinesEx(boxRect, 3, COL_WOOD_MAIN);
    
    // Coins
    DrawRectangle(boxRect.x, boxRect.y, 8, 8, COL_METAL);
    DrawRectangle(boxRect.x + boxRect.width - 8, boxRect.y, 8, 8, COL_METAL);
    DrawRectangle(boxRect.x, boxRect.y + boxRect.height - 8, 8, 8, COL_METAL);
    DrawRectangle(boxRect.x + boxRect.width - 8, boxRect.y + boxRect.height - 8, 8, 8, COL_METAL);

    // Fenêtre Ronde
    Vector2 center = {boxRect.x + boxRect.width/2, boxRect.y + boxRect.height/2};
    DrawCircleV(center, 15, Fade(BLACK, 0.5f));
    DrawCircleLines(center.x, center.y, 15, COL_GOLD);
    
    float angle = GetTime() * 120.0f;
    DrawWeaponMini(item->type, center, 12, angle);
}

void DrawHUD(Player* p, int i) {
    int x = 20 + i * 310; int y = SCREEN_HEIGHT - 110;
    
    Rectangle hudRect = {x, y, 280, 100};
    DrawRectangleRec(hudRect, COL_WOOD_MAIN);
    DrawRectangleLinesEx(hudRect, 4, COL_WOOD_DETAIL);
    
    DrawRectangle(x+10, y+10, 60, 80, COL_TILE_BG);
    DrawDaltonFace(x+40, y+50, 50, 50, p->type);
    
    DrawText(TextFormat("%d%%", (int)p->damagePercent), x+80, y+10, 50, p->damagePercent>100?RED:WHITE);
    
    DrawRectangle(x+80, y+70, 180, 15, DARKGRAY);
    DrawRectangle(x+80, y+70, (int)(p->ultCharge/ULT_MAX * 180), 15, p->ultCharge>=ULT_MAX ? RED : ORANGE); 
    
    for(int s=0; s<p->stocks; s++) DrawPoly((Vector2){x+260-s*25, y+25}, 5, 10, 0, COL_GOLD);
    
    if(p->comboCount > 1) {
         for(int c=0; c<p->comboCount && c < 5; c++) DrawPoly((Vector2){x + 200 + c*15, y - 10}, 5, 6, 0, ORANGE);
    }
}

void DrawLuckyLukeEvent() {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.9f));
    DrawCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 80, WHITE);
    DrawRectangle(SCREEN_WIDTH/2 - 15, SCREEN_HEIGHT/2 + 20, 30, 80, BLACK); 
    if((int)(GetTime()*15)%2==0) DrawCircle(SCREEN_WIDTH/2 + 40, SCREEN_HEIGHT/2 + 10, 40, YELLOW);
}

// ============================================================================
// --- LOGIC CORE ---
// ============================================================================

void LoadMap(MapType t) {
    platformCount = 0; destructibleCount = 0;
    
    platforms[platformCount++] = (EnvPlatform){ (Rectangle){-500, 600, 3000, 400}, COL_WOOD_MAIN, false, false }; 
    platforms[platformCount++] = (EnvPlatform){ (Rectangle){200, 400, 400, 30}, COL_WOOD_MAIN, true, false };
    platforms[platformCount++] = (EnvPlatform){ (Rectangle){800, 300, 400, 30}, COL_WOOD_MAIN, true, false };
    
    platforms[platformCount++] = (EnvPlatform){ (Rectangle){500, 200, 200, 30}, COL_WOOD_MAIN, true, true, {500, 200}, {900, 200}, 0.5f, 0 };
    
    SpawnDestructible(2000); SpawnDestructible(2000); SpawnDestructible(2000);
}

void SpawnDestructible(float mapW) {
    for(int i=0; i<MAX_DESTRUCTIBLES; i++) {
        if(!destructibles[i].active) {
            destructibles[i] = (Destructible){ (Rectangle){RandomFloat(200, mapW-200), 400, 60, 60}, 30, true, ORANGE }; 
            return;
        }
    }
}

Vector2 GetSafeSpawn(int index, float mapW, float mapH) {
    return (Vector2){ 300 + index*200, 300 };
}

void InitPlayer(Player* p, int id, DaltonType t, bool ai, int level) {
    p->id = id; p->type = t; p->isAI = ai; p->aiLevel = level; p->isMaDalton = false; p->isReady = false; p->stocks = 3; 
    p->color = GetPlayerColor(id);
    
    switch (t) {
        case JOE: p->moveSpeed = MOVE_SPEED_BASE * 1.2f; p->weight = 0.9f; p->jumpForce = JUMP_FORCE * 1.05f; p->rect = (Rectangle){0,0, BASE_WIDTH*PLAYER_SCALE, BASE_HEIGHT*PLAYER_SCALE*0.8f}; break;
        case WILLIAM: p->moveSpeed = MOVE_SPEED_BASE * 1.0f; p->weight = 1.0f; p->jumpForce = JUMP_FORCE; p->rect = (Rectangle){0,0, BASE_WIDTH*PLAYER_SCALE, BASE_HEIGHT*PLAYER_SCALE}; break;
        case JACK: p->moveSpeed = MOVE_SPEED_BASE * 0.9f; p->weight = 1.2f; p->jumpForce = JUMP_FORCE * 0.95f; p->rect = (Rectangle){0,0, BASE_WIDTH*PLAYER_SCALE, BASE_HEIGHT*PLAYER_SCALE*1.1f}; break;
        case AVERELL: p->moveSpeed = MOVE_SPEED_BASE * 0.8f; p->weight = 1.4f; p->jumpForce = JUMP_FORCE * 0.9f; p->rect = (Rectangle){0,0, BASE_WIDTH*PLAYER_SCALE, BASE_HEIGHT*PLAYER_SCALE*1.3f}; break;
    }
    p->damagePercent = 0; p->ultCharge = 0; p->shieldHealth = SHIELD_MAX;
    p->runAnimTimer = 0;
}

void ResetPlayer(Player* p, Vector2 pos) {
    p->rect.x = pos.x; p->rect.y = pos.y;
    p->velocity = (Vector2){0,0}; p->damagePercent = 0; p->shieldHealth = SHIELD_MAX;
    p->shieldBroken = false; p->isShielding = false; p->isParrying = false; p->isDodging = false; p->isDead = false;
    p->currentWeapon = COLT; p->ammo = -1; 
    p->invincibilityTimer = 3.0f; p->ultCharge = 0; p->ultActive = false;
    p->comboCount = 0; p->jumpCount = 0;
    p->runAnimTimer = 0;
}

void SpawnProjectile(Vector2 pos, Vector2 vel, int ownerId, WeaponType type, int dmg) {
    for(int i=0; i<MAX_PROJECTILES; i++) {
        if(!projectiles[i].active) { 
            projectiles[i] = (Projectile){pos, vel, ownerId, type, dmg, 2.0f, true, false, 2.0f, 0}; 
            if(type == DYNAMITE) projectiles[i].life = 2.5f; 
            if(type == LASSO) { projectiles[i].life = 0.6f; projectiles[i].startPos = pos; }
            return; 
        }
    }
}
void SpawnItem(float mapW) {
    for(int i=0; i<MAX_ITEMS; i++) {
        if(!items[i].active) { 
            items[i] = (GameItem){ {RandomFloat(200, 1200), -200}, (WeaponType)RandomInt(0, 8), true, (Rectangle){0,0,50,50}, true, 0 }; 
            return; 
        }
    }
}

void AddPlayers(int count) {
    playerCount = count;
    for(int i=0; i<count; i++) InitPlayer(&players[i], i+1, (DaltonType)(i%4), false, 0);
}

void ResetRound() {
    LoadMap((MapType)selectedMapId);
    for(int i=0; i<MAX_PROJECTILES; i++) projectiles[i].active = false;
    for(int i=0; i<MAX_ITEMS; i++) items[i].active = false;
    ClearParticles();
    for(int i=0; i<playerCount; i++) ResetPlayer(&players[i], GetSafeSpawn(i, 2000, 1000));
    timeScale = 1.0f; luckyLukeTimer = LUCKY_LUKE_INTERVAL;
}

void UpdateGameCamera(int aliveCount) {
    if (aliveCount == 0) return;
    
    Vector2 minPos = {10000, 10000};
    Vector2 maxPos = {-10000, -10000};
    
    for(int i=0; i<playerCount; i++) {
        if(!players[i].isDead) {
            if(players[i].rect.x < minPos.x) minPos.x = players[i].rect.x;
            if(players[i].rect.y < minPos.y) minPos.y = players[i].rect.y;
            if(players[i].rect.x > maxPos.x) maxPos.x = players[i].rect.x;
            if(players[i].rect.y > maxPos.y) maxPos.y = players[i].rect.y;
        }
    }
    
    Vector2 center = Vector2Scale(Vector2Add(minPos, maxPos), 0.5f);
    float width = maxPos.x - minPos.x;
    float height = maxPos.y - minPos.y;
    
    float zoomW = SCREEN_WIDTH / (width + 700.0f); 
    float zoomH = SCREEN_HEIGHT / (height + 500.0f);
    float targetZoom = (zoomW < zoomH) ? zoomW : zoomH;
    
    if (targetZoom < 0.5f) targetZoom = 0.5f;
    if (targetZoom > 1.1f) targetZoom = 1.1f;
    
    camera.target = Vector2Lerp(camera.target, center, 0.1f);
    camera.zoom = Lerp(camera.zoom, targetZoom, 0.05f);
    camera.offset = (Vector2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
    
    if (screenShake > 0) {
        camera.offset.x += RandomFloat(-screenShake, screenShake);
        camera.offset.y += RandomFloat(-screenShake, screenShake);
    }
}

void UpdateGameplay(float dt, float updateDt) {
    timeScale += dt * 0.01f; 
    UpdateDecor(dt);
    UpdateParticles(dt);
    
    if (parryFlashAlpha > 0) parryFlashAlpha -= dt * 3.0f;

    for(int i=0; i<MAX_ITEMS; i++) {
        if(items[i].active) {
            if(items[i].isFalling) {
                items[i].pos.y += 3.0f * 60.0f * dt; 
                items[i].swingAngle = sinf(GetTime() * 3.0f) * 20.0f;
                items[i].pos.x += sinf(GetTime() * 3.0f) * 2.0f; 
                for(int j=0; j<platformCount; j++) {
                    if(CheckCollisionCircleRec(items[i].pos, 25, platforms[j].rect)) {
                        items[i].isFalling = false; items[i].pos.y = platforms[j].rect.y - 25;
                        platforms[j].landShake = 5.0f; 
                    }
                }
            }
            for(int p=0; p<playerCount; p++) {
                if(!players[p].isDead && CheckCollisionCircleRec(items[i].pos, 30, players[p].rect)) {
                    players[p].currentWeapon = items[i].type;
                    items[i].active = false;
                    PlayAudio("ching");
                    SpawnParticle(items[i].pos, (Vector2){0,-10}, YELLOW, STAR, 10, 0.5f, 0);
                    SpawnParticle(items[i].pos, (Vector2){0,-10}, WHITE, STAR, 10, 0.5f, 0);
                    SpawnParticle(items[i].pos, (Vector2){0,-10}, GOLD, STAR, 10, 0.5f, 0);
                    SpawnParticle(items[i].pos, (Vector2){0,-30}, WHITE, ICON_WEAPON, 30, 1.5f, (int)items[i].type);
                }
            }
        }
    }
    
    for(int i=0; i<platformCount; i++) {
        if(platforms[i].landShake > 0) platforms[i].landShake = Lerp(platforms[i].landShake, 0, dt*10.0f);
    }

    spawnTimer -= dt; if(spawnTimer <= 0) { SpawnItem(2000); spawnTimer = RandomFloat(10, 25); }
    
    luckyLukeTimer -= dt; 
    if(luckyLukeTimer <= 0) { 
        CreateExplosion((Vector2){players[0].rect.x, players[0].rect.y}, COL_GOLD, 50); 
        luckyLukeTimer = LUCKY_LUKE_INTERVAL; 
        hitStop = 0.5f; 
    }

    for (int i=0; i<platformCount; i++) { if (platforms[i].moving) { float t = (sinf(GetTime() * platforms[i].moveSpeed) + 1.0f) / 2.0f; platforms[i].rect.x = Lerp(platforms[i].startPos.x, platforms[i].endPos.x, t); } }

    int aliveCount = 0;
    for(int i=0; i<playerCount; i++) {
        Player* p = &players[i];
        if(p->isDead) continue;
        aliveCount++;

        if(fabs(p->velocity.x) > 1.0f && p->isGrounded) p->runAnimTimer += dt * 15.0f;
        else p->runAnimTimer = 0;
        
        if(fabs(p->velocity.x) > 8.0f && p->isGrounded && (int)(GetTime()*10)%3==0) {
             SpawnParticle((Vector2){p->rect.x + p->rect.width/2, p->rect.y + p->rect.height}, (Vector2){-p->velocity.x*0.2f, -1}, GRAY, SMOKE, 5, 0.3f, 0);
        }

        if(p->gunRecoilAnim > 0) p->gunRecoilAnim -= dt * 5.0f;

        if(p->invincibilityTimer > 0) p->invincibilityTimer -= dt;
        if(p->shootCooldown > 0) p->shootCooldown -= dt;
        if(p->dodgeTimer > 0) p->dodgeTimer -= dt;
        if(p->parryTimer > 0) { p->parryTimer -= dt; if(p->parryTimer <= 0) p->isParrying = false; }
        if(p->comboTimer > 0) { p->comboTimer -= dt; if(p->comboTimer <= 0) p->comboCount = 0; }
        if(p->ultActive) { p->ultTimer -= dt; if(p->ultTimer <= 0) { p->ultActive = false; p->currentWeapon = COLT; p->ultCharge = 0; } }

        InputState in = {0};
        if(!p->isAI) {
            if(p->id == 1) { in.left = IsKeyDown(KEY_A) || IsKeyDown(KEY_Q); in.right = IsKeyDown(KEY_D); in.up = IsKeyDown(KEY_Z) || IsKeyDown(KEY_W); in.down = IsKeyDown(KEY_S); in.jumpPressed = IsKeyPressed(KEY_SPACE); in.shoot = IsKeyDown(KEY_E); in.shield = IsKeyDown(KEY_S) || IsKeyDown(KEY_LEFT_SHIFT); in.shieldPressed = IsKeyPressed(KEY_S); }
            else if(p->id == 2) { in.left = IsKeyDown(KEY_LEFT); in.right = IsKeyDown(KEY_RIGHT); in.up = IsKeyDown(KEY_UP); in.down = IsKeyDown(KEY_DOWN); in.jumpPressed = IsKeyPressed(KEY_UP); in.shoot = IsKeyDown(KEY_M); in.shield = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_RIGHT_SHIFT); in.shieldPressed = IsKeyPressed(KEY_DOWN); }
            else if(p->id == 3) { in.left = IsKeyDown(KEY_J); in.right = IsKeyDown(KEY_L); in.up = IsKeyDown(KEY_I); in.down = IsKeyDown(KEY_K); in.jumpPressed = IsKeyPressed(KEY_I); in.shoot = IsKeyDown(KEY_O); in.shield = IsKeyDown(KEY_K); in.shieldPressed = IsKeyPressed(KEY_K); }
            else if(p->id == 4) { in.left = IsKeyDown(KEY_F); in.right = IsKeyDown(KEY_H); in.up = IsKeyDown(KEY_T); in.down = IsKeyDown(KEY_G); in.jumpPressed = IsKeyPressed(KEY_T); in.shoot = IsKeyDown(KEY_Y); in.shield = IsKeyDown(KEY_G); in.shieldPressed = IsKeyPressed(KEY_G); }
        } else {
            if(players[0].rect.x > p->rect.x + 100) in.right = true; else if(players[0].rect.x < p->rect.x - 100) in.left = true; else in.shoot = (RandomInt(0, 100) < 2); if(RandomInt(0, 100) < 2) in.jumpPressed = true;
        }

        p->isShielding = in.shield && p->shieldHealth > 0 && !p->isDodging;
        if(p->isShielding) { p->shieldHealth -= 20.0f * dt; if(p->shieldHealth <= 0) { p->shieldBroken = true; p->hitStunFrames = 60; } } else if(p->shieldHealth < SHIELD_MAX) { p->shieldHealth += SHIELD_REGEN * 60.0f * dt; }
        if(in.shieldPressed && !p->shieldBroken && !p->isDodging) { p->isParrying = true; p->parryTimer = PARRY_WINDOW; }
        if(in.shield && (in.left || in.right) && p->dodgeTimer <= 0) { p->isDodging = true; p->dodgeTimer = DODGE_COOLDOWN; p->velocity.x = (in.right ? 1 : -1) * DODGE_SPEED; p->velocity.y = 0; }
        if(p->isDodging && p->dodgeTimer > DODGE_COOLDOWN - DODGE_DURATION) { p->rect.x += p->velocity.x * dt * 2.0f; continue; } else { p->isDodging = false; }

        if (p->onWall && p->velocity.y > 0) { p->velocity.y += GRAVITY * 0.3f; if(p->velocity.y > WALL_SLIDE_SPEED) p->velocity.y = WALL_SLIDE_SPEED; } else { p->velocity.y += GRAVITY; }
        if(in.left) { p->velocity.x -= 2.0f; p->facingRight = false; } if(in.right) { p->velocity.x += 2.0f; p->facingRight = true; }
        if(!in.left && !in.right) p->velocity.x *= (p->isGrounded ? FRICTION : AIR_FRICTION);
        
        if(p->velocity.x > p->moveSpeed) p->velocity.x = p->moveSpeed; 
        if(p->velocity.x < -p->moveSpeed) p->velocity.x = -p->moveSpeed;

        bool wasGrounded = p->isGrounded;
        p->isGrounded = false; p->onWall = false;
        for(int j=0; j<platformCount; j++) {
            if(CheckCollisionRecs(p->rect, platforms[j].rect)) {
                if(p->velocity.y > 0 && p->rect.y + p->rect.height < platforms[j].rect.y + 30) {
                    p->isGrounded = true; p->jumpCount = 0; p->velocity.y = 0; p->rect.y = platforms[j].rect.y - p->rect.height;
                    if(!wasGrounded) platforms[j].landShake = 3.0f; 
                } else if(!p->isGrounded) { p->onWall = true; p->jumpCount = 0; }
            }
        }

        if(in.jumpPressed) {
            if(p->isGrounded) { p->velocity.y = p->jumpForce; p->jumpCount = 1; } 
            else if (p->onWall) { p->velocity.y = WALL_JUMP_FORCE_Y; p->velocity.x = (p->facingRight ? -1 : 1) * WALL_JUMP_FORCE_X; p->facingRight = !p->facingRight; } 
            else if (p->jumpCount < MAX_JUMPS) { p->velocity.y = p->jumpForce * 0.9f; p->jumpCount++; SpawnParticle((Vector2){p->rect.x + p->rect.width/2, p->rect.y+p->rect.height}, (Vector2){0,0}, WHITE, SMOKE, 20, 0.5f, 0); }
        }

        p->rect.x += p->velocity.x * dt * 60.0f; p->rect.y += p->velocity.y * dt * 60.0f;

        if(p->ultCharge >= ULT_MAX && in.shoot && in.shield) { 
            p->ultActive = true; p->ultTimer = 10.0f; p->ultCharge = 0; 
            SpawnParticle((Vector2){p->rect.x, p->rect.y}, (Vector2){0,-5}, RED, CONFETTI, 60, 2.0f, 0); 
            if(p->type == JOE) p->currentWeapon = GATLING; 
            if(p->type == WILLIAM) p->currentWeapon = WINCHESTER; 
            if(p->type == JACK) p->currentWeapon = RPG; 
            if(p->type == AVERELL) p->currentWeapon = BARREL; 
        }
        if(in.shoot && p->shootCooldown <= 0 && !p->isShielding) { 
            WeaponData wd = GetWeaponData(p->currentWeapon); 
            SpawnProjectile((Vector2){p->rect.x + p->rect.width/2, p->rect.y + p->rect.height/2}, (Vector2){(p->facingRight?1:-1) * wd.speed, 0}, p->id, p->currentWeapon, wd.damage); 
            p->shootCooldown = wd.fireRate; 
            p->velocity.x -= (p->facingRight?1:-1) * wd.recoil * 2.0f;
            p->gunRecoilAnim = 0.2f; 
            SpawnParticle((Vector2){p->rect.x + (p->facingRight?p->rect.width:-20), p->rect.y + p->rect.height/2}, (Vector2){0,0}, WHITE, FLASH, 25, 0.1f, 0); 
            SpawnParticle((Vector2){p->rect.x + (p->facingRight?p->rect.width:-20), p->rect.y + p->rect.height/2}, (Vector2){0,0}, WHITE, SMOKE, 15, 0.3f, 0); 
        }
        
        if(p->rect.y > 1500 || p->rect.x < -1000 || p->rect.x > 4000) { 
            p->stocks--; p->damagePercent = 0; 
            CreateExplosion((Vector2){p->rect.x, p->rect.y}, p->color, 30);
            SpawnParticle((Vector2){p->rect.x, p->rect.y}, (Vector2){0,0}, RED, FLASH, 100, 0.5f, 0);
            screenShake = 10.0f; hitStop = 0.2f;
            if(p->stocks > 0) ResetPlayer(p, GetSafeSpawn(i, mapInfos[selectedMapId].width, mapInfos[selectedMapId].height)); 
            else { p->isDead = true; winnerId = (p->id == 1 ? 2 : 1); currentState = ENDING; } 
        }
    }
    
    UpdateGameCamera(aliveCount);

    for(int i=0; i<MAX_PROJECTILES; i++) {
        if(!projectiles[i].active) continue;
        
        WeaponData wd = GetWeaponData(projectiles[i].type);

        if(projectiles[i].type == DYNAMITE || projectiles[i].type == BARREL) {
             projectiles[i].vel.y += GRAVITY * dt * 60.0f; 
        } else {
             projectiles[i].vel.y = 0; 
        }

        if (projectiles[i].type == LASSO) {
            float dist = Vector2Distance(projectiles[i].startPos, projectiles[i].pos);
            if (dist > LASSO_MAX_DIST) {
                projectiles[i].vel.x *= -1; 
            }
            if (projectiles[i].vel.x < 0 && dist < 20) projectiles[i].active = false;
        }

        projectiles[i].pos.x += projectiles[i].vel.x * dt * 60.0f; 
        projectiles[i].pos.y += projectiles[i].vel.y * dt * 60.0f;
        projectiles[i].life -= dt; 

        if ((int)(GetTime()*60)%2 == 0 && projectiles[i].type != LASSO) {
             SpawnParticle(projectiles[i].pos, (Vector2){0,0}, wd.color, SMOKE, 5, 0.2f, 0);
        }

        if(projectiles[i].life <= 0) { 
            projectiles[i].active = false;
            if(projectiles[i].type == DYNAMITE) CreateExplosion(projectiles[i].pos, RED, 40); 
        }

        for(int j=0; j<playerCount; j++) {
            Player* vic = &players[j];
            if(vic->id == projectiles[i].ownerId && !projectiles[i].reflected) continue;
            if(vic->isDead) continue;
            if(CheckCollisionCircleRec(projectiles[i].pos, 15, vic->rect)) {
                if(vic->isDodging) { } 
                else if(vic->isParrying) { 
                    projectiles[i].vel.x *= -1.5f; projectiles[i].reflected = true; projectiles[i].ownerId = vic->id; vic->ultCharge += 20; 
                    parryFlashAlpha = 0.5f; screenShake = 10.0f;
                    SpawnParticle((Vector2){vic->rect.x + vic->rect.width/2, vic->rect.y + vic->rect.height/2}, (Vector2){0,0}, COL_CYAN_SOLID, SHOCKWAVE, 100, 0.5f, 0);
                }
                else if(vic->isShielding) { projectiles[i].active = false; vic->shieldHealth -= projectiles[i].damage * 2; SpawnParticle(projectiles[i].pos, (Vector2){0,0}, BLUE, CIRCLE, 10, 0.2f, 0); }
                else { 
                    projectiles[i].active = false; 
                    vic->damagePercent += projectiles[i].damage; 
                    vic->ultCharge += projectiles[i].damage; 
                    
                    float knockDir = (projectiles[i].vel.x > 0 ? 1 : -1);
                    float kbPower = wd.knockback;
                    
                    if(projectiles[i].type == LASSO) {
                        knockDir = (players[projectiles[i].ownerId-1].rect.x < vic->rect.x) ? -1 : 1; 
                        kbPower = 20.0f; 
                    }

                    float kbScale = (vic->damagePercent / 10.0f) * kbPower;
                    vic->velocity.x = knockDir * kbScale; 
                    vic->velocity.y = -5.0f - (kbScale * 0.5f);
                    
                    Player* attacker = &players[projectiles[i].ownerId-1]; 
                    attacker->comboCount++; attacker->comboTimer = COMBO_RESET_TIME; attacker->ultCharge += projectiles[i].damage * 0.5f; 
                    
                    CreateExplosion(projectiles[i].pos, wd.color, 15);
                    SpawnParticle(projectiles[i].pos, (Vector2){0,-20}, WHITE, IMPACT_CIRCLE, 30, 0.2f, 0);
                    
                    if(attacker->comboCount > 2) {
                        for(int c=0; c<3; c++) SpawnParticle((Vector2){vic->rect.x + vic->rect.width/2, vic->rect.y}, (Vector2){RandomFloat(-5,5), -5}, COL_GOLD, STAR, 15, 0.5f, 0);
                    }

                    hitStop = wd.hitstop; screenShake = 5.0f;
                }
            }
        }
        for(int d=0; d<MAX_DESTRUCTIBLES; d++) { if(destructibles[d].active && CheckCollisionCircleRec(projectiles[i].pos, 15, destructibles[d].rect)) { destructibles[d].health -= projectiles[i].damage; projectiles[i].active = false; if(destructibles[d].health <= 0) { destructibles[d].active = false; CreateExplosion((Vector2){destructibles[d].rect.x, destructibles[d].rect.y}, ORANGE, 20); } } }
    }
}

// ============================================================================
// --- MAIN ---
// ============================================================================

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BLAST BRAWL : DALTON EDITION");
    SetTargetFPS(TARGET_FPS);
    InitAudio();
    InitDecor();

    camera.zoom = 1.0f;
    miniMapCam.zoom = 0.15f;

    InitPlayer(&players[0], 1, JOE, false, 0);
    InitPlayer(&players[1], 2, WILLIAM, true, 1);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        float updateDt = dt * timeScale;

        if(transActive) UpdateTransition(dt, &currentState);
        
        if (hitStop > 0) {
            hitStop -= dt;
        } else {
            switch(currentState) {
                case MENU:
                    UpdateDecor(dt);
                    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) { menuSelection--; }
                    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) { menuSelection++; }
                    
                    if (menuSelection < 0) menuSelection = 2; 
                    if (menuSelection > 2) menuSelection = 0;
                    
                    if (IsKeyPressed(KEY_ENTER)) {
                        if (menuSelection == 0) { AddPlayers(2); StartTransition(MAP_SELECT); }
                        else if (menuSelection == 1) { AddPlayers(4); StartTransition(MAP_SELECT); } 
                        else if (menuSelection == 2) { StartTransition(OPTIONS); }
                    }
                    break;
                case MAP_SELECT:
                     LoadMap((MapType)selectedMapId);
                     if (IsKeyPressed(KEY_RIGHT)) selectedMapId = (selectedMapId + 1) % 5;
                     if (IsKeyPressed(KEY_LEFT)) selectedMapId = (selectedMapId - 1 < 0) ? 4 : selectedMapId - 1;
                     if (IsKeyPressed(KEY_ENTER)) StartTransition(CHAR_SELECT);
                     if (IsKeyPressed(KEY_BACKSPACE)) StartTransition(MENU);
                     break;
                case CHAR_SELECT:
                    charSelectTimer += dt;
                    UpdateDecor(dt);
                    for(int i=0; i<playerCount; i++) {
                        if(!players[i].isReady && !players[i].isAI) { 
                            bool change = false;
                            if(i==0) { 
                                if(IsKeyPressed(KEY_D)) { players[i].type = (DaltonType)(((int)players[i].type + 1) % 4); change=true; }
                                if(IsKeyPressed(KEY_Q)) { players[i].type = (DaltonType)(((int)players[i].type - 1 < 0 ? 3 : (int)players[i].type - 1)); change=true; }
                                if(IsKeyPressed(KEY_LEFT_SHIFT)) players[i].isReady = true;
                            } else if(i==1) { 
                                if(IsKeyPressed(KEY_RIGHT)) { players[i].type = (DaltonType)(((int)players[i].type + 1) % 4); change=true; }
                                if(IsKeyPressed(KEY_LEFT)) { players[i].type = (DaltonType)(((int)players[i].type - 1 < 0 ? 3 : (int)players[i].type - 1)); change=true; }
                                if(IsKeyPressed(KEY_RIGHT_SHIFT) || IsKeyPressed(KEY_KP_0)) players[i].isReady = true;
                            }
                            if(change) InitPlayer(&players[i], i+1, players[i].type, players[i].isAI, 0);
                        }
                    }
                    {
                        bool all = true;
                        for(int i=0; i<playerCount; i++) if(!players[i].isReady && !players[i].isAI) all = false;
                        if(all && IsKeyPressed(KEY_ENTER)) { ResetRound(); StartTearTransition(GAMEPLAY); }
                        if (IsKeyPressed(KEY_BACKSPACE)) StartTransition(MAP_SELECT);
                    }
                    break;
                case OPTIONS:
                    UpdateDecor(dt);
                    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) optionsSelection--;
                    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) optionsSelection++;
                    
                    if (optionsSelection < 0) optionsSelection = 2; 
                    if (optionsSelection > 2) optionsSelection = 0;
                    
                    if (IsKeyPressed(KEY_ENTER)) {
                        if (optionsSelection == 0) { audio.enabled = !audio.enabled; }
                        if (optionsSelection == 1) { fullscreen = !fullscreen; ToggleFullscreen(); }
                        if (optionsSelection == 2) { StartTransition(MENU); }
                    }
                    if (IsKeyPressed(KEY_BACKSPACE)) StartTransition(MENU);
                    break;
                case GAMEPLAY:
                    UpdateGameplay(dt, updateDt);
                    break;
                case ENDING:
                    if (IsKeyPressed(KEY_ENTER)) StartTransition(MENU);
                    break;
                default: break;
            }
        }

        // --- DRAWING ---
        BeginDrawing();
        ClearBackground(BLACK);
        
        if (screenShake > 0) {
            camera.offset.x += RandomFloat(-screenShake, screenShake);
            camera.offset.y += RandomFloat(-screenShake, screenShake);
            screenShake -= dt * 30.0f;
            if(screenShake < 0) screenShake = 0;
        }

        if (currentState == MENU) {
            DrawDecor(camera);
            DrawRectangleGradientV(0, 0, SCREEN_WIDTH, 200, Fade(BLACK, 0.8f), Fade(BLACK, 0.0f));
            DrawRectangleGradientV(0, SCREEN_HEIGHT-200, SCREEN_WIDTH, 200, Fade(BLACK, 0.0f), Fade(BLACK, 0.8f));
            
            float s = 1.0f + sinf(GetTime()*2.0f)*0.05f;
            Vector2 titlePos = { SCREEN_WIDTH/2, 150 };
            DrawTextPro(GetFontDefault(), "BLAST BRAWL", (Vector2){MeasureText("BLAST BRAWL", 80)*s/2, 40}, titlePos, 0, 80*s, 5, BLACK);
            DrawTextPro(GetFontDefault(), "BLAST BRAWL", (Vector2){MeasureText("BLAST BRAWL", 80)*s/2 - 5, 45}, titlePos, 0, 80*s, 5, COL_GOLD);
            DrawMenuButton("1v1 DUEL", 500, 350, 280, 60, menuSelection == 0);
            DrawMenuButton("4 PLAYERS CHAOS", 500, 430, 280, 60, menuSelection == 1);
            DrawMenuButton("OPTIONS", 500, 510, 280, 60, menuSelection == 2);
        }
        else if (currentState == MAP_SELECT) {
            DrawSmashBackground();
            Rectangle previewRect = (Rectangle){ 340, 150, 600, 360 }; 
            DrawRectangleRec(previewRect, BLACK);
            BeginMode2D(miniMapCam); 
                miniMapCam.target = (Vector2){mapInfos[selectedMapId].width/2, mapInfos[selectedMapId].height/2}; 
                miniMapCam.offset = (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2}; 
                DrawDecor(miniMapCam); 
                for(int i=0; i<platformCount; i++) DrawPlatformStylish(&platforms[i]);
            EndMode2D();
            DrawRectangleLinesEx(previewRect, 8, WHITE); 
            DrawText("SELECT MAP", 400, 80, 50, WHITE); 
            DrawText(mapInfos[selectedMapId].name, SCREEN_WIDTH/2 - MeasureText(mapInfos[selectedMapId].name, 40)/2, 550, 40, WHITE); 
            DrawText("< ARROWS >", SCREEN_WIDTH/2 - MeasureText("< ARROWS >", 20)/2, 620, 20, LIGHTGRAY);
        }
        else if (currentState == CHAR_SELECT) {
            DrawSmashBackground();
            DrawText("CHOOSE YOUR FIGHTER", SCREEN_WIDTH/2 - MeasureText("CHOOSE YOUR FIGHTER", 40)/2, 30, 40, WHITE);
            
            int tileW = 150; int tileH = 150;
            int startX = (SCREEN_WIDTH - (4 * (tileW + 20))) / 2;
            for(int i=0; i<4; i++) {
                bool active = false;
                for(int p=0; p<playerCount; p++) if(players[p].type == i) active = true;
                DrawCharacterTile((DaltonType)i, startX + i*(tileW+20), 100, tileW, tileH, active);
            }

            int dockW = SCREEN_WIDTH / playerCount;
            for(int i=0; i<playerCount; i++) {
                DrawPlayerDock(&players[i], i * dockW, 300, dockW, 420);
            }

            bool allReady = true;
            for(int i=0; i<playerCount; i++) if(!players[i].isReady && !players[i].isAI) allReady = false;
            if(allReady) {
                DrawRectangle(0, SCREEN_HEIGHT/2 - 50, SCREEN_WIDTH, 100, COL_READY_BAR);
                DrawText("READY TO FIGHT!", SCREEN_WIDTH/2 - MeasureText("READY TO FIGHT!", 60)/2, SCREEN_HEIGHT/2 - 30, 60, WHITE);
                if((int)(GetTime()*10)%2==0) DrawText("PRESS ENTER", SCREEN_WIDTH/2 - MeasureText("PRESS ENTER", 30)/2, SCREEN_HEIGHT/2 + 40, 30, YELLOW);
            }
        }
        else if (currentState == OPTIONS) {
             DrawDecor(camera); 
             DrawRectangle(300, 100, 680, 500, Fade(BLACK, 0.8f));
             DrawRectangleLinesEx((Rectangle){300,100,680,500}, 5, WHITE);
             DrawText("OPTIONS", 550, 130, 50, WHITE);
             DrawMenuButton(audio.enabled ? "AUDIO: ON" : "AUDIO: OFF", 400, 250, 480, 60, optionsSelection==0);
             DrawMenuButton(fullscreen ? "FULLSCREEN: ON" : "FULLSCREEN: OFF", 400, 330, 480, 60, optionsSelection==1);
             DrawMenuButton("BACK", 400, 450, 480, 60, optionsSelection==2);
        }
        else if (currentState == GAMEPLAY) {
            BeginMode2D(camera);
            DrawDecor(camera);
            for(int i=0; i<platformCount; i++) DrawPlatformStylish(&platforms[i]);
            for(int i=0; i<MAX_DESTRUCTIBLES; i++) if(destructibles[i].active) DrawRectangleRec(destructibles[i].rect, destructibles[i].color);
            for(int i=0; i<playerCount; i++) if(!players[i].isDead) DrawDaltonInGame(&players[i], players[i].rect.x + players[i].rect.width/2, players[i].rect.y, players[i].rect.height);
            for(int i=0; i<MAX_PROJECTILES; i++) {
                if(projectiles[i].active) {
                    Color c = GetWeaponData(projectiles[i].type).color;
                    if(projectiles[i].type == DYNAMITE) c = (int)(GetTime()*10)%2==0 ? RED : ORANGE; 
                    
                    if (projectiles[i].type == LASSO) {
                        DrawLineEx(projectiles[i].startPos, projectiles[i].pos, 4, ORANGE); 
                        DrawCircleLines(projectiles[i].pos.x, projectiles[i].pos.y, 10, ORANGE); 
                    }
                    else if (projectiles[i].type == BARREL) DrawRectanglePro((Rectangle){projectiles[i].pos.x, projectiles[i].pos.y, 30, 40}, (Vector2){15,20}, projectiles[i].angle, BROWN);
                    else if (projectiles[i].type == COLT) {
                         DrawPoly(projectiles[i].pos, 5, 10, GetTime()*10, c); 
                    }
                    else if (projectiles[i].type == WINCHESTER) {
                        DrawRectanglePro((Rectangle){projectiles[i].pos.x, projectiles[i].pos.y, 20, 6}, (Vector2){10,3}, 0, c);
                    }
                    else DrawCircleV(projectiles[i].pos, 6, c);
                }
            }
            for(int i=0; i<MAX_ITEMS; i++) {
                if(items[i].active) {
                    DrawItemVisual(&items[i]); 
                }
            }
            DrawParticles();
            if (luckyLukeTimer > LUCKY_LUKE_INTERVAL - 2.0f) DrawLuckyLukeEvent();
            EndMode2D();

            // HUD
            for(int i=0; i<playerCount; i++) {
                DrawHUD(&players[i], i);
            }
        }
        else if(currentState == ENDING) {
             DrawRectangle(0,0,SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8f));
             DrawText(TextFormat("PLAYER %d WINS!", winnerId), SCREEN_WIDTH/2 - MeasureText(TextFormat("PLAYER %d WINS!", winnerId), 60)/2, 300, 60, COL_GOLD);
             DrawText("PRESS ENTER", SCREEN_WIDTH/2 - MeasureText("PRESS ENTER", 30)/2, 450, 30, WHITE);
        }
        
        DrawTransition();
        EndDrawing();
    }
    
    CloseAudioDevice();
    return 0;
}