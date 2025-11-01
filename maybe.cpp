#include "raylib.h"
#include <cstdlib>
#include <cmath>

#define MAGE_SPEED 5
#define BULLET_SPEED 8
#define DEMON_BULLET_SPEED 6
#define MAX_BULLETS 20
#define MAX_DEMON_BULLETS 10

typedef struct Attack {
    const char *name;
    Texture2D texture;
    Sound sound;
    int damage;
    KeyboardKey key;
} Attack;

int main() {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(0, 0, "STRANGE");
    int SCREEN_WIDTH = GetScreenWidth();
    int SCREEN_HEIGHT = GetScreenHeight();

    InitAudioDevice();
    SetTargetFPS(60);

    // Textures
    Image mageImg = LoadImage("images/frierenn.png");
    ImageResize(&mageImg, mageImg.width/2, mageImg.height/2);
    Texture2D mageTex = LoadTextureFromImage(mageImg);
    //Texture2D mageTex = LoadTexture("images/frierenn.png");
    Image mageImg2 = LoadImage("images/enemy.png");
    ImageResize(&mageImg2, mageImg2.width/5, mageImg2.height/5);
    Texture2D demonTex = LoadTextureFromImage(mageImg2);
    //Texture2D demonTex = LoadTexture("images/enemy.png");
    Texture2D bgTex = LoadTexture("images/bg.png");
   

    // Music
    Music bgm = LoadMusicStream("wavfiles/cinematic-background-1-291975.wav");
    PlayMusicStream(bgm);
    SetMusicVolume(bgm, 0.6f);

    // Mage Attacks
    Attack mageAttacks[8] = {
        {"Fireball", LoadTexture("images/bluu.png"), LoadSound("wavfiles/fa.wav"), 10, KEY_Z},
        {"Ice Blast", LoadTexture("images/bluu.png"), LoadSound("wavfiles/do.wav"), 8, KEY_X},
        {"Thunder", LoadTexture("images/bluu.png"), LoadSound("wavfiles/mi.wav"), 12, KEY_C},
        {"Wind Slash", LoadTexture("images/bluu.png"), LoadSound("wavfiles/do.wav"), 7, KEY_V},
        {"Earth Spike", LoadTexture("images/purple.png"), LoadSound("wavfiles/fa.wav"), 9, KEY_B},
        {"Dark Orb", LoadTexture("images/purple.png"), LoadSound("wavfiles/mi.wav"), 11, KEY_N},
        {"Light Beam", LoadTexture("images/purple.png"), LoadSound("wavfiles/do.wav"), 10, KEY_M},
        {"Arcane Shot", LoadTexture("images/purple.png"), LoadSound("wavfiles/fa.wav"), 13, KEY_COMMA}
    };

    // Demon Attacks
    Attack demonAttacks[3] = {
        {"Dark Flame", LoadTexture("images/black.png"), LoadSound("wavfiles/scary-violin-382735.wav"), 12, KEY_NULL},
        {"Shadow Spear", LoadTexture("images/bllack.png"), LoadSound("wavfiles/scary-violin-382735.wav"), 10, KEY_NULL},
        {"Doom Wave", LoadTexture("images/black.png"), LoadSound("wavfiles/scary-violin-382735.wav"), 15, KEY_NULL}
    };

    // Game Entities
    Vector2 magePos = {(float)SCREEN_WIDTH / 6, (float)SCREEN_HEIGHT / 2};
    Rectangle mageRect = {magePos.x, magePos.y, (float)mageTex.width, (float)mageTex.height};
    int mageHealth = 100;

    Vector2 demonPos = {(float)SCREEN_WIDTH * 3 / 4, (float)SCREEN_HEIGHT / 2};
    Rectangle demonRect = {demonPos.x, demonPos.y, (float)demonTex.width, (float)demonTex.height};
    int demonHealth = 200;

    DrawTextureEx(mageTex, {magePos.x, magePos.y}, 0.0f, 0.2f, WHITE);

    // Mage bullets
    Vector2 bullets[MAX_BULLETS] = {0};
    bool bulletActive[MAX_BULLETS] = {false};
    int bulletType[MAX_BULLETS] = {0};

    // Demon bullets
    Vector2 demonBullets[MAX_DEMON_BULLETS] = {0};
    bool demonBulletActive[MAX_DEMON_BULLETS] = {false};
    int demonBulletType[MAX_DEMON_BULLETS] = {0};
    Vector2 demonBulletDir[MAX_DEMON_BULLETS] = {0};

    float demonTimer = 0.0f;

    // Game Loop
    while (!WindowShouldClose()) {
        UpdateMusicStream(bgm);

        // PLAYER MOVEMENT
        if (IsKeyDown(KEY_W)) magePos.y -= MAGE_SPEED;
        if (IsKeyDown(KEY_S)) magePos.y += MAGE_SPEED;
        if (IsKeyDown(KEY_A)) magePos.x -= MAGE_SPEED;
        if (IsKeyDown(KEY_D)) magePos.x += MAGE_SPEED;

       

        // MAGE ATTACKS
        for (int i = 0; i < 8; i++) {
            if (IsKeyPressed(mageAttacks[i].key)) {
                PlaySound(mageAttacks[i].sound); // ✅ multiple sounds can overlap
                for (int b = 0; b < MAX_BULLETS; b++) {
                    if (!bulletActive[b]) {
                        bulletActive[b] = true;
                        bullets[b] = {magePos.x + mageTex.width, magePos.y + mageTex.height / 2};
                        bulletType[b] = i;
                        break;
                    }
                }
            }
        }

        // Mage bullet movement
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bulletActive[i]) {
                bullets[i].x += BULLET_SPEED;
                if (bullets[i].x > SCREEN_WIDTH)
                    bulletActive[i] = false;
            }
        }

        // Mage bullet collision
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bulletActive[i]) {
                Rectangle bulletRect = {bullets[i].x, bullets[i].y, 15, 15}; // smaller collision
                if (CheckCollisionRecs(bulletRect, demonRect)) {
                    bulletActive[i] = false;
                    demonHealth -= mageAttacks[bulletType[i]].damage;
                }
            }
        }

        // DEMON ATTACKS
        demonTimer += GetFrameTime();
        if (demonTimer > 2.5f) {
            demonTimer = 0;
            int atk = GetRandomValue(0, 2);
            PlaySound(demonAttacks[atk].sound); // ✅ use PlaySoundMulti here too

            for (int b = 0; b < MAX_DEMON_BULLETS; b++) {
                if (!demonBulletActive[b]) {
                    demonBulletActive[b] = true;
                    demonBulletType[b] = atk;
                    demonBullets[b] = {demonPos.x, demonPos.y + demonTex.height / 2};

                    // direction toward mage
                    Vector2 dir = {magePos.x - demonPos.x, magePos.y - demonPos.y};
                    float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
                    dir.x /= len;
                    dir.y /= len;
                    demonBulletDir[b] = dir;
                    break;
                }
            }
        }

        // Demon bullet movement
        for (int i = 0; i < MAX_DEMON_BULLETS; i++) {
            if (demonBulletActive[i]) {
                demonBullets[i].x += demonBulletDir[i].x * DEMON_BULLET_SPEED;
                demonBullets[i].y += demonBulletDir[i].y * DEMON_BULLET_SPEED;

                Rectangle demonBulletRect = {demonBullets[i].x, demonBullets[i].y, 20, 20};
                if (CheckCollisionRecs(demonBulletRect, mageRect)) {
                    mageHealth -= demonAttacks[demonBulletType[i]].damage;
                    demonBulletActive[i] = false;
                }

                if (demonBullets[i].x < 0 || demonBullets[i].x > SCREEN_WIDTH ||
                    demonBullets[i].y < 0 || demonBullets[i].y > SCREEN_HEIGHT)
                    demonBulletActive[i] = false;
            }
        }

        // DRAW EVERYTHING
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(bgTex, 0, 0, WHITE);

        DrawTexture(mageTex, magePos.x, magePos.y, WHITE);
        DrawTexture(demonTex, demonPos.x, demonPos.y, WHITE);

        // Smaller mage bullets
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bulletActive[i]) {
                float scale = 0.2f; // ✅ very small mage attack
                DrawTextureEx(mageAttacks[bulletType[i]].texture, bullets[i], 0, scale, WHITE);
            }
        }

        // Smaller demon bullets
        for (int i = 0; i < MAX_DEMON_BULLETS; i++) {
            if (demonBulletActive[i]) {
                float scale = 0.25f; // ✅ small demon attack
                DrawTextureEx(demonAttacks[demonBulletType[i]].texture, demonBullets[i], 0, scale, WHITE);
            }
        }

        // Health bars
        DrawRectangle(50, 30, 200, 20, GRAY);
        DrawRectangle(50, 30, mageHealth * 2, 20, GREEN);
        DrawText("Mage", 50, 10, 20, BLUE);

        DrawRectangle(SCREEN_WIDTH - 250, 30, 200, 20, GRAY);
        DrawRectangle(SCREEN_WIDTH - 250, 30, demonHealth, 20, RED);
        DrawText("Demon King", SCREEN_WIDTH - 250, 10, 20, MAROON);

        if (mageHealth <= 0)
            DrawText("Demon King Wins!", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, 30, RED);
        else if (demonHealth <= 0)
            DrawText("Mage Wins!", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 30, BLUE);

        EndDrawing();
    }

    // Cleanup
    for (int i = 0; i < 8; i++) {
        UnloadTexture(mageAttacks[i].texture);
        UnloadSound(mageAttacks[i].sound);
    }
    for (int i = 0; i < 3; i++) {
        UnloadTexture(demonAttacks[i].texture);
        UnloadSound(demonAttacks[i].sound);
    }
    UnloadTexture(mageTex);
    UnloadTexture(demonTex);
    UnloadTexture(bgTex);
    UnloadImage(mageImg);
    UnloadMusicStream(bgm);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
