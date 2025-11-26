#include "player.h"
#include "../core/assets.h"
#include "../bonus/bonus.h"
#include <math.h>

void InitPlayer(Player* p, int id, DaltonType t, bool ai) {
    p->id = id; p->type = t; p->isAI = ai; p->stocks = 3; 
    p->color = (id == 1) ? COL_P1 : COL_P2;
    p->damagePercent = 0; 
    float h = BASE_HEIGHT * PLAYER_SCALE;
    if(t == JOE) h *= 0.85f; else if(t == AVERELL) h *= 1.15f;
    p->rect = (Rectangle){0,0, BASE_WIDTH*PLAYER_SCALE, h};
    p->moveSpeed = MOVE_SPEED_BASE; p->jumpForce = JUMP_FORCE;
    p->facingRight = true;
}

void ResetPlayer(Player* p, Vector2 pos) {
    p->rect.x = pos.x; p->rect.y = pos.y; p->velocity = (Vector2){0,0}; 
    p->damagePercent = 0; p->isDead = false;
    p->currentWeapon = COLT; p->invincibilityTimer = 2.0f;
    p->coyoteTimer = 0; p->jumpBufferTimer = 0;
}

void UpdatePlayerPhysics(Player* p, float dt, EnvPlatform platforms[], int platformCount) {
    InputState in = {0};
    if(!p->isAI) {
        if(p->id == 1) { 
            in.left = IsKeyDown(KEY_A); in.right = IsKeyDown(KEY_D); 
            in.jumpPressed = IsKeyPressed(KEY_SPACE); in.jumpReleased = IsKeyReleased(KEY_SPACE);
            in.shoot = IsKeyDown(KEY_E); 
        }
        if(p->id == 2) { 
            in.left = IsKeyDown(KEY_LEFT); in.right = IsKeyDown(KEY_RIGHT); 
            in.jumpPressed = IsKeyPressed(KEY_UP); in.jumpReleased = IsKeyReleased(KEY_UP);
            in.shoot = IsKeyDown(KEY_M); 
        }
    } else {
         if(GetTime() > 1.0f) { 
             // IA Simple (Random walk)
             if(GetRandomValue(0,100) < 5) in.right = !in.right;
             if(GetRandomValue(0,100) < 5) in.left = !in.left;
             if(GetRandomValue(0,100) < 2) in.jumpPressed = true;
             if(GetRandomValue(0,100) < 2) in.shoot = true;
         }
    }

    float accel = (p->isGrounded) ? 2.5f : 1.5f; 
    if(in.left) { p->velocity.x -= accel; p->facingRight = false; }
    if(in.right) { p->velocity.x += accel; p->facingRight = true; }
    
    float fric = (p->isGrounded) ? FRICTION : AIR_FRICTION;
    if(!in.left && !in.right) p->velocity.x *= fric;
    if(p->velocity.x > p->moveSpeed) p->velocity.x = p->moveSpeed; 
    if(p->velocity.x < -p->moveSpeed) p->velocity.x = -p->moveSpeed;
    
    if(p->isGrounded && fabs(p->velocity.x) > 4.0f && (int)(GetTime()*20)%4==0) {
        SpawnParticle((Vector2){p->rect.x + p->rect.width/2, p->rect.y + p->rect.height}, (Vector2){-p->velocity.x*0.2f, -1}, Fade(BROWN, 0.5f), DUST, 5, 0.3f);
    }

    p->velocity.y += GRAVITY;
    if(p->velocity.y > MAX_FALL_SPEED) p->velocity.y = MAX_FALL_SPEED;

    if (p->isGrounded) p->coyoteTimer = COYOTE_TIME;
    else p->coyoteTimer -= dt;

    if (in.jumpPressed) p->jumpBufferTimer = JUMP_BUFFER;
    else p->jumpBufferTimer -= dt;

    if (p->jumpBufferTimer > 0 && p->coyoteTimer > 0) {
        p->velocity.y = p->jumpForce;
        p->jumpBufferTimer = 0; p->coyoteTimer = 0; p->isGrounded = false;
        SpawnParticle((Vector2){p->rect.x + p->rect.width/2, p->rect.y + p->rect.height}, (Vector2){0, 0}, WHITE, DUST, 15, 0.4f);
    }

    if (in.jumpReleased && p->velocity.y < 0) {
        p->velocity.y *= JUMP_CUT_OFF;
    }

    p->rect.x += p->velocity.x * dt * 60.0f; 
    p->rect.y += p->velocity.y * dt * 60.0f;

    p->wasGrounded = p->isGrounded;
    p->isGrounded = false;
    
    for(int j=0; j<platformCount; j++) {
        if(CheckCollisionRecs(p->rect, platforms[j].rect)) {
            if (!platforms[j].canDropThrough) {
                if (p->velocity.y < 0 && p->rect.y > platforms[j].rect.y + platforms[j].rect.height - 10) {
                     p->velocity.y = 0; p->rect.y = platforms[j].rect.y + platforms[j].rect.height;
                }
                else if (p->velocity.y > 0) {
                    p->isGrounded = true; p->velocity.y = 0; p->rect.y = platforms[j].rect.y - p->rect.height;
                }
            } else {
                if (p->velocity.y > 0 && p->rect.y + p->rect.height < platforms[j].rect.y + 30) {
                    p->isGrounded = true; p->velocity.y = 0; p->rect.y = platforms[j].rect.y - p->rect.height;
                }
            }
        }
    }
    
    if (!p->wasGrounded && p->isGrounded) {
         SpawnParticle((Vector2){p->rect.x + p->rect.width/2, p->rect.y + p->rect.height}, (Vector2){-5, -1}, WHITE, DUST, 10, 0.2f);
         SpawnParticle((Vector2){p->rect.x + p->rect.width/2, p->rect.y + p->rect.height}, (Vector2){5, -1}, WHITE, DUST, 10, 0.2f);
    }

    if(p->shootCooldown > 0) p->shootCooldown -= dt;
    if(in.shoot && p->shootCooldown <= 0) { 
        WeaponData wd = GetWeaponData(p->currentWeapon); 
        SpawnProjectile(
            (Vector2){p->rect.x + p->rect.width/2, p->rect.y + p->rect.height/2}, 
            (Vector2){(p->facingRight?1:-1) * wd.speed, 0}, 
            p->id, p->currentWeapon, wd.damage
        );
        p->shootCooldown = wd.fireRate; 
        p->velocity.x -= (p->facingRight?1:-1) * wd.recoil; 
    }
}

void DrawPlayer(Player* p) {
    Texture2D texToDraw = texCowboyIdle;
    int frames = 4;
    if (p->shootCooldown > 0.05f) { texToDraw = texCowboyAttack; frames = 5; } 
    else if (!p->isGrounded) { texToDraw = texCowboyRun; frames = 6; } 
    else if (fabs(p->velocity.x) > 1.0f) { texToDraw = texCowboyRun; frames = 6; }
    
    if(texToDraw.id > 0) {
        float animSpeed = (texToDraw.id == texCowboyRun.id) ? 12.0f : 8.0f;
        int currentFrame = (int)(GetTime() * animSpeed) % frames;
        float frameW = (float)texToDraw.width / frames;
        Rectangle source = { currentFrame * frameW, 0, frameW, (float)texToDraw.height };
        if (!p->facingRight) source.width *= -1; 
        
        float scale = SPRITE_SCALE;
        if(p->type == JOE) scale *= 0.85f; else if(p->type == AVERELL) scale *= 1.15f;
        
        Vector2 pos = { p->rect.x + p->rect.width/2 - (fabs(frameW)*scale)/2, p->rect.y + p->rect.height - (texToDraw.height*scale) };
        DrawTexturePro(texToDraw, source, (Rectangle){pos.x, pos.y, fabs(frameW)*scale, texToDraw.height*scale}, (Vector2){0,0}, 0, p->color);
        
        if(p->currentWeapon != WEAPON_NONE && texGuns[p->currentWeapon].id > 0) {
            float gunX = p->rect.x + p->rect.width/2 + (p->facingRight ? 10 : -10);
            float gunY = p->rect.y + p->rect.height * 0.4f;
            Rectangle gunSrc = {0,0, (float)texGuns[p->currentWeapon].width, (float)texGuns[p->currentWeapon].height};
            if(!p->facingRight) gunSrc.height *= -1;
            DrawTexturePro(texGuns[p->currentWeapon], gunSrc, (Rectangle){gunX, gunY, gunSrc.width*1.5f, fabs(gunSrc.height)*1.5f}, (Vector2){0, fabs(gunSrc.height)/2}, 0, WHITE);
        }
    } else {
        DrawRectangleRec(p->rect, p->color); 
    }
    DrawTriangle((Vector2){p->rect.x, p->rect.y - 15}, (Vector2){p->rect.x+p->rect.width, p->rect.y - 15}, (Vector2){p->rect.x+p->rect.width/2, p->rect.y}, p->color);
}
