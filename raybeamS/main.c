#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 1040

bool correct(int width, int height, Vector2 right[5][4], Vector2 shuf[5][4])
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (right[i][j].x != shuf[i][j].x || right[i][j].y != shuf[i][j].y)
            {
                return false;
            }
        }
    }
    return true;
}

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Sliding Puzzle");
    SetTargetFPS(60);

    // Load background image
    Image bgImage = LoadImage("bgg.png");
    Texture2D background = LoadTextureFromImage(bgImage);

    // Load puzzle image
    Image image = LoadImage("hum.png");
    Texture2D texture = LoadTextureFromImage(image);

    int grid_width = 5;
    int grid_height = 4;
    int padding = 6;

    Vector2 piece_src = {(float)image.width / grid_width, (float)image.height / grid_height};

    // Puzzle scale and piece size
    float puzzleScale = 0.8f;
    Vector2 puzzleSize = {
        WINDOW_WIDTH * puzzleScale,
        WINDOW_HEIGHT * puzzleScale};

    Vector2 piece_dest = {
        (puzzleSize.x - padding * (grid_width - 1)) / grid_width,
        (puzzleSize.y - padding * (grid_height - 1)) / grid_height};

    // Calculating top-left offset to center puzzle
    Vector2 puzzleOffset = {
        (WINDOW_WIDTH - puzzleSize.x) / 2.0f,
        (WINDOW_HEIGHT - puzzleSize.y) / 2.0f};

    Vector2 order[8][6];
    Vector2 shuffle[8][6];
    int total = grid_width * grid_height;
    Vector2 positions[48];

    for (int y = 0; y < grid_height; y++)
    {
        for (int x = 0; x < grid_width; x++)
        {
            order[x][y] = (Vector2){(float)x, (float)y};
        }
    }

    srand((unsigned int)time(NULL));

    int i = 0;
    for (int y = 0; y < grid_height; y++)
    {
        for (int x = 0; x < grid_width; x++)
        {
            positions[i++] = (Vector2){(float)x, (float)y};
        }
    }

    // Fisher-Yates shuffle
    for (int i = total - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        Vector2 temp = positions[i];
        positions[i] = positions[j];
        positions[j] = temp;
    }

    i = 0;
    for (int y = 0; y < grid_height; y++)
    {
        for (int x = 0; x < grid_width; x++)
        {
            shuffle[x][y] = positions[i++];
        }
    }

    Vector2 highlight = {(float)(grid_width / 2), (float)(grid_height / 2)};
    bool solved = false;
    bool wasSolved = false;
    float flashTimer = 0.0f;
    const float flashDuration = 1.0f;

    // --- TIMER FIX ---
    float elapsedTime = 0.0f;
    bool timerStopped = false;

    while (!WindowShouldClose())
    {
        // Update elapsed time only if timer not stopped
        if (!timerStopped)
            elapsedTime += GetFrameTime();

        bool Pieces_changed = false;

        // Input
        if (IsKeyPressed(KEY_LEFT))
        {
            if (highlight.x > 0)
            {
                if (IsKeyDown(KEY_LEFT_CONTROL))
                {
                    Vector2 temp = shuffle[(int)highlight.x][(int)highlight.y];
                    shuffle[(int)highlight.x][(int)highlight.y] = shuffle[(int)highlight.x - 1][(int)highlight.y];
                    shuffle[(int)highlight.x - 1][(int)highlight.y] = temp;
                    Pieces_changed = true;
                }
                else
                {
                    highlight.x--;
                    Pieces_changed = true;
                }
            }
        }
        else if (IsKeyPressed(KEY_RIGHT))
        {
            if (highlight.x < grid_width - 1)
            {
                if (IsKeyDown(KEY_LEFT_CONTROL))
                {
                    Vector2 temp = shuffle[(int)highlight.x][(int)highlight.y];
                    shuffle[(int)highlight.x][(int)highlight.y] = shuffle[(int)highlight.x + 1][(int)highlight.y];
                    shuffle[(int)highlight.x + 1][(int)highlight.y] = temp;
                    Pieces_changed = true;
                }
                else
                {
                    highlight.x++;
                    Pieces_changed = true;
                }
            }
        }
        else if (IsKeyPressed(KEY_UP))
        {
            if (highlight.y > 0)
            {
                if (IsKeyDown(KEY_LEFT_CONTROL))
                {
                    Vector2 temp = shuffle[(int)highlight.x][(int)highlight.y];
                    shuffle[(int)highlight.x][(int)highlight.y] = shuffle[(int)highlight.x][(int)highlight.y - 1];
                    shuffle[(int)highlight.x][(int)highlight.y - 1] = temp;
                    Pieces_changed = true;
                }
                else
                {
                    highlight.y--;
                    Pieces_changed = true;
                }
            }
        }
        else if (IsKeyPressed(KEY_DOWN))
        {
            if (highlight.y < grid_height - 1)
            {
                if (IsKeyDown(KEY_LEFT_CONTROL))
                {
                    Vector2 temp = shuffle[(int)highlight.x][(int)highlight.y];
                    shuffle[(int)highlight.x][(int)highlight.y] = shuffle[(int)highlight.x][(int)highlight.y + 1];
                    shuffle[(int)highlight.x][(int)highlight.y + 1] = temp;
                    Pieces_changed = true;
                }
                else
                {
                    highlight.y++;
                    Pieces_changed = true;
                }
            }
        }

        if (Pieces_changed)
        {
            if (correct(grid_width, grid_height, order, shuffle))
            {
                solved = true;
            }
            else
            {
                solved = false;
            }

            if (solved && !wasSolved)
            {
                flashTimer = flashDuration;
                wasSolved = true;
                padding = 0;
            }
            else if (!solved)
            {
                wasSolved = false;
                padding = 6;
            }
        }

        // Stop the timer once when solved
        if (solved && !timerStopped)
            timerStopped = true;

        // --- DRAWING ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(background, 0, 0, WHITE);

        int minutes = (int)(elapsedTime / 60);
        int seconds = (int)((int)elapsedTime % 60);

        DrawText(TextFormat("Time: %02d:%02d", minutes, seconds), 20, 20, 24, RED);

        if (solved)
        {
            DrawText("Puzzle Solved! Time taken:", 20, 60, 24, GREEN);
            DrawText(TextFormat("%02d:%02d", minutes, seconds), 20, 100, 24, GREEN);
        }

        int border = solved ? 0 : 4;

        if (!solved)
        {
            // Draw highlight
            DrawRectangle(
                puzzleOffset.x + highlight.x * (piece_dest.x + padding) - border,
                puzzleOffset.y + highlight.y * (piece_dest.y + padding) - border,
                piece_dest.x + border * 2,
                piece_dest.y + border * 2,
                RED);

            // Draw puzzle pieces with padding
            for (int y = 0; y < grid_height; y++)
            {
                for (int x = 0; x < grid_width; x++)
                {
                    Vector2 pieceIndex = shuffle[x][y];

                    Rectangle srcRect = {
                        pieceIndex.x * piece_src.x,
                        pieceIndex.y * piece_src.y,
                        piece_src.x,
                        piece_src.y};

                    Rectangle destRect = {
                        puzzleOffset.x + x * (piece_dest.x + padding),
                        puzzleOffset.y + y * (piece_dest.y + padding),
                        piece_dest.x,
                        piece_dest.y};

                    DrawTexturePro(texture, srcRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);
                }
            }
        }
        else
        {
            // Draw solved puzzle (no padding)
            for (int y = 0; y < grid_height; y++)
            {
                for (int x = 0; x < grid_width; x++)
                {
                    Vector2 pieceIndex = shuffle[x][y];

                    Rectangle srcRect = {
                        pieceIndex.x * piece_src.x,
                        pieceIndex.y * piece_src.y,
                        piece_src.x,
                        piece_src.y};

                    Rectangle destRect = {
                        puzzleOffset.x + x * piece_dest.x,
                        puzzleOffset.y + y * piece_dest.y,
                        piece_dest.x,
                        piece_dest.y};

                    DrawTexturePro(texture, srcRect, destRect, (Vector2){0, 0}, 0.0f, WHITE);
                }
            }

            // Flash/fade effect on solve
            if (flashTimer > 0.0f)
            {
                float alpha = flashTimer / flashDuration;
                DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){255, 255, 255, (unsigned char)(alpha * 255)});
                flashTimer -= GetFrameTime();
            }
        }

        EndDrawing();
    }

    UnloadTexture(texture);
    UnloadImage(image);
    UnloadTexture(background);
    UnloadImage(bgImage);

    CloseWindow();
    return 0;
}
