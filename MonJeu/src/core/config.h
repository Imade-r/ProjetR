#ifndef CONFIG_H
#define CONFIG_H

#include "raylib.h"

// --- CONSTANTES GLOBALES ---
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define TARGET_FPS 60

// --- ÉCHELLE ---
#define SPRITE_SCALE 2.0f      
#define PLAYER_SCALE 1.65f 
#define BASE_WIDTH 40.0f
#define BASE_HEIGHT 60.0f

// --- PHYSIQUE (Game Feel) ---
#define GRAVITY 0.90f           
#define MAX_FALL_SPEED 16.0f    
#define FRICTION 0.80f          
#define AIR_FRICTION 0.95f      
#define MOVE_SPEED_BASE 9.0f    
#define JUMP_FORCE -18.5f       
#define JUMP_CUT_OFF 0.5f       

// --- TIMINGS ---
#define COYOTE_TIME 0.1f        
#define JUMP_BUFFER 0.1f        

// --- LIMITES ---
#define MAX_PARTICLES 6000
#define MAX_PROJECTILES 300
#define MAX_ITEMS 20
#define MAX_PLATFORMS 50
#define MAX_DESTRUCTIBLES 20
#define MAX_PLAYERS 4

// --- COULEURS ---
static const Color COL_SKY_TOP = { 100, 149, 237, 255 };     
static const Color COL_SKY_BOT = { 255, 228, 196, 255 };     
static const Color COL_P1 = { 220, 20, 60, 255 };    
static const Color COL_P2 = { 30, 144, 255, 255 };   
static const Color COL_GOLD = { 255, 215, 0, 255 };

#endif
