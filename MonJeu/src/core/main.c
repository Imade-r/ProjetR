#include "raylib.h"
#include "config.h"
#include "game.h"
#include "assets.h"
#include "../ui/menu.h"

GameState currentState = MENU;

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BLAST BRAWL: MODULAR EDITION");
    SetTargetFPS(TARGET_FPS);
    InitAudioDevice();
    
    InitGame();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        
        switch (currentState) {
            case MENU:
                if (IsKeyPressed(KEY_ENTER)) currentState = GAMEPLAY;
                break;
            case GAMEPLAY:
                UpdateGame(dt);
                break;
            case ENDING:
                if (IsKeyPressed(KEY_ENTER)) {
                    // Reset Logic simple
                    InitGame();
                    currentState = GAMEPLAY;
                }
                break;
            default: break;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        
        if (currentState == MENU) DrawMenu(assetsLoaded);
        else if (currentState == GAMEPLAY) DrawGame();
        else if (currentState == ENDING) {
            DrawText(TextFormat("PLAYER %d WINS", winnerId), SCREEN_WIDTH/2 - 200, 300, 60, WHITE);
            DrawText("PRESS ENTER TO RESTART", SCREEN_WIDTH/2 - 150, 400, 30, GRAY);
        }
        
        EndDrawing();
    }
    
    UnloadAssets();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
