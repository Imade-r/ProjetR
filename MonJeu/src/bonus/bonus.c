#include "bonus.h"
#include "../core/assets.h"
#include <math.h>

Particle particles[MAX_PARTICLES];
Projectile projectiles[MAX_PROJECTILES];
GameItem items[MAX_ITEMS];

float RandomFloat(float min, float max) { return min + (float)GetRandomValue(0, 10000) / 10000.0f * (max - min); }

void InitBonusSystem() {
    for(int i=0; i<MAX_PARTICLES; i++) particles[i].active = false;
    for(int i=0; i<MAX_PROJECTILES; i++) projectiles[i].active = false;
    for(int i=0; i<MAX_ITEMS; i++) items[i].active = false;
}

WeaponData GetWeaponData(WeaponType type) {
    switch (type) {
        case COLT: return (WeaponData){ .name="COLT", .damage=6, .fireRate=0.30f, .ammoMax=6, .recoil=5.0f, .speed=40.0f, .hitstop=0.1f, .color=COL_GOLD };
        case WINCHESTER: return (WeaponData){ .name="WINCHESTER", .damage=12, .fireRate=0.8f, .ammoMax=8, .recoil=10.0f, .speed=50.0f, .hitstop=0.2f, .color=BROWN };
        case DYNAMITE: return (WeaponData){ .name="DYNAMITE", .damage=35, .fireRate=0.5f, .ammoMax=3, .recoil=5.0f, .speed=12.0f, .hitstop=0.4f, .color=RED };
        default: return (WeaponData){ .name="MAINS", .damage=0, .fireRate=0, .ammoMax=0, .recoil=0, .speed=0, .hitstop=0, .color=WHITE };
    }
}

// --- PARTICULES ---
void SpawnParticle(Vector2 pos, Vector2 vel, Color col, ParticleType type, float size, float life) {
    for(int i=0; i<MAX_PARTICLES; i++) {
        if(!particles[i].active) {
            particles[i] = (Particle){pos, vel, col, type, size, life, life, true};
            return;
        }
    }
}

void CreateExplosion(Vector2 pos, Color color, int intensity) {
    SpawnParticle(pos, (Vector2){0,0}, WHITE, EXPLOSION, (float)intensity * 2.0f, 0.2f);
    for(int i=0; i<intensity/2; i++) {
        float ang = RandomFloat(0, 360) * DEG2RAD; float spd = RandomFloat(5, 20);
        SpawnParticle(pos, (Vector2){cosf(ang)*spd, sinf(ang)*spd}, color, SPARK, RandomFloat(3,8), 0.4f);
    }
    SpawnParticle(pos, (Vector2){0,-10}, GRAY, SMOKE, 40, 0.6f);
}

void UpdateParticles(float dt) {
    for(int i=0; i<MAX_PARTICLES; i++) {
        if(!particles[i].active) continue;
        particles[i].position.x += particles[i].velocity.x * dt; 
        particles[i].position.y += particles[i].velocity.y * dt;
        particles[i].life -= dt; 
        if (particles[i].type == EXPLOSION || particles[i].type == FLASH) {
             particles[i].color.a = (unsigned char)(255 * (particles[i].life / particles[i].maxLife));
        } else if (particles[i].type == DUST) {
             particles[i].size += 10.0f * dt;
             particles[i].color.a = (unsigned char)(200 * (particles[i].life / particles[i].maxLife));
        }
        if (particles[i].life <= 0) particles[i].active = false;
    }
}

void DrawParticles() {
    for(int i=0; i<MAX_PARTICLES; i++) {
        if(!particles[i].active) continue;
        DrawCircleV(particles[i].position, particles[i].size, particles[i].color);
    }
}

// --- PROJECTILES ---
void SpawnProjectile(Vector2 pos, Vector2 vel, int ownerId, WeaponType type, int dmg) {
    for(int i=0; i<MAX_PROJECTILES; i++) {
        if(!projectiles[i].active) {
            projectiles[i] = (Projectile){pos, vel, ownerId, type, dmg, 2.0f, true};
            if(type == DYNAMITE) projectiles[i].life = 2.5f;
            break;
        }
    }
}

void UpdateProjectiles(float dt, Player players[], int playerCount) {
    for(int i=0; i<MAX_PROJECTILES; i++) {
        if(!projectiles[i].active) continue;
        projectiles[i].pos.x += projectiles[i].vel.x * dt * 60.0f; 
        projectiles[i].pos.y += projectiles[i].vel.y * dt * 60.0f;
        projectiles[i].life -= dt;
        if(projectiles[i].life <= 0) projectiles[i].active = false;
        
        for(int j=0; j<playerCount; j++) {
            if(players[j].id != projectiles[i].ownerId && !players[j].isDead && CheckCollisionCircleRec(projectiles[i].pos, 15, players[j].rect)) {
                players[j].damagePercent += projectiles[i].damage;
                players[j].velocity.x += (projectiles[i].vel.x > 0 ? 1 : -1) * 12.0f * (1 + players[j].damagePercent/100.0f);
                players[j].velocity.y = -8.0f - (players[j].damagePercent/20.0f);
                projectiles[i].active = false;
                CreateExplosion(projectiles[i].pos, RED, 10);
            }
        }
    }
}

void DrawProjectiles() {
    for(int i=0; i<MAX_PROJECTILES; i++) if(projectiles[i].active) DrawCircleV(projectiles[i].pos, 5, YELLOW);
}

// --- ITEMS ---
void SpawnItemRandom() {
    for(int i=0; i<MAX_ITEMS; i++) if(!items[i].active) { 
        items[i] = (GameItem){ {RandomFloat(200, 1500), -200}, (WeaponType)GetRandomValue(0, 8), true, (Rectangle){0,0,50,50}, true }; 
        break; 
    }
}

void UpdateItems(float dt, EnvPlatform platforms[], int platformCount, Player players[], int playerCount) {
    for(int i=0; i<MAX_ITEMS; i++) {
        if(!items[i].active) continue;
        if(items[i].isFalling) {
            items[i].pos.y += 300.0f * dt;
            for(int j=0; j<platformCount; j++) if(CheckCollisionCircleRec(items[i].pos, 25, platforms[j].rect)) { items[i].isFalling = false; items[i].pos.y = platforms[j].rect.y - 25; }
        }
        items[i].rect.x = items[i].pos.x - 25; items[i].rect.y = items[i].pos.y - 25;

        for(int p=0; p<playerCount; p++) if(!players[p].isDead && CheckCollisionCircleRec(items[i].pos, 30, players[p].rect)) {
            players[p].currentWeapon = items[i].type; items[i].active = false;
            SpawnParticle(items[i].pos, (Vector2){0,-2}, COL_GOLD, FLASH, 30, 0.5f);
        }
    }
}

void DrawItems() {
    for(int i=0; i<MAX_ITEMS; i++) if(items[i].active) {
        if(texObjCrate.id > 0) DrawTexturePro(texObjCrate, (Rectangle){0,0,texObjCrate.width,texObjCrate.height}, items[i].rect, (Vector2){0,0}, 0, WHITE);
        else DrawRectangleRec(items[i].rect, BLUE);
        DrawTexturePro(texGuns[items[i].type], (Rectangle){0,0,texGuns[items[i].type].width,texGuns[items[i].type].height}, (Rectangle){items[i].rect.x+10, items[i].rect.y-20, 30, 20}, (Vector2){0,0}, 0, WHITE);
    }
}
