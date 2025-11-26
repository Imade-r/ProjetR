#include "game.h"
#include "map.h"
#include "assets.h"
#include "../players/player.h"
#include "../bonus/bonus.h"
#include <math.h>

Player players[MAX_PLAYERS];
int playerCount = 2;
int winnerId = 0;
float screenShake = 0;
float spawnTimer = 0;
Camera2D camera = {0};

extern GameState currentState; // Défini dans main.c

void InitGame() {
    InitAssets();
    InitMap();
    InitBonusSystem();
    camera.zoom = 1.0f;
    camera.offset = (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    
    InitPlayer(&players[0], 1, JOE, false);
    InitPlayer(&players[1], 2, AVERELL, true);
}

void UpdateCameraSmart() {
    Vector2 minPos = {10000, 10000}, maxPos = {-10000, -10000};
    int count = 0;
    for(int i=0; i<playerCount; i++) if(!players[i].isDead) {
        if(players[i].rect.x < minPos.x) minPos.x = players[i].rect.x;
        if(players[i].rect.y < minPos.y) minPos.y = players[i].rect.y;
        if(players[i].rect.x > maxPos.x) maxPos.x = players[i].rect.x;
        if(players[i].rect.y > maxPos.y) maxPos.y = players[i].rect.y;
        count++;
    }
    if(count == 0) return;
    
    Vector2 center = Vector2Scale(Vector2Add(minPos, maxPos), 0.5f);
    float width = maxPos.x - minPos.x + 600; float height = maxPos.y - minPos.y + 400;
    float zoom = fminf(SCREEN_WIDTH/width, SCREEN_HEIGHT/height);
    if(zoom < 0.5f) zoom = 0.5f; if(zoom > 1.3f) zoom = 1.3f;
    
    camera.target = Vector2Lerp(camera.target, center, 0.1f);
    camera.zoom = Lerp(camera.zoom, zoom, 0.05f);
    
    if(screenShake > 0) {
        camera.offset.x = (SCREEN_WIDTH/2) + GetRandomValue(-screenShake, screenShake);
        camera.offset.y = (SCREEN_HEIGHT/2) + GetRandomValue(-screenShake, screenShake);
        screenShake -= GetFrameTime() * 30.0f;
    } else {
        camera.offset = (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    }
}

void UpdateGame(float dt) {
    UpdateCameraSmart();
    UpdateParticles(dt);
    
    spawnTimer -= dt;
    if(spawnTimer <= 0) {
        SpawnItemRandom();
        spawnTimer = 10.0f; // Reset timer
    }
    
    UpdateItems(dt, platforms, platformCount, players, playerCount);
    
    for(int i=0; i<playerCount; i++) {
        if(!players[i].isDead) {
            UpdatePlayerPhysics(&players[i], dt, platforms, platformCount);
            if(players[i].rect.y > 1500) {
                players[i].stocks--;
                if(players[i].stocks > 0) ResetPlayer(&players[i], (Vector2){300 + i*200, 300});
                else { players[i].isDead = true; winnerId = (i == 0) ? 2 : 1; currentState = ENDING; }
            }
        }
    }
    
    UpdateProjectiles(dt, players, playerCount);
}

void DrawGame() {
    BeginMode2D(camera);
    DrawDecor(camera);
    DrawMap();
    DrawItems();
    for(int i=0; i<playerCount; i++) if(!players[i].isDead) DrawPlayer(&players[i]);
    DrawProjectiles();
    DrawParticles();
    EndMode2D();
    
    // Simple HUD
    for(int i=0; i<playerCount; i++) {
        DrawRectangle(20 + i*300, SCREEN_HEIGHT - 60, 200, 50, Fade(BLACK, 0.5f));
        DrawText(TextFormat("P%d: %d%%", i+1, (int)players[i].damagePercent), 30 + i*300, SCREEN_HEIGHT - 50, 30, players[i].damagePercent > 100 ? RED : WHITE);
    }
}
