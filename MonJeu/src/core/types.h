#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include <stdbool.h>

// --- ENUMS ---
typedef enum { MENU, MAP_SELECT, GAMEPLAY, ENDING } GameState;
typedef enum { JOE, WILLIAM, JACK, AVERELL } DaltonType;
typedef enum { COLT, WINCHESTER, SHOTGUN, DYNAMITE, GATLING, AK47, M4A1, MINIGUN, RPG, LASSO, BARREL, WEAPON_NONE } WeaponType;
typedef enum { CIRCLE, SPARK, SMOKE, FLASH, EXPLOSION, DUST } ParticleType;

// --- STRUCTS ---
typedef struct {
    bool left, right, up, down, jumpPressed, jumpReleased, shoot;
} InputState;

typedef struct {
    char name[32]; int damage; float fireRate; int ammoMax; float recoil; float speed; float hitstop; Color color;
} WeaponData;

typedef struct {
    Vector2 position; Vector2 velocity; Color color; ParticleType type;
    float size; float life; float maxLife;
    bool active; 
} Particle;

typedef struct {
    Rectangle rect; Color color; bool canDropThrough; Vector2 startPos, endPos; bool moving;
    float landShake; 
} EnvPlatform;

typedef struct {
    Rectangle rect; float health; bool active; Color color;
} Destructible;

typedef struct {
    int id; Rectangle rect; Vector2 velocity; DaltonType type; bool isAI;
    float moveSpeed, jumpForce; int stocks; float damagePercent; 
    
    // Etats Physique
    bool isGrounded; bool wasGrounded; bool facingRight; bool isDead;
    
    // Timers Game Feel
    float coyoteTimer;      
    float jumpBufferTimer;  
    float shootCooldown; float invincibilityTimer;
    
    Color color; 
    WeaponType currentWeapon;
    InputState inputs; 
} Player;

typedef struct {
    Vector2 pos; Vector2 vel; int ownerId; WeaponType type; int damage; float life; bool active;
} Projectile;

typedef struct {
    Vector2 pos; WeaponType type; bool active; Rectangle rect; bool isFalling;
} GameItem;

#endif
