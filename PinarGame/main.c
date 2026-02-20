#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>

typedef enum GameState { MENU, GARAGE, PLAYING, GAMEOVER } GameState;

const int screenWidth = 1024;
const int screenHeight = 768;
#define MAX_COINS 10
#define MAX_OBSTACLES 8

// --- Yapılar ---
typedef struct {
    Vector2 pos;
    bool active;
} Entity;

typedef struct {
    Texture2D texture;
    Vector2 position;
    float speed;
    float rotation;
    float maxSpeed;
    float acceleration;
    int health; // YENİ: Arabanın canı
} Car;

// Renk seçenekleri
Color carColors[] = { WHITE, RED, BLUE, LIME, GOLD, PURPLE };
int selectedColorIndex = 0;
const int totalColors = 6;

void DrawEndlessRoad(Texture2D grass, Texture2D road, Vector2 cameraTarget);

int main() {
    InitWindow(screenWidth, screenHeight, "Pro Racer: Yol ve Engeller");

    Texture2D carTex = LoadTexture("resources/arabak.png");
    Texture2D grassTex = LoadTexture("resources/grass.png");
    Texture2D roadTex = LoadTexture("resources/road.png");

    // Oyuncu Başlangıç Ayarları (3 Can ile başlar)
    Car player = { carTex, { 512, 384 }, 0.0f, 0.0f, 7.0f, 0.06f, 3 };
    GameState currentState = MENU;
    int score = 0;

    // Altınlar ve Engeller (Variller)
    Entity coins[MAX_COINS];
    Entity obstacles[MAX_OBSTACLES];

    // İlk yerleştirme
    for (int i = 0; i < MAX_COINS; i++) coins[i].active = false;
    for (int i = 0; i < MAX_OBSTACLES; i++) obstacles[i].active = false;

    Camera2D camera = { 0 };
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- GÜNCELLEME ---
        
        // 1. ANA MENÜ DURUMU
        if (currentState == MENU) {
            if (IsKeyPressed(KEY_ENTER)) currentState = PLAYING;
            if (IsKeyPressed(KEY_G)) currentState = GARAGE;
        } 
        
        // 2. GARAJ DURUMU (YENİ)
        else if (currentState == GARAGE) {
            if (IsKeyPressed(KEY_RIGHT)) selectedColorIndex = (selectedColorIndex + 1) % totalColors;
            if (IsKeyPressed(KEY_LEFT)) {
                selectedColorIndex--;
                if (selectedColorIndex < 0) selectedColorIndex = totalColors - 1;
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) currentState = MENU;
        }
        
        // 3. OYUN BİTTİ DURUMU (YENİ)
        else if (currentState == GAMEOVER) {
            if (IsKeyPressed(KEY_ENTER)) {
                // Oyunu Sıfırla
                player.position = (Vector2){ 512, 384 };
                player.speed = 0.0f;
                player.rotation = 0.0f;
                player.health = 3;
                score = 0;
                for (int i = 0; i < MAX_COINS; i++) coins[i].active = false;
                for (int i = 0; i < MAX_OBSTACLES; i++) obstacles[i].active = false;
                currentState = PLAYING;
            }
        }
        
        // 4. OYUN OYNANIYOR DURUMU
        else if (currentState == PLAYING) {
            // İleri gidildikçe skoru yavaşça artır
            if (player.speed > 2.0f) score += 1;

            // Kontroller
            if (IsKeyDown(KEY_W)) player.speed += player.acceleration;
            else if (IsKeyDown(KEY_S)) player.speed -= player.acceleration * 2.0f;
            else player.speed *= 0.98f; // Sürtünme

            // Hıza duyarlı dönüş
            float turnFactor = (player.speed / player.maxSpeed) * 4.0f;
            if (IsKeyDown(KEY_A)) player.rotation -= turnFactor;
            if (IsKeyDown(KEY_D)) player.rotation += turnFactor;

            // Yol Sınırı Kontrolü (Off-road cezası)
            float roadWidth = roadTex.width;
            float roadLeft = (screenWidth / 2.0f) - (roadWidth / 2.0f);
            float roadRight = (screenWidth / 2.0f) + (roadWidth / 2.0f);

            if (player.position.x < roadLeft + 20 || player.position.x > roadRight - 20) {
                player.speed *= 0.92f; // Çimende çok yavaşla
                if (player.speed > 1.0f) player.position.x += GetRandomValue(-2, 2); // Titreme efekti
            }

            // Hareket Matematigi
            Vector2 dir = Vector2Rotate((Vector2){ 0, -1 }, player.rotation * DEG2RAD);
            player.position = Vector2Add(player.position, Vector2Scale(dir, player.speed));
            
            // Kamera Takibi
            camera.target = Vector2Lerp(camera.target, player.position, 0.1f);

            // --- ENGEL DÖNGÜSÜ ---
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                if (!obstacles[i].active || obstacles[i].pos.y > player.position.y + 500) {
                    obstacles[i].pos = (Vector2){ (float)GetRandomValue(roadLeft + 50, roadRight - 50), player.position.y - GetRandomValue(800, 2000) };
                    obstacles[i].active = true;
                }
                // Çarpışma Kontrolü (Engel)
                if (CheckCollisionCircles(player.position, 15, obstacles[i].pos, 20)) {
                    player.speed *= 0.2f; // Hızı kes
                    obstacles[i].active = false; // Engeli yok et
                    player.health -= 1; // Can düşür
                    
                    if (player.health <= 0) currentState = GAMEOVER; // Can biterse öl
                }
            }

            // --- ALTIN DÖNGÜSÜ (YENİ) ---
            for (int i = 0; i < MAX_COINS; i++) {
                if (!coins[i].active || coins[i].pos.y > player.position.y + 500) {
                    coins[i].pos = (Vector2){ (float)GetRandomValue(roadLeft + 50, roadRight - 50), player.position.y - GetRandomValue(500, 1500) };
                    coins[i].active = true;
                }
                // Çarpışma Kontrolü (Altın)
                if (CheckCollisionCircles(player.position, 20, coins[i].pos, 15)) {
                    score += 500; // Altın toplayınca skor ver
                    coins[i].active = false; // Altını yoldan sil
                }
            }
        }

        // --- ÇİZİM ---
        BeginDrawing();
            ClearBackground(DARKGREEN);

            // ANA MENÜ ÇİZİMİ
            if (currentState == MENU) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));
                DrawText("PRO RACER", screenWidth/2 - 130, 200, 50, RED);
                DrawText("[ENTER] BASLA  |  [G] GARAJ", screenWidth/2 - 190, 400, 25, WHITE);
            }
            
            // GARAJ ÇİZİMİ (YENİ)
            else if (currentState == GARAGE) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(DARKBLUE, 0.9f));
                DrawText("GARAJ", screenWidth/2 - 70, 100, 40, GOLD);
                DrawText("< SOL OK       SAG OK >", screenWidth/2 - 150, screenHeight/2 + 150, 25, WHITE);
                DrawText("[ENTER] MENUYE DON", screenWidth/2 - 130, screenHeight - 100, 20, LIGHTGRAY);
                
                // Arabayı kocaman çiz
                if (player.texture.id != 0) {
                    DrawTexturePro(player.texture, 
                        (Rectangle){0,0,player.texture.width, player.texture.height}, 
                        (Rectangle){screenWidth/2, screenHeight/2, 100, 180}, 
                        (Vector2){50, 90}, 0, carColors[selectedColorIndex]);
                }
            }

            // OYUN BİTTİ ÇİZİMİ (YENİ)
            else if (currentState == GAMEOVER) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.9f));
                DrawText("OYUN BITTI!", screenWidth/2 - 140, 250, 50, RED);
                DrawText(TextFormat("TOPLAM SKOR: %i", score), screenWidth/2 - 140, 350, 30, GOLD);
                DrawText("[ENTER] TEKRAR DENE", screenWidth/2 - 150, 500, 25, WHITE);
            }

            // OYUN OYNANIYOR ÇİZİMİ
            else if (currentState == PLAYING) {
                BeginMode2D(camera);
                    DrawEndlessRoad(grassTex, roadTex, camera.target);

                    // Altınları Çiz (YENİ)
                    for (int i = 0; i < MAX_COINS; i++) {
                        if (coins[i].active) {
                            DrawCircle(coins[i].pos.x, coins[i].pos.y, 12, GOLD);
                            DrawCircle(coins[i].pos.x, coins[i].pos.y, 7, YELLOW); // Parlama efekti
                        }
                    }

                    // Engelleri Çiz (Kırmızı variller gibi)
                    for (int i = 0; i < MAX_OBSTACLES; i++) {
                        if (obstacles[i].active) {
                            DrawCircleGradient(obstacles[i].pos.x, obstacles[i].pos.y, 20, GRAY, DARKGRAY);
                            DrawRectangle(obstacles[i].pos.x - 20, obstacles[i].pos.y - 5, 40, 10, RED); 
                        }
                    }

                    // Araba Çizimi
                    if (player.texture.id != 0) {
                        DrawTexturePro(player.texture, (Rectangle){0,0,player.texture.width, player.texture.height}, 
                                       (Rectangle){player.position.x, player.position.y, 40, 70}, 
                                       (Vector2){20,35}, player.rotation, carColors[selectedColorIndex]);
                    } else {
                        DrawRectanglePro((Rectangle){player.position.x, player.position.y, 40, 70}, 
                                         (Vector2){20,35}, player.rotation, MAGENTA);
                    }              
                EndMode2D();

                // HUD (Bilgi Ekranı)
                DrawRectangle(10, 10, 240, 100, Fade(BLACK, 0.6f));
                DrawText(TextFormat("SKOR: %i", score), 20, 20, 20, WHITE);
                DrawText(TextFormat("HIZ: %.0f KM/H", player.speed * 20), 20, 45, 20, LIGHTGRAY);
                DrawText(TextFormat("CAN: %i / 3", player.health), 20, 70, 20, (player.health == 1) ? RED : LIME);
            }
        EndDrawing();
    }
    
    // Temizlik...
    UnloadTexture(carTex);
    UnloadTexture(grassTex);
    UnloadTexture(roadTex);
    CloseWindow();
    return 0;
}

void DrawEndlessRoad(Texture2D grass, Texture2D road, Vector2 cameraTarget) {
    int gW = grass.width;
    int rH = road.height;
    float roadX = (screenWidth / 2.0f) - (road.width / 2.0f);

    // Çimenler
    for (int x = -2; x < 3; x++) {
        for (int y = -3; y < 4; y++) {
            float posX = (floor(cameraTarget.x / gW) + x) * gW;
            float posY = (floor(cameraTarget.y / gW) + y) * gW;
            DrawTexture(grass, posX, posY, WHITE);
        }
    }
    // Yol şeritleri
    for (int y = -4; y < 5; y++) {
        float posY = (floor(cameraTarget.y / rH) + y) * rH;
        DrawTexture(road, roadX, posY, WHITE);
    }
}