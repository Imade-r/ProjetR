#ifndef BONUS_H
#define BONUS_H

#include "../core/config.h"
#include "../core/types.h"

// Tableaux globaux pour les entités "volatiles"
extern Particle particles[MAX_PARTICLES];
extern Projectile projectiles[MAX_PROJECTILES];
extern GameItem items[MAX_ITEMS];

void InitBonusSystem();
void SpawnParticle(Vector2 pos, Vector2 vel, Color col, ParticleType type, float size, float life);
void CreateExplosion(Vector2 pos, Color color, int intensity);
void UpdateParticles(float dt);
void DrawParticles();

void SpawnProjectile(Vector2 pos, Vector2 vel, int ownerId, WeaponType type, int dmg);
void UpdateProjectiles(float dt, Player players[], int playerCount);
void DrawProjectiles();

void SpawnItemRandom();
void UpdateItems(float dt, EnvPlatform platforms[], int platformCount, Player players[], int playerCount);
void DrawItems();

WeaponData GetWeaponData(WeaponType type);

#endif
