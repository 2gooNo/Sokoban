#include <raylib.h>
#include <cstring>
#include <cstdlib>

enum Tile
{
    WALL,
    FLOOR,
    TARGET,
    BOX,
    BOX_ON_TARGET,
    PLAYER,
    PLAYER_ON_TARGET
};

enum GameState
{
    STATE_DIFFICULTY,
    STATE_PLAYING,
    STATE_WIN,
    STATE_ALL_COMPLETE
};

const int TILE_SIZE = 64;
const int SCREEN_W = 800;
const int SCREEN_H = 600;
const int MAX_MAP_W = 14;
const int MAX_MAP_H = 12;
const int NUM_DIFFICULTIES = 3;
const int LEVELS_PER_DIFFICULTY = 3;

struct LevelDef
{
    int width;
    int height;
    Tile data[MAX_MAP_H][MAX_MAP_W];
};

const int TILE_SIZE_SMALL = 48; // For larger maps

// Game state
GameState gameState = STATE_DIFFICULTY;
int selectedDifficulty = 0;
int selectedLevel = 0;
int mapW = 0, mapH = 0;
Tile level[MAX_MAP_H][MAX_MAP_W];
bool shouldQuit = false;
bool showSolution = false;
int solutionStep = 0;
float solutionTimer = 0.0f;
const float SOLUTION_STEP_DELAY = 0.5f;

// Textures
Texture2D texWall, texFloor, texTarget;
Texture2D texBox, texBoxTarget;
Texture2D texPlayer, texPlayerTarget;

// All levels
LevelDef allLevels[NUM_DIFFICULTIES][LEVELS_PER_DIFFICULTY];
const char *difficultyNames[] = {"EASY", "MEDIUM", "HARD"};
const Color DARK_BG = {30, 30, 40, 255};

// Solutions for all levels (U=up, D=down, L=left, R=right)
const char *solutions[NUM_DIFFICULTIES][LEVELS_PER_DIFFICULTY] = {
    {"RRRR", "LURRLLULURRRDRU", "LUURRRUURDD"},
    {"URURDLDLLLURRDRULLUULURRRR", "LLLLUUURRRUURDDDDLDRLLUURULLLULDD", "UULUUULURDDDDDLDRRRRRUDRUULLRUULURRURDDDD"},
    {"URRRUULULUDDLLULDDUUUURRR", "UURRRDRDLLLLLUUUUUDRRRURRURDDDD", "URURULLUUUULURDRRRURRDRURDDLDDDD"}};

void InitLevels()
{
    // ========== EASY LEVELS ==========

    // Easy 1 (8x6, 1 box) - L-shaped push
    allLevels[0][0] = {8, 6, {{WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}, {WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, FLOOR, WALL}, {WALL, PLAYER, BOX, FLOOR, FLOOR, FLOOR, TARGET, WALL}, {WALL, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL}, {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}}};

    // Easy 2 (8x7, 2 boxes) - Two boxes, walls force routing
    allLevels[0][1] = {8, 7, {{WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}, {WALL, FLOOR, FLOOR, FLOOR, WALL, TARGET, FLOOR, WALL}, {WALL, FLOOR, BOX, FLOOR, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, BOX, FLOOR, TARGET, FLOOR, WALL}, {WALL, FLOOR, FLOOR, PLAYER, FLOOR, FLOOR, FLOOR, WALL}, {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}}};

    // Easy 3 (9x7, 2 boxes) - Corridor routing
    allLevels[0][2] = {9, 7, {{WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}, {WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, TARGET, FLOOR, WALL, FLOOR, BOX, FLOOR, WALL}, {WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, BOX, FLOOR, WALL, FLOOR, TARGET, FLOOR, WALL}, {WALL, FLOOR, FLOOR, PLAYER, WALL, FLOOR, FLOOR, FLOOR, WALL}, {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}}};

    // ========== MEDIUM LEVELS ==========

    // Medium 1 (9x8, 3 boxes) - Maze with narrow passages
    allLevels[1][0] = {9, 8, {{WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}, {WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, BOX, FLOOR, FLOOR, FLOOR, TARGET, FLOOR, WALL}, {WALL, FLOOR, FLOOR, WALL, WALL, WALL, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, WALL, TARGET, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, BOX, FLOOR, FLOOR, BOX, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, PLAYER, FLOOR, TARGET, FLOOR, FLOOR, WALL}, {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}}};

    // Medium 2 (10x8, 3 boxes) - Separated chambers
    allLevels[1][1] = {10, 8, {{WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}, {WALL, FLOOR, TARGET, FLOOR, WALL, FLOOR, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, BOX, FLOOR, FLOOR, WALL}, {WALL, FLOOR, BOX, FLOOR, FLOOR, FLOOR, FLOOR, WALL, FLOOR, WALL}, {WALL, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, WALL}, {WALL, TARGET, FLOOR, WALL, FLOOR, BOX, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, PLAYER, TARGET, FLOOR, WALL}, {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}}};

    // Medium 3 (10x9, 4 boxes) - Tight corridors
    allLevels[1][2] = {10, 9, {{WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}, {WALL, FLOOR, FLOOR, TARGET, WALL, FLOOR, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, BOX, FLOOR, FLOOR, WALL}, {WALL, FLOOR, BOX, FLOOR, FLOOR, FLOOR, FLOOR, WALL, FLOOR, WALL}, {WALL, WALL, FLOOR, WALL, WALL, WALL, FLOOR, WALL, FLOOR, WALL}, {WALL, FLOOR, FLOOR, FLOOR, TARGET, FLOOR, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, BOX, FLOOR, FLOOR, WALL, BOX, FLOOR, TARGET, WALL}, {WALL, FLOOR, FLOOR, PLAYER, FLOOR, FLOOR, FLOOR, FLOOR, TARGET, WALL}, {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}}};

    // ========== HARD LEVELS ==========

    // Hard 1 (9x9, 3 boxes) - The Detour
    allLevels[2][0] = {9, 9, {{WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}, {WALL, FLOOR, FLOOR, FLOOR, FLOOR, TARGET, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, WALL, BOX, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL, FLOOR, WALL}, {WALL, FLOOR, BOX, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL}, {WALL, TARGET, FLOOR, FLOOR, FLOOR, FLOOR, BOX, TARGET, WALL}, {WALL, FLOOR, FLOOR, PLAYER, FLOOR, FLOOR, FLOOR, FLOOR, WALL}, {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}}};

    // Hard 2 (10x9, 3 boxes) - The Crossroads
    allLevels[2][1] = {10, 9, {{WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}, {WALL, FLOOR, TARGET, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, FLOOR, BOX, FLOOR, WALL}, {WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL, FLOOR, WALL}, {WALL, FLOOR, BOX, FLOOR, FLOOR, WALL, BOX, FLOOR, TARGET, WALL}, {WALL, TARGET, FLOOR, PLAYER, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL}, {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}}};

    // Hard 3 (11x11, 4 boxes) - The Gauntlet
    allLevels[2][2] = {11, 11, {{WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}, {WALL, FLOOR, FLOOR, TARGET, WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL}, {WALL, FLOOR, BOX, FLOOR, FLOOR, FLOOR, WALL, FLOOR, BOX, FLOOR, WALL}, {WALL, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, FLOOR, WALL}, {WALL, WALL, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, FLOOR, TARGET, WALL}, {WALL, FLOOR, FLOOR, WALL, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL}, {WALL, TARGET, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL, FLOOR, FLOOR, WALL}, {WALL, FLOOR, FLOOR, BOX, FLOOR, WALL, FLOOR, FLOOR, BOX, FLOOR, WALL}, {WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, TARGET, FLOOR, WALL}, {WALL, FLOOR, PLAYER, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL}, {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}}};
}

void LoadLevel(int difficulty, int lvl)
{
    LevelDef &def = allLevels[difficulty][lvl];
    mapW = def.width;
    mapH = def.height;
    memcpy(level, def.data, sizeof(level));
    showSolution = false;
    solutionStep = 0;
    solutionTimer = 0.0f;
}

void LoadAssets()
{
    texWall = LoadTexture("assets/wall.png");
    texFloor = LoadTexture("assets/floor.png");
    texTarget = LoadTexture("assets/target.png");
    texBox = LoadTexture("assets/box.png");
    texBoxTarget = LoadTexture("assets/box_target.png");
    texPlayer = LoadTexture("assets/player.png");
    texPlayerTarget = LoadTexture("assets/player_target.png");

    SetTextureFilter(texWall, TEXTURE_FILTER_POINT);
    SetTextureFilter(texFloor, TEXTURE_FILTER_POINT);
    SetTextureFilter(texTarget, TEXTURE_FILTER_POINT);
    SetTextureFilter(texBox, TEXTURE_FILTER_POINT);
    SetTextureFilter(texBoxTarget, TEXTURE_FILTER_POINT);
    SetTextureFilter(texPlayer, TEXTURE_FILTER_POINT);
    SetTextureFilter(texPlayerTarget, TEXTURE_FILTER_POINT);
}

void UnloadAssets()
{
    UnloadTexture(texWall);
    UnloadTexture(texFloor);
    UnloadTexture(texTarget);
    UnloadTexture(texBox);
    UnloadTexture(texBoxTarget);
    UnloadTexture(texPlayer);
    UnloadTexture(texPlayerTarget);
}

int GetTileSize()
{
    // Use smaller tiles for maps that won't fit at 64px
    if (mapW * TILE_SIZE > SCREEN_W - 20 || mapH * TILE_SIZE > SCREEN_H - 60)
        return TILE_SIZE_SMALL;
    return TILE_SIZE;
}

void DrawMap()
{
    int tileSize = GetTileSize();
    int offsetX = (SCREEN_W - mapW * tileSize) / 2;
    int offsetY = (SCREEN_H - mapH * tileSize) / 2;

    for (int y = 0; y < mapH; y++)
    {
        for (int x = 0; x < mapW; x++)
        {
            Texture2D tex;

            switch (level[y][x])
            {
            case WALL:
                tex = texWall;
                break;
            case FLOOR:
                tex = texFloor;
                break;
            case TARGET:
                tex = texTarget;
                break;
            case BOX:
                tex = texBox;
                break;
            case BOX_ON_TARGET:
                tex = texBoxTarget;
                break;
            case PLAYER:
                tex = texPlayer;
                break;
            case PLAYER_ON_TARGET:
                tex = texPlayerTarget;
                break;
            }

            Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
            Rectangle dst = {(float)(offsetX + x * tileSize), (float)(offsetY + y * tileSize), (float)tileSize, (float)tileSize};
            DrawTexturePro(tex, src, dst, {0, 0}, 0, WHITE);
        }
    }
}

void FindPlayer(int &px, int &py)
{
    for (int y = 0; y < mapH; y++)
    {
        for (int x = 0; x < mapW; x++)
        {
            if (level[y][x] == PLAYER || level[y][x] == PLAYER_ON_TARGET)
            {
                px = x;
                py = y;
                return;
            }
        }
    }
}

bool IsWalkable(Tile t)
{
    return t == FLOOR || t == TARGET;
}

void TryMove(int dx, int dy)
{
    int px, py;
    FindPlayer(px, py);

    int nx = px + dx;
    int ny = py + dy;

    if (nx < 0 || nx >= mapW || ny < 0 || ny >= mapH)
        return;

    Tile next = level[ny][nx];

    if (next == WALL)
        return;

    if (next == BOX || next == BOX_ON_TARGET)
    {
        int bx = nx + dx;
        int by = ny + dy;

        if (bx < 0 || bx >= mapW || by < 0 || by >= mapH)
            return;

        if (!IsWalkable(level[by][bx]))
            return;

        level[by][bx] = (level[by][bx] == TARGET) ? BOX_ON_TARGET : BOX;
        level[ny][nx] = (next == BOX_ON_TARGET) ? TARGET : FLOOR;
    }

    level[py][px] = (level[py][px] == PLAYER_ON_TARGET) ? TARGET : FLOOR;
    level[ny][nx] = (level[ny][nx] == TARGET) ? PLAYER_ON_TARGET : PLAYER;
}

void HandleInput()
{
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
        TryMove(0, -1);
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
        TryMove(0, 1);
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))
        TryMove(-1, 0);
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT))
        TryMove(1, 0);
}

bool CheckWin()
{
    for (int y = 0; y < mapH; y++)
        for (int x = 0; x < mapW; x++)
            if (level[y][x] == TARGET || level[y][x] == PLAYER_ON_TARGET)
                return false;
    return true;
}

bool DrawButton(const char *text, int x, int y, int w, int h, Color color)
{
    Rectangle rect = {(float)x, (float)y, (float)w, (float)h};
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, rect);
    bool clicked = hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    Color bgColor = hover ? Fade(color, 0.4f) : Fade(color, 0.2f);
    DrawRectangleRec(rect, bgColor);
    DrawRectangleLinesEx(rect, 2, color);

    int fontSize = 30;
    int textW = MeasureText(text, fontSize);
    DrawText(text, x + (w - textW) / 2, y + (h - fontSize) / 2, fontSize, WHITE);

    return clicked;
}

int main()
{
    InitWindow(SCREEN_W, SCREEN_H, "Sokoban");
    SetTargetFPS(60);
    SetExitKey(0); // Disable ESC auto-close so we can use it for navigation

    LoadAssets();
    InitLevels();

    Color difficultyColors[] = {GREEN, ORANGE, RED};

    while (!WindowShouldClose() && !shouldQuit)
    {
        BeginDrawing();
        ClearBackground(DARK_BG);

        switch (gameState)
        {
        case STATE_DIFFICULTY:
        {
            // Title
            const char *title = "SOKOBAN";
            int titleW = MeasureText(title, 60);
            DrawText(title, (SCREEN_W - titleW) / 2, 80, 60, WHITE);

            const char *subtitle = "Select Difficulty";
            int subW = MeasureText(subtitle, 25);
            DrawText(subtitle, (SCREEN_W - subW) / 2, 160, 25, LIGHTGRAY);

            // Difficulty buttons
            int btnW = 250, btnH = 60;
            int btnX = (SCREEN_W - btnW) / 2;
            int startY = 240;
            int gap = 80;

            if (DrawButton("EASY", btnX, startY, btnW, btnH, GREEN))
            {
                selectedDifficulty = 0;
                selectedLevel = 0;
                LoadLevel(selectedDifficulty, selectedLevel);
                gameState = STATE_PLAYING;
            }
            if (DrawButton("MEDIUM", btnX, startY + gap, btnW, btnH, ORANGE))
            {
                selectedDifficulty = 1;
                selectedLevel = 0;
                LoadLevel(selectedDifficulty, selectedLevel);
                gameState = STATE_PLAYING;
            }
            if (DrawButton("HARD", btnX, startY + gap * 2, btnW, btnH, RED))
            {
                selectedDifficulty = 2;
                selectedLevel = 0;
                LoadLevel(selectedDifficulty, selectedLevel);
                gameState = STATE_PLAYING;
            }

            // Keyboard shortcuts
            if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1))
            {
                selectedDifficulty = 0;
                selectedLevel = 0;
                LoadLevel(selectedDifficulty, selectedLevel);
                gameState = STATE_PLAYING;
            }
            if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2))
            {
                selectedDifficulty = 1;
                selectedLevel = 0;
                LoadLevel(selectedDifficulty, selectedLevel);
                gameState = STATE_PLAYING;
            }
            if (IsKeyPressed(KEY_THREE) || IsKeyPressed(KEY_KP_3))
            {
                selectedDifficulty = 2;
                selectedLevel = 0;
                LoadLevel(selectedDifficulty, selectedLevel);
                gameState = STATE_PLAYING;
            }
            if (IsKeyPressed(KEY_ESCAPE))
                shouldQuit = true;

            // Footer hint
            DrawText("Press 1, 2, 3 or click  |  ESC to quit", 180, SCREEN_H - 35, 18, GRAY);
            break;
        }

        case STATE_PLAYING:
        {
            // Solution auto-play mode
            if (showSolution)
            {
                solutionTimer += GetFrameTime();
                const char *sol = solutions[selectedDifficulty][selectedLevel];
                int solLen = (int)strlen(sol);

                if (solutionStep < solLen && solutionTimer >= SOLUTION_STEP_DELAY)
                {
                    solutionTimer = 0.0f;
                    char c = sol[solutionStep];
                    switch (c)
                    {
                    case 'U':
                        TryMove(0, -1);
                        break;
                    case 'D':
                        TryMove(0, 1);
                        break;
                    case 'L':
                        TryMove(-1, 0);
                        break;
                    case 'R':
                        TryMove(1, 0);
                        break;
                    }
                    solutionStep++;
                }

                // Stop showing solution on any key press
                if (IsKeyPressed(KEY_H) || IsKeyPressed(KEY_ESCAPE))
                {
                    showSolution = false;
                    if (IsKeyPressed(KEY_ESCAPE))
                    {
                        LoadLevel(selectedDifficulty, selectedLevel);
                        gameState = STATE_DIFFICULTY;
                    }
                }

                // Restart cancels solution
                if (IsKeyPressed(KEY_R))
                {
                    LoadLevel(selectedDifficulty, selectedLevel);
                }
            }
            else
            {
                HandleInput();

                // Show solution
                if (IsKeyPressed(KEY_H))
                {
                    LoadLevel(selectedDifficulty, selectedLevel);
                    showSolution = true;
                    solutionStep = 0;
                    solutionTimer = 0.0f;
                }

                // Restart level
                if (IsKeyPressed(KEY_R))
                    LoadLevel(selectedDifficulty, selectedLevel);

                // Back to difficulty menu
                if (IsKeyPressed(KEY_ESCAPE))
                    gameState = STATE_DIFFICULTY;
            }

            DrawMap();

            // HUD
            const char *info = TextFormat("%s - Level %d", difficultyNames[selectedDifficulty], selectedLevel + 1);
            DrawText(info, 10, 10, 20, WHITE);

            if (showSolution)
            {
                const char *sol = solutions[selectedDifficulty][selectedLevel];
                int solLen = (int)strlen(sol);
                const char *solInfo = TextFormat("SOLUTION: Step %d / %d", solutionStep, solLen);
                int solInfoW = MeasureText(solInfo, 22);
                DrawText(solInfo, (SCREEN_W - solInfoW) / 2, 10, 22, YELLOW);
                DrawText("H: Stop  |  R: Restart", 10, SCREEN_H - 30, 18, YELLOW);
            }
            else
            {
                DrawText("R: Restart  |  H: Solution  |  ESC: Menu", 10, SCREEN_H - 30, 18, LIGHTGRAY);
            }

            // Check win
            if (CheckWin())
                gameState = STATE_WIN;

            break;
        }

        case STATE_WIN:
        {
            DrawMap();

            // Dark overlay
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.6f));

            const char *winText = TextFormat("LEVEL %d COMPLETE!", selectedLevel + 1);
            int winW = MeasureText(winText, 50);
            DrawText(winText, (SCREEN_W - winW) / 2, 200, 50, GREEN);

            if (selectedLevel < LEVELS_PER_DIFFICULTY - 1)
            {
                const char *nextText = "Press any key for next level";
                int nextW = MeasureText(nextText, 22);
                DrawText(nextText, (SCREEN_W - nextW) / 2, 300, 22, WHITE);

                if (GetKeyPressed() != 0 || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    selectedLevel++;
                    LoadLevel(selectedDifficulty, selectedLevel);
                    gameState = STATE_PLAYING;
                }
            }
            else
            {
                gameState = STATE_ALL_COMPLETE;
            }

            break;
        }

        case STATE_ALL_COMPLETE:
        {
            Color dColor = difficultyColors[selectedDifficulty];

            const char *congrats = "CONGRATULATIONS!";
            int cW = MeasureText(congrats, 50);
            DrawText(congrats, (SCREEN_W - cW) / 2, 150, 50, GOLD);

            const char *completed = TextFormat("You beat all %s levels!", difficultyNames[selectedDifficulty]);
            int compW = MeasureText(completed, 28);
            DrawText(completed, (SCREEN_W - compW) / 2, 230, 28, dColor);

            int btnW = 250, btnH = 60;
            int btnX = (SCREEN_W - btnW) / 2;

            if (DrawButton("BACK TO MENU", btnX, 340, btnW, btnH, GRAY))
            {
                gameState = STATE_DIFFICULTY;
            }

            if (IsKeyPressed(KEY_ESCAPE) || GetKeyPressed() != 0)
                gameState = STATE_DIFFICULTY;

            break;
        }
        }

        EndDrawing();
    }

    UnloadAssets();
    CloseWindow();
    return 0;
}