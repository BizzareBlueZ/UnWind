// three_games_linked_no_physac.c
#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// =====================================================
// =============== Optional Front "PLAY" =================
// =====================================================
static void RunFrontScreen(void) {
    const int W = 960, H = 540;
    InitWindow(W, H, "Mini Collection");
    SetTargetFPS(60);

    Rectangle btn = { W / 2.0f - 140, H / 2.0f - 40, 280, 80 };
    bool start = false;

    while (!WindowShouldClose()) {
        Vector2 m = GetMousePosition();
        bool hover = CheckCollisionPointRec(m, btn);
        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) start = true;

        BeginDrawing();
        ClearBackground((Color) { 20, 22, 28, 255 });
        DrawText("Welcome!", W / 2 - MeasureText("Welcome!", 50) / 2, 110, 50, RAYWHITE);
        DrawText("Games will run one after another", W / 2 - MeasureText("Games will run one after another", 20) / 2, 170, 20, LIGHTGRAY);

        DrawRectangleRounded(btn, 0.2f, 12, hover ? (Color) { 70, 120, 255, 255 } : (Color) { 50, 90, 220, 255 });
        DrawText("PLAY", btn.x + btn.width / 2 - MeasureText("PLAY", 36) / 2, btn.y + 20, 36, WHITE);
        EndDrawing();

        if (start) break;
    }
    CloseWindow();
}

// =====================================================
// ============== Game 1: Basketball Toss ==============
// (your original logic; auto-advance on game over)
// =====================================================
static void RunBasketballGame(void) {

    Image courtImg = LoadImage("wow.png");
    int screenWidth = courtImg.width;
    int screenHeight = courtImg.height;
    UnloadImage(courtImg);
    InitWindow(screenWidth, screenHeight, "Basketball Toss");
    SetTargetFPS(60);

    Texture2D courtBg = LoadTexture("wow.png");
    Texture2D ballTex = LoadTexture("haha.png");
    Texture2D rimTex = LoadTexture("wiwi.png");

    float ballRadius = 100;
    Vector2 ballPosition = (Vector2){ screenWidth / 2.0f, screenHeight - 160.0f };
    Vector2 ballVelocity = (Vector2){ 0, 0 };
    bool ballActive = false;

    float rimScale = 1.05f;
    float rimDrawW = rimTex.width * rimScale;
    float rimDrawH = rimTex.height * rimScale;
    float rimX = (screenWidth / 2.0f) - rimDrawW / 2.0f;
    float rimY = 0.0f;
    float rimSpeed = 0.8f;
    int rimDirection = 1;

    bool isAiming = false;
    Vector2 aimStart = { 0 };
    Vector2 aimEnd = { 0 };

    int score = 0;
    bool scored = false;
    int lives = 3;
    bool gameOver = false;

    float gravity = 0.46f;
    float powerScaleX = 0.19f;
    float powerScaleY = 0.19f;
    float maxVel = 31.0f;

    float endDelay = 1.2f; // brief banner pause before auto-advance

    while (!WindowShouldClose()) {
        if (!gameOver) {
            // Rim movement
            rimX += rimSpeed * rimDirection;
            if (rimX <= 0 || rimX + rimDrawW >= screenWidth) rimDirection *= -1;

            // Mouse input
            Vector2 mouse = GetMousePosition();
            if (!ballActive && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && lives > 0) {
                float dist = Vector2Distance(mouse, ballPosition);
                if (dist <= ballRadius + 8) { isAiming = true; aimStart = ballPosition; aimEnd = mouse; }
            }

            // Aiming / release
            if (isAiming) {
                aimEnd = GetMousePosition();
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    Vector2 drag = (Vector2){ aimStart.x - aimEnd.x, aimStart.y - aimEnd.y };
                    float dragLength = sqrtf(drag.x * drag.x + drag.y * drag.y);
                    Vector2 tempVel = (Vector2){ drag.x * powerScaleX, drag.y * powerScaleY };
                    float speed = sqrtf(tempVel.x * tempVel.x + tempVel.y * tempVel.y);
                    if (speed > maxVel) { float k = maxVel / speed; tempVel.x *= k; tempVel.y *= k; }
                    if (dragLength > 16) { ballVelocity = tempVel; ballActive = true; scored = false; }
                    isAiming = false;
                }
            }

            // Ball physics
            if (ballActive) {
                ballVelocity.y += gravity;
                ballPosition.x += ballVelocity.x;
                ballPosition.y += ballVelocity.y;

                if (ballPosition.x < ballRadius) { ballPosition.x = ballRadius; ballVelocity.x = -ballVelocity.x * 0.25f; }
                else if (ballPosition.x > screenWidth - ballRadius) { ballPosition.x = screenWidth - ballRadius; ballVelocity.x = -ballVelocity.x * 0.25f; }
                if (ballPosition.y < ballRadius) { ballPosition.y = ballRadius; ballVelocity.y = 0.0f; }

                if (ballPosition.y > screenHeight - ballRadius) {
                    ballActive = false; ballVelocity = (Vector2){ 0,0 };
                    ballPosition = (Vector2){ screenWidth / 2.0f, screenHeight - 160.0f };
                    if (!scored) { lives--; if (lives <= 0) gameOver = true; }
                }

                // Net-only scoring window
                if (!scored && ballActive && ballVelocity.y > 0.0f) {
                    float netTop = rimY + rimDrawH * 0.82f;
                    float netBottom = netTop + 86.0f;
                    float netLeft = rimX + rimDrawW * 0.30f;
                    float netRight = rimX + rimDrawW * 0.70f;

                    float padX = ballRadius * 0.25f;
                    float padY = ballRadius * 0.15f;

                    bool insideX = (ballPosition.x > netLeft + padX) && (ballPosition.x < netRight - padX);
                    bool insideY = (ballPosition.y + padY > netTop) && (ballPosition.y - padY < netBottom);

                    if (insideX && insideY) { score++; scored = true; }
                }
            }
        }
        else {
            endDelay -= GetFrameTime();
            if (endDelay <= 0.0f) break; // auto-advance
        }

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(courtBg, 0, 0, WHITE);
        DrawTextureEx(rimTex, (Vector2) { rimX, rimY }, 0.0f, rimScale, WHITE);

        Rectangle src = { 0,0,(float)ballTex.width,(float)ballTex.height };
        Rectangle dst = { ballPosition.x, ballPosition.y, ballRadius * 2.0f, ballRadius * 2.0f };
        Vector2 origin = { ballRadius, ballRadius };
        DrawTexturePro(ballTex, src, dst, origin, 0.0f, WHITE);

        // Drag shooter + trajectory preview (visible)
        if (isAiming) {
            Vector2 drag = (Vector2){ aimStart.x - aimEnd.x, aimStart.y - aimEnd.y };
            float length = sqrtf(drag.x * drag.x + drag.y * drag.y);
            if (length > 14) {
                if (length > 340) {
                    drag.x *= (340.0f / length); drag.y *= (340.0f / length);
                    aimEnd.x = aimStart.x - drag.x; aimEnd.y = aimStart.y - drag.y;
                }
                Vector2 vel = (Vector2){ drag.x * powerScaleX, drag.y * powerScaleY };
                float spd = sqrtf(vel.x * vel.x + vel.y * vel.y);
                if (spd > maxVel) { float k = maxVel / spd; vel.x *= k; vel.y *= k; }
                for (int i = 1; i <= 35; i++) {
                    float t = i * 0.1f;
                    Vector2 next = {
                        ballPosition.x + vel.x * t,
                        ballPosition.y + vel.y * t + 0.5f * gravity * t * t
                    };
                    if (i % 2 == 0) DrawCircleV(next, 2, RED);
                }
                DrawLineEx(aimStart, aimEnd, 4, BLUE);
            }
        }

        DrawText(TextFormat("Score: %d", score), 24, 18, 38, WHITE);
        for (int i = 0; i < lives; i++) DrawText("❤", screenWidth - 100 + i * 30, 18, 36, RED);

        if (gameOver) DrawText("GAME OVER", screenWidth / 2 - 170, screenHeight / 2 - 60, 70, YELLOW);
        EndDrawing();
    }

    UnloadTexture(rimTex);
    UnloadTexture(ballTex);
    UnloadTexture(courtBg);
    CloseWindow();
}

// =====================================================
// ============= Game 2: Drop-down (no Physac) =========
// (converted to raylib + math.h; auto-advance at time up)
// =====================================================
#define DD_SCREEN_WIDTH   800
#define DD_SCREEN_HEIGHT  600
#define DD_MAX_CATS       10
#define DD_MAX_OBSTACLES  5
#define DD_MAX_POWERUPS   3
#define DD_CAT_SCALE      0.20f
#define DD_BASKET_SCALE   0.25f
#define DD_BASKET_SPEED   8.0f
#define DD_GRAVITY        1.0f     // px/frame^2 feel (scaled internally by dt)
#define DD_GAME_DURATION  60.0f

typedef enum { OBSTACLE_SPEEDUP = 0, OBSTACLE_SLOWDOWN, OBSTACLE_TIMEDEDUCT, OBSTACLE_SCOREDEDUCT } ObstacleType;
typedef enum { POWERUP_BIGBASKET = 0 } PowerUpType;

typedef struct {
    Texture2D texture;
    Vector2   pos;
    Vector2   vel;
    bool      active;
    int       scoreValue;
    float     w, h;     // scaled
} CatDD;

typedef struct {
    Texture2D texture;
    Vector2   pos;
    Vector2   vel;
    bool      active;
    ObstacleType type;
    float     w, h;
} ObstacleDD;

typedef struct {
    Texture2D texture;
    Vector2   pos;
    Vector2   vel;
    bool      active;
    PowerUpType type;
    float     w, h;
} PowerUpDD;

typedef struct {
    Texture2D texture;
    Vector2   pos;
    float     width, height;
    float     originalWidth, originalHeight;
} BasketDD;

static void RunDropDownGame_NoPhysac(void) {
    InitWindow(DD_SCREEN_WIDTH, DD_SCREEN_HEIGHT, "Falling objects with Obstacles (No Physac)");
    SetTargetFPS(60);
    srand((unsigned int)time(NULL));

    Texture2D background = LoadTexture("loll.jpg");
    Texture2D basketTex = LoadTexture("basket1.png");

    BasketDD basket = { 0 };
    basket.texture = basketTex;
    basket.width = basket.originalWidth = basketTex.width * DD_BASKET_SCALE;
    basket.height = basket.originalHeight = basketTex.height * DD_BASKET_SCALE;
    basket.pos = (Vector2){ DD_SCREEN_WIDTH / 2.0f, DD_SCREEN_HEIGHT - 100.0f };

    Texture2D catTextures[5] = {
        LoadTexture("ici.png"), LoadTexture("juice.png"), LoadTexture("ice2.png"),
        LoadTexture("lemonade.png"), LoadTexture("milk1.png")
    };
    int catScoreValues[5] = { 1,2,3,4,5 };

    Texture2D obstacleTextures[4] = {
        LoadTexture("obstacle1.png"), LoadTexture("obstacle2.png"),
        LoadTexture("obstacle3.png"), LoadTexture("obstacle4.png")
    };

    Texture2D powerupTexture = LoadTexture("tiny.png");

    CatDD cats[DD_MAX_CATS] = { 0 };
    ObstacleDD obstacles[DD_MAX_OBSTACLES] = { 0 };
    PowerUpDD powerups[DD_MAX_POWERUPS] = { 0 };

    int spawnTimer = 0, obstacleSpawnTimer = 90, powerupSpawnTimer = 300;

    int   score = 0;
    int   totalCollected = 0;
    float gameTime = DD_GAME_DURATION;
    float gameSpeed = 1.0f;
    bool  gameOver = false;
    float endDelay = 1.2f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (!gameOver) {
            gameTime -= dt * gameSpeed;
            if (gameTime <= 0.0f) { gameTime = 0.0f; gameOver = true; }
        }
        else {
            endDelay -= dt;
            if (endDelay <= 0.0f) break; // auto-advance
        }

        if (!gameOver) {
            // Basket movement
            if (IsKeyDown(KEY_LEFT))  basket.pos.x -= DD_BASKET_SPEED * gameSpeed;
            if (IsKeyDown(KEY_RIGHT)) basket.pos.x += DD_BASKET_SPEED * gameSpeed;
            float halfW = basket.width / 2.0f;
            if (basket.pos.x < halfW) basket.pos.x = halfW;
            if (basket.pos.x > DD_SCREEN_WIDTH - halfW) basket.pos.x = DD_SCREEN_WIDTH - halfW;

            // Spawns
            spawnTimer++;
            if (spawnTimer > (int)(120 / gameSpeed)) {
                for (int i = 0; i < DD_MAX_CATS; i++) if (!cats[i].active) {
                    int tIdx = GetRandomValue(0, 4);
                    cats[i].texture = catTextures[tIdx];
                    cats[i].scoreValue = catScoreValues[tIdx];
                    cats[i].w = cats[i].texture.width * DD_CAT_SCALE;
                    cats[i].h = cats[i].texture.height * DD_CAT_SCALE;
                    cats[i].pos = (Vector2){ (float)GetRandomValue(60, DD_SCREEN_WIDTH - 60), -cats[i].h };
                    cats[i].vel = (Vector2){ 0, 0 };
                    cats[i].active = true; break;
                }
                spawnTimer = 0;
            }

            obstacleSpawnTimer++;
            if (obstacleSpawnTimer > (int)(180 / gameSpeed)) {
                for (int i = 0; i < DD_MAX_OBSTACLES; i++) if (!obstacles[i].active) {
                    ObstacleType type = (ObstacleType)GetRandomValue(0, 3);
                    obstacles[i].texture = obstacleTextures[type];
                    obstacles[i].type = type;
                    obstacles[i].w = obstacles[i].texture.width * 0.15f;
                    obstacles[i].h = obstacles[i].texture.height * 0.15f;
                    obstacles[i].pos = (Vector2){ (float)GetRandomValue(60, DD_SCREEN_WIDTH - 60), -obstacles[i].h };
                    obstacles[i].vel = (Vector2){ 0, 0 };
                    obstacles[i].active = true; break;
                }
                obstacleSpawnTimer = 0;
            }

            powerupSpawnTimer++;
            if (powerupSpawnTimer > (int)(300 / gameSpeed)) {
                for (int i = 0; i < DD_MAX_POWERUPS; i++) if (!powerups[i].active) {
                    powerups[i].texture = powerupTexture;
                    powerups[i].type = POWERUP_BIGBASKET;
                    powerups[i].w = powerups[i].texture.width * 0.15f;
                    powerups[i].h = powerups[i].texture.height * 0.15f;
                    powerups[i].pos = (Vector2){ (float)GetRandomValue(60, DD_SCREEN_WIDTH - 60), -powerups[i].h };
                    powerups[i].vel = (Vector2){ 0, 0 };
                    powerups[i].active = true; break;
                }
                powerupSpawnTimer = 0;
            }

            // Physics step (Euler)
            const float g = DD_GRAVITY * 60.0f; // match a Physac-like feel at 60 FPS
            for (int i = 0; i < DD_MAX_CATS; i++) if (cats[i].active) {
                cats[i].vel.y += g * dt;
                cats[i].pos.y += cats[i].vel.y * dt;
                if (cats[i].pos.y >= DD_SCREEN_HEIGHT - 100) cats[i].active = false;
            }
            for (int i = 0; i < DD_MAX_OBSTACLES; i++) if (obstacles[i].active) {
                obstacles[i].vel.y += g * dt;
                obstacles[i].pos.y += obstacles[i].vel.y * dt;
                if (obstacles[i].pos.y >= DD_SCREEN_HEIGHT - 100) obstacles[i].active = false;
            }
            for (int i = 0; i < DD_MAX_POWERUPS; i++) if (powerups[i].active) {
                powerups[i].vel.y += g * dt;
                powerups[i].pos.y += powerups[i].vel.y * dt;
                if (powerups[i].pos.y >= DD_SCREEN_HEIGHT - 100) powerups[i].active = false;
            }

            // Collisions vs basket (AABB)
            Rectangle basketRec = (Rectangle){
                basket.pos.x - basket.width / 2.0f,
                basket.pos.y - basket.height / 2.0f,
                basket.width, basket.height
            };

            for (int i = 0; i < DD_MAX_CATS; i++) if (cats[i].active) {
                Rectangle r = (Rectangle){
                    cats[i].pos.x - cats[i].w / 2.0f,
                    cats[i].pos.y - cats[i].h / 2.0f,
                    cats[i].w, cats[i].h
                };
                if (CheckCollisionRecs(r, basketRec)) {
                    score += cats[i].scoreValue; totalCollected++;
                    cats[i].active = false;
                }
            }

            for (int i = 0; i < DD_MAX_OBSTACLES; i++) if (obstacles[i].active) {
                Rectangle r = (Rectangle){
                    obstacles[i].pos.x - obstacles[i].w / 2.0f,
                    obstacles[i].pos.y - obstacles[i].h / 2.0f,
                    obstacles[i].w, obstacles[i].h
                };
                if (CheckCollisionRecs(r, basketRec)) {
                    obstacles[i].active = false;
                    switch (obstacles[i].type) {
                    case OBSTACLE_SPEEDUP:    gameSpeed *= 1.5f; break;
                    case OBSTACLE_SLOWDOWN:   gameSpeed *= 0.5f; break;
                    case OBSTACLE_TIMEDEDUCT: gameTime = (gameTime > 5.0f) ? (gameTime - 5.0f) : 0.0f; break;
                    case OBSTACLE_SCOREDEDUCT: score = (score > 5) ? (score - 5) : 0; break;
                    }
                }
            }

            for (int i = 0; i < DD_MAX_POWERUPS; i++) if (powerups[i].active) {
                Rectangle r = (Rectangle){
                    powerups[i].pos.x - powerups[i].w / 2.0f,
                    powerups[i].pos.y - powerups[i].h / 2.0f,
                    powerups[i].w, powerups[i].h
                };
                if (CheckCollisionRecs(r, basketRec)) {
                    powerups[i].active = false;
                    if (powerups[i].type == POWERUP_BIGBASKET) {
                        basket.width *= 1.1f;
                        basket.height *= 1.1f;
                    }
                }
            }
        }

        // Draw
        BeginDrawing();
        DrawTexture(background, 0, 0, WHITE);
        DrawRectangle(0, DD_SCREEN_HEIGHT - 100, DD_SCREEN_WIDTH, 100, (Color) { 200, 200, 200, 200 });

        // basket
        DrawTexturePro(
            basket.texture,
            (Rectangle) {
            0, 0, (float)basket.texture.width, (float)basket.texture.height
        },
            (Rectangle) {
            basket.pos.x - basket.width / 2.0f, basket.pos.y - basket.height / 2.0f, basket.width, basket.height
        },
            (Vector2) {
            0, 0
        }, 0, WHITE
        );

        // cats
        for (int i = 0; i < DD_MAX_CATS; i++) if (cats[i].active) {
            DrawTextureEx(cats[i].texture,
                (Vector2) {
                cats[i].pos.x - cats[i].w / 2.0f, cats[i].pos.y - cats[i].h / 2.0f
            },
                0, DD_CAT_SCALE, WHITE);
        }

        // obstacles
        for (int i = 0; i < DD_MAX_OBSTACLES; i++) if (obstacles[i].active) {
            float s = 0.15f;
            DrawTextureEx(obstacles[i].texture,
                (Vector2) {
                obstacles[i].pos.x - (obstacles[i].texture.width * s) / 2.0f,
                    obstacles[i].pos.y - (obstacles[i].texture.height * s) / 2.0f
            },
                0, s, WHITE);
        }

        // powerups
        for (int i = 0; i < DD_MAX_POWERUPS; i++) if (powerups[i].active) {
            float s = 0.15f;
            DrawTextureEx(powerups[i].texture,
                (Vector2) {
                powerups[i].pos.x - (powerups[i].texture.width * s) / 2.0f,
                    powerups[i].pos.y - (powerups[i].texture.height * s) / 2.0f
            },
                0, s, WHITE);
        }

        // HUD
        DrawText(TextFormat("Score: %d", score), 10, 10, 20, DARKGRAY);
        DrawText(TextFormat("Collected: %d", totalCollected), 10, 40, 20, DARKGRAY);
        DrawText(TextFormat("Time: %.1f", gameTime), 10, 70, 20, DARKGRAY);
        DrawText(TextFormat("Speed: x%.1f", gameSpeed), 10, 100, 20, DARKGRAY);

        if (gameOver) {
            DrawRectangle(0, 0, DD_SCREEN_WIDTH, DD_SCREEN_HEIGHT, (Color) { 0, 0, 0, 180 });
            DrawText("GAME OVER", DD_SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 40) / 2, DD_SCREEN_HEIGHT / 2 - 40, 40, WHITE);
        }

        EndDrawing();
    }

    // Cleanup
    for (int i = 0; i < 5; i++) UnloadTexture(catTextures[i]);
    for (int i = 0; i < 4; i++) UnloadTexture(obstacleTextures[i]);
    UnloadTexture(powerupTexture);
    UnloadTexture(basketTex);
    UnloadTexture(background);
    CloseWindow();
}

// =====================================================
// =============== Game 3: Sliding Puzzle ==============
// (your original logic; auto-exit after solve)
// =====================================================
#define PUZ_WINDOW_WIDTH  1300
#define PUZ_WINDOW_HEIGHT 1040

typedef struct { const char* path; float weight; } PuzzlePic;

static int PickWeightedIndexFloat(const float* weights, int n) {
    float total = 0.0f; for (int i = 0; i < n; i++) total += weights[i];
    if (total <= 0.0f) return 0;
    float r = (float)GetRandomValue(0, 1000000) / 1000000.0f * total;
    float a = 0.0f; for (int i = 0; i < n; i++) { a += weights[i]; if (r <= a) return i; }
    return n - 1;
}

static bool correct(int width, int height, Vector2 right[5][4], Vector2 shuf[5][4]) {
    for (int i = 0; i < width; i++)
        for (int j = 0; j < height; j++)
            if (right[i][j].x != shuf[i][j].x || right[i][j].y != shuf[i][j].y) return false;
    return true;
}

static void RunPuzzleGame(void)
{
    InitWindow(PUZ_WINDOW_WIDTH, PUZ_WINDOW_HEIGHT, "Sliding Puzzle");
    SetTargetFPS(60);
    srand((unsigned int)time(NULL));
    GetRandomValue(0, 1000);

    Image   bgImage = LoadImage("bgg.png");
    Texture2D background = { 0 };
    if (bgImage.data) background = LoadTextureFromImage(bgImage);

    PuzzlePic candidates[] = {
        { "2.jpg", 1.0f }, { "3.jpg", 1.0f }, { "4.jpg", 1.0f }, { "5.jpg", 1.0f }, { "hum.png", 1.0f }
    };
    const int allCount = (int)(sizeof(candidates) / sizeof(candidates[0]));
#define MAX_IMAGES 5
    const char* usablePaths[MAX_IMAGES]; float usableWeights[MAX_IMAGES]; int usableCount = 0;
    for (int i = 0; i < allCount; i++) {
        if (FileExists(candidates[i].path) && candidates[i].weight > 0.0f) {
            usablePaths[usableCount] = candidates[i].path;
            usableWeights[usableCount] = candidates[i].weight;
            usableCount++;
        }
    }
    if (usableCount == 0) {
        BeginDrawing(); ClearBackground(RAYWHITE);
        DrawText("ERROR: No usable puzzle images found.", 40, 40, 28, RED);
        EndDrawing(); while (!WindowShouldClose()) {}
        if (background.id) UnloadTexture(background); if (bgImage.data) UnloadImage(bgImage);
        CloseWindow(); return;
    }

    int pickIdx = PickWeightedIndexFloat(usableWeights, usableCount);
    const char* chosenPath = usablePaths[pickIdx];

    Image image = LoadImage(chosenPath);
    if (!image.data) {
        BeginDrawing(); ClearBackground(RAYWHITE);
        DrawText("ERROR: Failed to load chosen puzzle image.", 40, 40, 28, RED);
        DrawText(TextFormat("Tried: %s", chosenPath), 40, 80, 18, DARKGRAY);
        EndDrawing(); while (!WindowShouldClose()) {}
        if (background.id) UnloadTexture(background); if (bgImage.data) UnloadImage(bgImage);
        CloseWindow(); return;
    }
    Texture2D texture = LoadTextureFromImage(image);

    int grid_width = 5, grid_height = 4, padding = 6;
    Vector2 piece_src = { (float)image.width / grid_width, (float)image.height / grid_height };

    float puzzleScale = 0.8f;
    Vector2 puzzleSize = { PUZ_WINDOW_WIDTH * puzzleScale, PUZ_WINDOW_HEIGHT * puzzleScale };
    Vector2 piece_dest = {
        (puzzleSize.x - padding * (grid_width - 1)) / grid_width,
        (puzzleSize.y - padding * (grid_height - 1)) / grid_height
    };
    Vector2 puzzleOffset = {
        (PUZ_WINDOW_WIDTH - puzzleSize.x) / 2.0f,
        (PUZ_WINDOW_HEIGHT - puzzleSize.y) / 2.0f
    };

    Vector2 order[8][6], shuffle[8][6], positions[48]; int total = grid_width * grid_height;
    for (int y = 0; y < grid_height; y++) for (int x = 0; x < grid_width; x++) order[x][y] = (Vector2){ (float)x,(float)y };
    int idx = 0; for (int y = 0; y < grid_height; y++) for (int x = 0; x < grid_width; x++) positions[idx++] = (Vector2){ (float)x,(float)y };
    for (int k = total - 1; k > 0; k--) { int j = rand() % (k + 1); Vector2 t = positions[k]; positions[k] = positions[j]; positions[j] = t; }
    idx = 0; for (int y = 0; y < grid_height; y++) for (int x = 0; x < grid_width; x++) shuffle[x][y] = positions[idx++];

    Vector2 highlight = { (float)(grid_width / 2), (float)(grid_height / 2) };
    bool solved = false, wasSolved = false; float flashTimer = 0.0f; const float flashDuration = 1.0f;
    float elapsedTime = 0.0f; bool timerStopped = false;
    float solvedExitDelay = 1.2f; // brief pause then auto-close

    while (!WindowShouldClose()) {
        if (!timerStopped) elapsedTime += GetFrameTime();
        bool Pieces_changed = false;

        if (IsKeyPressed(KEY_LEFT)) {
            if (highlight.x > 0) {
                if (IsKeyDown(KEY_LEFT_CONTROL)) {
                    Vector2 t = shuffle[(int)highlight.x][(int)highlight.y];
                    shuffle[(int)highlight.x][(int)highlight.y] = shuffle[(int)highlight.x - 1][(int)highlight.y];
                    shuffle[(int)highlight.x - 1][(int)highlight.y] = t;
                }
                else { highlight.x--; }
                Pieces_changed = true;
            }
        }
        else if (IsKeyPressed(KEY_RIGHT)) {
            if (highlight.x < grid_width - 1) {
                if (IsKeyDown(KEY_LEFT_CONTROL)) {
                    Vector2 t = shuffle[(int)highlight.x][(int)highlight.y];
                    shuffle[(int)highlight.x][(int)highlight.y] = shuffle[(int)highlight.x + 1][(int)highlight.y];
                    shuffle[(int)highlight.x + 1][(int)highlight.y] = t;
                }
                else { highlight.x++; }
                Pieces_changed = true;
            }
        }
        else if (IsKeyPressed(KEY_UP)) {
            if (highlight.y > 0) {
                if (IsKeyDown(KEY_LEFT_CONTROL)) {
                    Vector2 t = shuffle[(int)highlight.x][(int)highlight.y];
                    shuffle[(int)highlight.x][(int)highlight.y] = shuffle[(int)highlight.x][(int)highlight.y - 1];
                    shuffle[(int)highlight.x][(int)highlight.y - 1] = t;
                }
                else { highlight.y--; }
                Pieces_changed = true;
            }
        }
        else if (IsKeyPressed(KEY_DOWN)) {
            if (highlight.y < grid_height - 1) {
                if (IsKeyDown(KEY_LEFT_CONTROL)) {
                    Vector2 t = shuffle[(int)highlight.x][(int)highlight.y];
                    shuffle[(int)highlight.x][(int)highlight.y] = shuffle[(int)highlight.x][(int)highlight.y + 1];
                    shuffle[(int)highlight.x][(int)highlight.y + 1] = t;
                }
                else { highlight.y++; }
                Pieces_changed = true;
            }
        }

        if (Pieces_changed) {
            if (correct(grid_width, grid_height, order, shuffle)) { solved = true; }
            else { solved = false; }

            if (solved && !wasSolved) { flashTimer = flashDuration; wasSolved = true; padding = 0; timerStopped = true; }
            else if (!solved) { wasSolved = false; padding = 6; }
        }

        if (solved) {
            solvedExitDelay -= GetFrameTime();
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (background.id) DrawTexture(background, 0, 0, WHITE);

        int minutes = (int)(elapsedTime / 60);
        int seconds = (int)((int)elapsedTime % 60);
        DrawText(TextFormat("Time: %02d:%02d", minutes, seconds), 20, 20, 24, RED);
        DrawText(TextFormat("Chosen: %s", chosenPath), 20, 60, 18, DARKGRAY);

        int border = solved ? 0 : 4;

        if (!solved) {
            DrawRectangle(
                (int)(puzzleOffset.x + highlight.x * (piece_dest.x + padding) - border),
                (int)(puzzleOffset.y + highlight.y * (piece_dest.y + padding) - border),
                (int)(piece_dest.x + border * 2),
                (int)(piece_dest.y + border * 2),
                RED);

            for (int y = 0; y < grid_height; y++) for (int x = 0; x < grid_width; x++) {
                Vector2 pieceIndex = shuffle[x][y];
                Rectangle srcRect = { pieceIndex.x * piece_src.x, pieceIndex.y * piece_src.y, piece_src.x, piece_src.y };
                Rectangle destRect = { puzzleOffset.x + x * (piece_dest.x + padding),
                                       puzzleOffset.y + y * (piece_dest.y + padding),
                                       piece_dest.x, piece_dest.y };
                DrawTexturePro(texture, srcRect, destRect, (Vector2) { 0, 0 }, 0.0f, WHITE);
            }
        }
        else {
            for (int y = 0; y < grid_height; y++) for (int x = 0; x < grid_width; x++) {
                Vector2 pieceIndex = shuffle[x][y];
                Rectangle srcRect = { pieceIndex.x * piece_src.x, pieceIndex.y * piece_src.y, piece_src.x, piece_src.y };
                Rectangle destRect = { puzzleOffset.x + x * piece_dest.x,
                                       puzzleOffset.y + y * piece_dest.y,
                                       piece_dest.x, piece_dest.y };
                DrawTexturePro(texture, srcRect, destRect, (Vector2) { 0, 0 }, 0.0f, WHITE);
            }
            if (flashTimer > 0.0f) {
                float alpha = flashTimer / flashDuration;
                DrawRectangle(0, 0, PUZ_WINDOW_WIDTH, PUZ_WINDOW_HEIGHT, (Color) { 255, 255, 255, (unsigned char)(alpha * 255) });
                flashTimer -= GetFrameTime();
            }
        }

        EndDrawing();

        if (solved && solvedExitDelay <= 0.0f) break; // auto-advance after showing solved
    }

    if (texture.id) UnloadTexture(texture);
    if (image.data) UnloadImage(image);
    if (background.id) UnloadTexture(background);
    if (bgImage.data) UnloadImage(bgImage);
    CloseWindow();
}

// =====================================================
// ========================= MAIN ======================
// =====================================================
int main(void) {
    RunFrontScreen();        // (Optional) PLAY button
    RunBasketballGame();     // 1) Basketball
    RunDropDownGame_NoPhysac(); // 2) Drop-down (converted; no physac)
    RunPuzzleGame();         // 3) Sliding Puzzle
    return 0;
}
