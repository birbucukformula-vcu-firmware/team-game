#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>

typedef enum GameState { MENU, GARAGE, PLAYING, GAMEOVER } GameState;
typedef enum WeatherType { SUNNY, SUNSET, NIGHT, RAINY, SNOWY } WeatherType;

const int screenWidth = 1024;
const int screenHeight = 768;
#define MAX_COINS 10
#define MAX_OBSTACLES 10 
#define MAX_PARTICLES 150 
#define MAX_WEATHER_PARTICLES 150 
#define MAX_REPAIRS 2 

// --- Yapılar ---
typedef struct {
    Vector2 pos;
    bool active;
} Entity;

typedef struct {
    Vector2 pos;
    bool active;
    float speed;  
    Color color;  
} Obstacle;

typedef struct {
    Vector2 position;
    float speed;
    float rotation;
    float maxSpeed;
    float acceleration;
    float brakingPower; 
    int health;
    float boost; 
    float maxBoost;     
} Car;

typedef struct {
    Vector2 pos;
    float speed;
    float maxSpeed;
    float targetX; 
    Color color;
} Rival;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float life;
    float maxLife;
    float size;
    Color color;
    bool active;
    bool isSkidMark; 
} Particle;

typedef struct {
    Vector2 pos;
    float speed;
    float size;
    bool active;
} WeatherParticle;

Color carColors[] = { RED, (Color){0, 121, 241, 255}, DARKGRAY, GREEN, ORANGE, PURPLE };
const char* teamNames[] = { "FERRARI", "ALPINE", "MERCEDES", "ASTON MARTIN", "MCLAREN", "RED BULL" };
int selectedColorIndex = 0;
const int totalColors = 6;

const char* weatherNames[] = { "GUNESLI", "GUN BATIMI", "GECE YARISI", "YAGMURLU", "KARLI" };

int teamBudget = 0; // Kasa -> Takım Bütçesi
int aeroLevel = 1;  // Motor -> Aerodinamik
int brakeLevel = 1;
int ersLevel = 1;   // Nitro -> ERS

void DrawF1Track(Texture2D grass, Texture2D road, Vector2 cameraTarget, WeatherType weather);
Color GetBackgroundColor(WeatherType weather);
void DrawFormulaCar(Vector2 pos, float rotation, Color carColor); // YENİ F1 ARAÇ ÇİZİMİ

int main() {
    InitWindow(screenWidth, screenHeight, "F1 Grand Prix: Formula Heyecani");

    // İsteğe bağlı dokular (Yoksa bile oyun kendi çizecek)
    Texture2D grassTex = LoadTexture("resources/grass.png");
    Texture2D roadTex = LoadTexture("resources/yol.png");

    Car player = { { 512, 384 }, 0.0f, 0.0f, 9.0f, 0.08f, 0.18f, 3, 100.0f, 100.0f };
    Rival rival = { { 480, 384 }, 0.0f, 8.8f, 480.0f, DARKBLUE }; // Rakip
    GameState currentState = MENU;
    WeatherType currentWeather = SUNNY; 
    
    int score = 0; 
    float cameraShake = 0.0f;
    
    const float raceDistance = -30000.0f; // Yarış daha uzun (F1)
    bool playerWon = false;
    bool raceFinished = false;

    Entity sponsors[MAX_COINS];
    Entity pitStops[MAX_REPAIRS]; 
    Obstacle backmarkers[MAX_OBSTACLES]; 
    Particle particles[MAX_PARTICLES] = {0}; 
    WeatherParticle weatherParticles[MAX_WEATHER_PARTICLES] = {0};

    for (int i = 0; i < MAX_COINS; i++) sponsors[i].active = false;
    for (int i = 0; i < MAX_REPAIRS; i++) pitStops[i].active = false;
    for (int i = 0; i < MAX_OBSTACLES; i++) backmarkers[i].active = false;
    for (int i = 0; i < MAX_WEATHER_PARTICLES; i++) weatherParticles[i].active = false;

    Camera2D camera = { 0 };
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- GÜNCELLEME ---
        
        if (currentState == MENU) {
            if (IsKeyPressed(KEY_ENTER)) currentState = PLAYING;
            if (IsKeyPressed(KEY_G)) currentState = GARAGE;
        } 
        else if (currentState == GARAGE) {
            if (IsKeyPressed(KEY_RIGHT)) selectedColorIndex = (selectedColorIndex + 1) % totalColors;
            if (IsKeyPressed(KEY_LEFT)) {
                selectedColorIndex--;
                if (selectedColorIndex < 0) selectedColorIndex = totalColors - 1;
            }
            
            if (IsKeyPressed(KEY_UP)) currentWeather = (currentWeather + 1) % 5;
            if (IsKeyPressed(KEY_DOWN)) {
                currentWeather = (currentWeather - 1);
                if (currentWeather < 0) currentWeather = 4;
            }

            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) currentState = MENU;

            int aeroCost = aeroLevel * 1500;
            int brakeCost = brakeLevel * 1200;
            int ersCost = ersLevel * 1800;

            if (IsKeyPressed(KEY_ONE) && teamBudget >= aeroCost && aeroLevel < 5) { teamBudget -= aeroCost; aeroLevel++; }
            if (IsKeyPressed(KEY_TWO) && teamBudget >= brakeCost && brakeLevel < 5) { teamBudget -= brakeCost; brakeLevel++; }
            if (IsKeyPressed(KEY_THREE) && teamBudget >= ersCost && ersLevel < 5) { teamBudget -= ersCost; ersLevel++; }
        }
        else if (currentState == GAMEOVER) {
            if (IsKeyPressed(KEY_ENTER)) {
                player.position = (Vector2){ 512, 384 };
                player.speed = 0.0f;
                player.rotation = 0.0f;
                player.health = 3;
                
                player.maxSpeed = 9.0f + (aeroLevel * 0.6f);
                player.brakingPower = 0.18f + (brakeLevel * 0.05f);
                player.maxBoost = 100.0f + (ersLevel * 20.0f);
                player.boost = player.maxBoost;

                rival.pos = (Vector2){ 480, 384 };
                rival.speed = 0.0f;
                rival.maxSpeed = player.maxSpeed * 0.96f; 

                score = 0;
                cameraShake = 0.0f;
                raceFinished = false;
                playerWon = false;
                for (int i = 0; i < MAX_COINS; i++) sponsors[i].active = false;
                for (int i = 0; i < MAX_REPAIRS; i++) pitStops[i].active = false;
                for (int i = 0; i < MAX_OBSTACLES; i++) backmarkers[i].active = false;
                for (int i = 0; i < MAX_PARTICLES; i++) particles[i].active = false;
                for (int i = 0; i < MAX_WEATHER_PARTICLES; i++) weatherParticles[i].active = false;
                currentState = PLAYING;
            }
        }
        else if (currentState == PLAYING) {
            if (player.speed > 2.0f) score += 5; 

            if (player.position.y <= raceDistance || rival.pos.y <= raceDistance) {
                raceFinished = true;
                playerWon = (player.position.y <= rival.pos.y);
                if (playerWon) score += 10000; // Grand Prix Birincilik Ödülü
                teamBudget += score;
                currentState = GAMEOVER;
            }

            float currentMaxSpeed = player.maxSpeed;
            bool isDrafting = false; // YENİ DRS Sistemi

            // DRS / Hava Koridoru Kontrolü
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                if (backmarkers[i].active && backmarkers[i].pos.y < player.position.y && backmarkers[i].pos.y > player.position.y - 300) {
                    if (abs(backmarkers[i].pos.x - player.position.x) < 30) isDrafting = true;
                }
            }
            if (rival.pos.y < player.position.y && rival.pos.y > player.position.y - 350 && abs(rival.pos.x - player.position.x) < 30) {
                isDrafting = true;
            }

            if (isDrafting && player.speed > 5.0f) {
                currentMaxSpeed = player.maxSpeed * 1.35f; // DRS ile muazzam hız
                player.speed += 0.08f;
            }

            if (IsKeyDown(KEY_LEFT_SHIFT) && player.boost > 0 && player.speed > 1.0f) {
                currentMaxSpeed = player.maxSpeed * 1.5f; 
                player.speed += player.acceleration * 2.5f; 
                player.boost -= 0.6f; 
                camera.zoom = Lerp(camera.zoom, 0.88f, 0.05f); 
            } else {
                if (player.boost < player.maxBoost) player.boost += 0.05f; 
            }

            bool isBraking = false;
            bool isTurningHard = false;

            if (IsKeyDown(KEY_W)) player.speed += player.acceleration;
            else if (IsKeyDown(KEY_S)) {
                player.speed -= player.brakingPower; 
                isBraking = true; 
            }
            else player.speed *= 0.985f; // F1 araçları havayı keser ama çabuk yavaşlar

            if (player.speed < -4.0f) player.speed = -4.0f; // Geri vites
            if (player.speed > currentMaxSpeed) player.speed -= 0.2f;

            float gripModifier = (currentWeather == SNOWY) ? 0.65f : (currentWeather == RAINY ? 0.80f : 1.0f);
            
            float turnFactor = (player.speed / player.maxSpeed) * 4.5f * gripModifier; // F1 direksiyonu çok keskindir
            if (IsKeyDown(KEY_A)) {
                player.rotation -= turnFactor;
                if (player.speed > 5.0f) isTurningHard = true;
            }
            if (IsKeyDown(KEY_D)) {
                player.rotation += turnFactor;
                if (player.speed > 5.0f) isTurningHard = true;
            }

            float roadWidth = roadTex.width > 0 ? roadTex.width : 500; // F1 pisti daha geniştir
            float roadLeft = (screenWidth / 2.0f) - (roadWidth / 2.0f);
            float roadRight = (screenWidth / 2.0f) + (roadWidth / 2.0f);

            bool isOffRoad = false;
            // Çakıl Havuzu / Çim Etkisi
            if (player.position.x < roadLeft + 25 || player.position.x > roadRight - 25) {
                player.speed *= (currentWeather == SNOWY ? 0.92f : 0.85f); 
                isOffRoad = true;
                if (player.speed > 2.0f) player.position.x += GetRandomValue(-3, 3); // F1 aracı çimde çok titrer
            }

            Vector2 dir = Vector2Rotate((Vector2){ 0, -1 }, player.rotation * DEG2RAD);
            player.position = Vector2Add(player.position, Vector2Scale(dir, player.speed));
            
            // Rakip (Rival) Yapay Zekası
            float distToPlayer = player.position.y - rival.pos.y; 
            if (distToPlayer < -500) rival.speed = rival.maxSpeed * 0.85f; 
            else if (distToPlayer > 800) rival.speed = rival.maxSpeed * 1.35f; 
            else rival.speed = rival.maxSpeed;

            rival.pos.y -= rival.speed;

            rival.targetX = rival.pos.x;
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                if (backmarkers[i].active && backmarkers[i].pos.y < rival.pos.y && backmarkers[i].pos.y > rival.pos.y - 400) {
                    if (abs(backmarkers[i].pos.x - rival.pos.x) < 60) {
                        if (rival.pos.x > screenWidth / 2.0f) rival.targetX = rival.pos.x - 140.0f;
                        else rival.targetX = rival.pos.x + 140.0f;
                    }
                }
            }
            rival.pos.x = Lerp(rival.pos.x, rival.targetX, 0.05f * gripModifier); 
            
            if (rival.pos.x < roadLeft + 50) rival.pos.x = roadLeft + 50;
            if (rival.pos.x > roadRight - 50) rival.pos.x = roadRight - 50;

            if (CheckCollisionCircles(player.position, 25, rival.pos, 25)) {
                player.speed *= 0.7f;
                rival.speed *= 0.7f;
                if (player.position.x < rival.pos.x) { player.position.x -= 8; rival.pos.x += 8; }
                else { player.position.x += 8; rival.pos.x -= 8; }
                cameraShake = 8.0f; // F1 temasları serttir
            }

            // Hava Durumu
            if (currentWeather == RAINY || currentWeather == SNOWY) {
                for (int i = 0; i < MAX_WEATHER_PARTICLES; i++) {
                    if (!weatherParticles[i].active) {
                        weatherParticles[i].pos = (Vector2){ player.position.x + GetRandomValue(-screenWidth, screenWidth), player.position.y - GetRandomValue(400, 800) };
                        weatherParticles[i].speed = (currentWeather == RAINY) ? (float)GetRandomValue(20, 30) : (float)GetRandomValue(5, 10);
                        weatherParticles[i].size = (currentWeather == RAINY) ? (float)GetRandomValue(1, 2) : (float)GetRandomValue(2, 4);
                        weatherParticles[i].active = true;
                        break;
                    }
                }
            }
            for (int i = 0; i < MAX_WEATHER_PARTICLES; i++) {
                if (weatherParticles[i].active) {
                    weatherParticles[i].pos.y += weatherParticles[i].speed; 
                    if (currentWeather == SNOWY) weatherParticles[i].pos.x += sin(GetTime() + i) * 3.0f; 
                    if (weatherParticles[i].pos.y > player.position.y + 600) weatherParticles[i].active = false;
                }
            }

            // F1 Lastik İzleri (Daha geniş ve kalın)
            if (!isOffRoad && (isBraking || isTurningHard) && player.speed > 3.0f) {
                for (int i = 0; i < MAX_PARTICLES; i++) {
                    if (!particles[i].active) {
                        Vector2 leftTire = Vector2Add(player.position, Vector2Rotate((Vector2){-18, 20}, player.rotation * DEG2RAD));
                        Vector2 rightTire = Vector2Add(player.position, Vector2Rotate((Vector2){18, 20}, player.rotation * DEG2RAD));
                        particles[i] = (Particle){ leftTire, {0,0}, 2.0f, 2.0f, 7.0f, Fade(BLACK, 0.5f), true, true };
                        for (int j = i + 1; j < MAX_PARTICLES; j++) {
                            if (!particles[j].active) {
                                particles[j] = (Particle){ rightTire, {0,0}, 2.0f, 2.0f, 7.0f, Fade(BLACK, 0.5f), true, true };
                                break;
                            }
                        }
                        break;
                    }
                }
            }

            // Egzoz ve DRS Efekti
            if (player.speed > 2.0f && GetRandomValue(0, 100) < (player.speed * 4)) {
                for (int i = 0; i < MAX_PARTICLES; i++) {
                    if (!particles[i].active) {
                        Vector2 pPos = Vector2Add(player.position, Vector2Scale(dir, -40.0f)); 
                        // F1 Araçları kerblere çarpınca kıvılcım atar!
                        Color pColor = (isOffRoad || isBraking) ? YELLOW : (isDrafting ? SKYBLUE : Fade(WHITE,0.2f)); 
                        particles[i] = (Particle){ pPos, {(float)GetRandomValue(-10,10)/10.0f, (float)GetRandomValue(-10,10)/10.0f}, 1.0f, 1.0f, (float)GetRandomValue(3,7), pColor, true, false };
                        break;
                    }
                }
            }

            for (int i = 0; i < MAX_PARTICLES; i++) {
                if (particles[i].active) {
                    if (!particles[i].isSkidMark) {
                        particles[i].position = Vector2Add(particles[i].position, particles[i].velocity);
                        particles[i].life -= 0.08f;
                    } else particles[i].life -= 0.003f; 
                    if (particles[i].life <= 0) particles[i].active = false;
                }
            }

            float targetZoom = 1.0f - (player.speed * 0.012f);
            if (targetZoom < 0.70f) targetZoom = 0.70f;
            camera.zoom = Lerp(camera.zoom, targetZoom, 0.05f);

            camera.target = Vector2Lerp(camera.target, player.position, 0.1f);
            if (cameraShake > 0) {
                camera.offset.x = (screenWidth / 2.0f) + GetRandomValue(-(int)cameraShake, (int)cameraShake);
                camera.offset.y = (screenHeight / 2.0f) + GetRandomValue(-(int)cameraShake, (int)cameraShake);
                cameraShake *= 0.85f; 
                if (cameraShake < 0.5f) cameraShake = 0.0f;
            } else {
                camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
            }

            // Diğer F1 Pilotları (Tur yiyenler)
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                if (!backmarkers[i].active || backmarkers[i].pos.y > player.position.y + 800) {
                    backmarkers[i].pos = (Vector2){ (float)GetRandomValue(roadLeft + 60, roadRight - 60), player.position.y - GetRandomValue(800, 2500) };
                    backmarkers[i].speed = (float)GetRandomValue(4, 7); // Yavaş pilotlar
                    backmarkers[i].color = carColors[GetRandomValue(0, totalColors - 1)];
                    backmarkers[i].active = true;
                }

                if (backmarkers[i].active) backmarkers[i].pos.y -= backmarkers[i].speed; 

                // F1 Aracı Çarpışma Hesaplaması (Daha Geniş)
                if (backmarkers[i].active && CheckCollisionCircles(player.position, 22, backmarkers[i].pos, 22)) {
                    player.speed *= -0.2f; // Kaza
                    backmarkers[i].active = false;
                    player.health -= 1;
                    cameraShake = 25.0f; 
                    if (player.health <= 0) {
                        teamBudget += score; 
                        currentState = GAMEOVER;
                    }
                }
                if (backmarkers[i].active && CheckCollisionCircles(rival.pos, 22, backmarkers[i].pos, 22)) {
                    rival.speed *= 0.4f; 
                    backmarkers[i].active = false;
                }
            }

            // Sponsorluk Anlaşmaları (Kupalar)
            for (int i = 0; i < MAX_COINS; i++) {
                if (!sponsors[i].active || sponsors[i].pos.y > player.position.y + 600) {
                    sponsors[i].pos = (Vector2){ (float)GetRandomValue(roadLeft + 60, roadRight - 60), player.position.y - GetRandomValue(600, 3000) };
                    sponsors[i].active = true;
                }
                if (CheckCollisionCircles(player.position, 25, sponsors[i].pos, 15)) {
                    score += 500; 
                    player.boost += 25.0f; 
                    if (player.boost > player.maxBoost) player.boost = player.maxBoost;
                    sponsors[i].active = false;
                }
            }

            // Pıt Stop Alanları (Can Yenileme)
            for (int i = 0; i < MAX_REPAIRS; i++) {
                if (!pitStops[i].active || pitStops[i].pos.y > player.position.y + 600) {
                    pitStops[i].pos = (Vector2){ (float)GetRandomValue(roadLeft + 60, roadRight - 60), player.position.y - GetRandomValue(4000, 9000) };
                    pitStops[i].active = true;
                }
                if (CheckCollisionCircles(player.position, 25, pitStops[i].pos, 20)) {
                    if (player.health < 3) player.health++; 
                    pitStops[i].active = false;
                }
            }
        }

        // --- ÇİZİM ---
        BeginDrawing();
            ClearBackground(GetBackgroundColor(currentWeather)); 

            if (currentState == MENU) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
                DrawText("F1 GRAND PRIX", screenWidth/2 - 190, 200, 50, RED);
                DrawText(TextFormat("TAKIM BUTCESI: $%i", teamBudget), screenWidth/2 - 120, 280, 30, GREEN);
                DrawText("[ENTER] YARISA BASLA  |  [G] TAKIM GARAJI", screenWidth/2 - 250, 400, 25, WHITE);
            } 
            else if (currentState == GARAGE) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade((Color){30, 30, 40, 255}, 0.95f));
                DrawText("TAKIM GARAJI", screenWidth/2 - 130, 40, 40, GOLD);
                DrawText(TextFormat("BUTCE: $%i", teamBudget), screenWidth/2 - 90, 90, 30, GREEN);
                
                DrawText("< SOL / SAG OK > (TAKIM SEC)", screenWidth/2 - 160, screenHeight/2 + 50, 20, WHITE);
                DrawText(TextFormat("TAKIM: %s", teamNames[selectedColorIndex]), screenWidth/2 - 100, screenHeight/2 + 20, 25, carColors[selectedColorIndex]);
                
                DrawText(TextFormat("HAVA DURUMU: %s", weatherNames[currentWeather]), screenWidth/2 - 160, screenHeight/2 + 80, 20, SKYBLUE);

                int aCost = aeroLevel * 1500;
                int bCost = brakeLevel * 1200;
                int eCost = ersLevel * 1800;

                DrawText(TextFormat("[1] AERO PAKETI %i/5 - Yukselt: $%i", aeroLevel, aCost), 50, screenHeight/2 + 130, 20, aeroLevel < 5 ? (teamBudget >= aCost ? LIME : RED) : GRAY);
                DrawText(TextFormat("[2] KARBON FRENLER %i/5 - Yukselt: $%i", brakeLevel, bCost), 50, screenHeight/2 + 170, 20, brakeLevel < 5 ? (teamBudget >= bCost ? LIME : RED) : GRAY);
                DrawText(TextFormat("[3] ERS BATARYASI %i/5 - Yukselt: $%i", ersLevel, eCost), 50, screenHeight/2 + 210, 20, ersLevel < 5 ? (teamBudget >= eCost ? LIME : RED) : GRAY);

                DrawText("[ENTER] PISTE DON", screenWidth/2 - 110, screenHeight - 60, 20, LIGHTGRAY);
                
                // Garajda F1 Aracı Çizimi
                DrawFormulaCar((Vector2){screenWidth/2, screenHeight/2 - 60}, 0.0f, carColors[selectedColorIndex]);
            }
            else if (currentState == GAMEOVER) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.9f));
                if (raceFinished) {
                    DrawText(playerWon ? "GRAND PRIX KAZANILDI!" : "PODYUM KACIRILDI!", screenWidth/2 - (playerWon? 260 : 250), 180, 50, playerWon ? LIME : RED);
                    if (playerWon) DrawText("BİRİNCİLİK ODULU: +$10000", screenWidth/2 - 180, 260, 30, GOLD);
                } else {
                    DrawText("ARAC PARCALANDI!", screenWidth/2 - 220, 200, 50, RED);
                }
                DrawText(TextFormat("KAZANILAN SPONSORLUK: $%i", score - (playerWon? 10000:0)), screenWidth/2 - 200, 320, 30, GREEN);
                DrawText(TextFormat("YENI BUTCE: $%i", teamBudget), screenWidth/2 - 130, 370, 30, GOLD);
                DrawText("[ENTER] YENI YARIS", screenWidth/2 - 120, 500, 25, LIGHTGRAY);
            }
            else if (currentState == PLAYING) {
                BeginMode2D(camera);
                    DrawF1Track(grassTex, roadTex, camera.target, currentWeather); 

                    // Bitiş Çizgisi
                    float roadWidth = roadTex.width > 0 ? roadTex.width : 500; 
                    float roadLeft = (screenWidth / 2.0f) - (roadWidth / 2.0f);
                    if (raceDistance - player.position.y > -1500 && raceDistance - player.position.y < 1500) {
                        for(int w=0; w < roadWidth; w+=40) {
                            DrawRectangle(roadLeft + w, raceDistance, 20, 20, WHITE);
                            DrawRectangle(roadLeft + w + 20, raceDistance, 20, 20, BLACK);
                            DrawRectangle(roadLeft + w, raceDistance+20, 20, 20, BLACK);
                            DrawRectangle(roadLeft + w + 20, raceDistance+20, 20, 20, WHITE);
                        }
                    }

                    for (int i = 0; i < MAX_PARTICLES; i++) {
                        if (particles[i].active) {
                            if (particles[i].isSkidMark) DrawCircleV(particles[i].position, particles[i].size, Fade(particles[i].color, particles[i].life / particles[i].maxLife));
                            else DrawCircleV(particles[i].position, particles[i].size * particles[i].life, Fade(particles[i].color, particles[i].life));
                        }
                    }

                    // Pit Stop (Tekerlek İkonu)
                    float pulse = (sin(GetTime() * 5.0f) * 2.0f); 
                    for (int i = 0; i < MAX_REPAIRS; i++) {
                        if (pitStops[i].active) {
                            DrawCircle(pitStops[i].pos.x, pitStops[i].pos.y, 16 + pulse, BLACK); // Lastik
                            DrawCircle(pitStops[i].pos.x, pitStops[i].pos.y, 8, DARKGRAY); // Jant
                            DrawText("PIT", pitStops[i].pos.x - 12, pitStops[i].pos.y - 6, 15, WHITE);
                        }
                    }

                    // Sponsor Logoları (Altın Yıldızlar)
                    for (int i = 0; i < MAX_COINS; i++) {
                        if (sponsors[i].active) {
                            DrawCircle(sponsors[i].pos.x, sponsors[i].pos.y, 14 + pulse, GOLD);
                            DrawText("S", sponsors[i].pos.x - 5, sponsors[i].pos.y - 10, 20, WHITE);
                        }
                    }

                    // Tur Yiyen F1 Araçları
                    for (int i = 0; i < MAX_OBSTACLES; i++) {
                        if (backmarkers[i].active) {
                            DrawFormulaCar(backmarkers[i].pos, 0.0f, ColorBrightness(backmarkers[i].color, (currentWeather==NIGHT?-0.3f:0.0f)));
                            if (currentWeather == NIGHT || currentWeather == RAINY) {
                                DrawCircle(backmarkers[i].pos.x - 10, backmarkers[i].pos.y + 25, 4, RED); // Yağmur Lambası
                            }
                        }
                    }

                    // RAKİP F1 ARACI
                    float carBright = (currentWeather == NIGHT) ? -0.3f : (currentWeather == RAINY ? -0.1f : 0.0f);
                    DrawFormulaCar(rival.pos, 0.0f, ColorBrightness(rival.color, carBright));

                    // OYUNCU F1 ARACI
                    DrawFormulaCar(player.position, player.rotation, ColorBrightness(carColors[selectedColorIndex], carBright));

                    // F1 Farları (Gece/Yağmur)
                    if (currentWeather == NIGHT || currentWeather == RAINY) {
                        BeginBlendMode(BLEND_ADDITIVE);
                            Vector2 leftH = Vector2Add(player.position, Vector2Rotate((Vector2){-10, -25}, player.rotation * DEG2RAD));
                            Vector2 rightH = Vector2Add(player.position, Vector2Rotate((Vector2){10, -25}, player.rotation * DEG2RAD));
                            Vector2 lightDir = Vector2Rotate((Vector2){0, -500}, player.rotation * DEG2RAD);
                            DrawTriangle(leftH, Vector2Add(leftH, Vector2Add(lightDir, Vector2Rotate((Vector2){-80,0}, player.rotation*DEG2RAD))), Vector2Add(leftH, Vector2Add(lightDir, Vector2Rotate((Vector2){80,0}, player.rotation*DEG2RAD))), Fade(WHITE, 0.15f));
                            DrawTriangle(rightH, Vector2Add(rightH, Vector2Add(lightDir, Vector2Rotate((Vector2){-80,0}, player.rotation*DEG2RAD))), Vector2Add(rightH, Vector2Add(lightDir, Vector2Rotate((Vector2){80,0}, player.rotation*DEG2RAD))), Fade(WHITE, 0.15f));
                        EndBlendMode();
                    }

                    for (int i = 0; i < MAX_WEATHER_PARTICLES; i++) {
                        if (weatherParticles[i].active) {
                            if (currentWeather == RAINY) DrawLineEx(weatherParticles[i].pos, (Vector2){weatherParticles[i].pos.x, weatherParticles[i].pos.y + 25}, weatherParticles[i].size, Fade(SKYBLUE, 0.5f));
                            else if (currentWeather == SNOWY) DrawCircleV(weatherParticles[i].pos, weatherParticles[i].size, Fade(WHITE, 0.8f));
                        }
                    }

                EndMode2D();

                if (IsKeyDown(KEY_LEFT_SHIFT) && player.boost > 0 && player.speed > 5.0f) {
                    for (int i = 0; i < 25; i++) {
                        float lineX = GetRandomValue(0, screenWidth);
                        float lineY = GetRandomValue(0, screenHeight);
                        float lineLen = GetRandomValue(100, 300);
                        DrawLineEx((Vector2){lineX, lineY}, (Vector2){lineX, lineY + lineLen}, 2.0f, Fade(WHITE, 0.2f));
                    }
                }

                // F1 HUD Ekranı
                DrawRectangle(10, 10, 280, 180, Fade(BLACK, 0.8f)); 
                DrawText(TextFormat("PUAN: %i", score), 20, 20, 20, WHITE);
                DrawText(TextFormat("HIZ: %.0f KM/H", player.speed * 30), 20, 45, 25, LIME); // F1 Hız Çarpanı
                DrawText(TextFormat("ARAC: %i / 3", 3 - player.health), 20, 75, 20, (player.health == 1) ? RED : ORANGE);
                
                // DRS Bilgisi
                bool isDrafting = false;
                for (int i = 0; i < MAX_OBSTACLES; i++) {
                    if (backmarkers[i].active && backmarkers[i].pos.y < player.position.y && backmarkers[i].pos.y > player.position.y - 300 && abs(backmarkers[i].pos.x - player.position.x) < 30) isDrafting = true;
                }
                if (rival.pos.y < player.position.y && rival.pos.y > player.position.y - 350 && abs(rival.pos.x - player.position.x) < 30) isDrafting = true;
                
                if (isDrafting) DrawText("DRS AKTIF!", 20, 100, 20, GREEN);
                else DrawText("DRS BEKLEMEDE", 20, 100, 20, DARKGRAY);

                DrawText("ERS:", 20, 135, 20, SKYBLUE);
                DrawRectangle(80, 138, 160, 15, DARKGRAY);
                DrawRectangle(80, 138, (int)((player.boost / player.maxBoost) * 160.0f), 15, SKYBLUE);

                // İlerleme Çubuğu (Minimap)
                int mapX = screenWidth - 40;
                int mapY = 100;
                int mapHeight = screenHeight - 200;
                DrawRectangle(mapX, mapY, 15, mapHeight, Fade(DARKGRAY, 0.7f)); 
                DrawRectangle(mapX - 5, mapY - 10, 25, 10, WHITE); // Bitiş
                DrawRectangle(mapX - 5, mapY + mapHeight, 25, 10, RED); // Başlangıç
                
                float totalDist = 384.0f - raceDistance;
                float pProgress = (384.0f - player.position.y) / totalDist;
                float rProgress = (384.0f - rival.pos.y) / totalDist;
                
                if (pProgress > 1.0f) pProgress = 1.0f;
                if (rProgress > 1.0f) rProgress = 1.0f;

                int pMapY = mapY + mapHeight - (int)(pProgress * mapHeight);
                int rMapY = mapY + mapHeight - (int)(rProgress * mapHeight);

                DrawCircle(mapX + 7, rMapY, 10, rival.color); 
                DrawCircle(mapX + 7, pMapY, 10, carColors[selectedColorIndex]); 
                DrawCircle(mapX + 7, pMapY, 5, WHITE); 
            }
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}

// Geometrik F1 Aracı Çizim Fonksiyonu
void DrawFormulaCar(Vector2 pos, float rotation, Color carColor) {
    // Gövde (Ana şasi)
    DrawRectanglePro((Rectangle){pos.x, pos.y, 14, 55}, (Vector2){7, 27}, rotation, carColor);
    
    // Ön Kanat
    DrawRectanglePro((Rectangle){pos.x, pos.y, 38, 8}, (Vector2){19, 27}, rotation, Fade(carColor, 0.8f));
    DrawRectanglePro((Rectangle){pos.x, pos.y, 42, 3}, (Vector2){21, 27}, rotation, BLACK); // Ön kanat detayı
    
    // Arka Kanat
    DrawRectanglePro((Rectangle){pos.x, pos.y, 34, 10}, (Vector2){17, -18}, rotation, DARKGRAY);
    DrawRectanglePro((Rectangle){pos.x, pos.y, 34, 4}, (Vector2){17, -20}, rotation, BLACK); // DRS Flap
    
    // 4 Açık Tekerlek (Siyah ve kalın)
    DrawRectanglePro((Rectangle){pos.x, pos.y, 10, 18}, (Vector2){24, 15}, rotation, BLACK); // Sol Ön
    DrawRectanglePro((Rectangle){pos.x, pos.y, 10, 18}, (Vector2){-14, 15}, rotation, BLACK); // Sağ Ön
    DrawRectanglePro((Rectangle){pos.x, pos.y, 12, 20}, (Vector2){25, -8}, rotation, BLACK); // Sol Arka
    DrawRectanglePro((Rectangle){pos.x, pos.y, 12, 20}, (Vector2){-13, -8}, rotation, BLACK); // Sağ Arka
    
    // Pilot Kaskı (Ortada küçük sarı detay)
    DrawRectanglePro((Rectangle){pos.x, pos.y, 8, 8}, (Vector2){4, 0}, rotation, YELLOW);
    
    // Halo (Kaskın etrafındaki siyah koruma barı)
    DrawRectanglePro((Rectangle){pos.x, pos.y, 12, 4}, (Vector2){6, 5}, rotation, BLACK);
}

Color GetBackgroundColor(WeatherType weather) {
    switch (weather) {
        case SUNNY: return SKYBLUE;
        case SUNSET: return (Color){40, 20, 30, 255};
        case NIGHT: return BLACK;
        case RAINY: return (Color){30, 35, 40, 255};
        case SNOWY: return (Color){200, 210, 220, 255};
    }
    return BLACK;
}

// F1 Kerblerini Çizen Yeni Pist Fonksiyonu
void DrawF1Track(Texture2D grass, Texture2D road, Vector2 cameraTarget, WeatherType weather) {
    int gW = 256;
    int rH = 100; // Kerb uzunluğu için daha küçük bölüntü
    int roadWidth = 500;
    float roadX = (screenWidth / 2.0f) - (roadWidth / 2.0f);

    Color grassColor, roadColor;

    switch (weather) {
        case SUNNY:  grassColor = (Color){34, 139, 34, 255}; roadColor = (Color){70, 70, 70, 255}; break; // Daha koyu F1 asfaltı
        case SUNSET: grassColor = (Color){80, 100, 60, 255}; roadColor = (Color){80, 75, 75, 255}; break;
        case NIGHT:  grassColor = (Color){20, 40, 20, 255}; roadColor = (Color){30, 30, 30, 255}; break;
        case RAINY:  grassColor = (Color){40, 60, 40, 255}; roadColor = (Color){50, 50, 60, 255}; break;
        case SNOWY:  grassColor = (Color){230, 240, 255, 255}; roadColor = (Color){180, 190, 200, 255}; break;
        default:     grassColor = GREEN; roadColor = DARKGRAY; break;
    }

    // Çimenler
    for (int x = -2; x < 3; x++) {
        for (int y = -3; y < 4; y++) {
            float posX = (floor(cameraTarget.x / gW) + x) * gW;
            float posY = (floor(cameraTarget.y / gW) + y) * gW;
            DrawRectangle(posX, posY, gW, gW, grassColor);
        }
    }
    
    // Asfalt
    DrawRectangle(roadX, cameraTarget.y - screenHeight, roadWidth, screenHeight * 3, roadColor);

    // Kırmızı-Beyaz Kerbler (Titreşim Bantları)
    for (int y = -10; y < 10; y++) {
        float posY = (floor(cameraTarget.y / rH) + y) * rH;
        Color kerbColor = ((int)(floor(cameraTarget.y / rH) + y) % 2 == 0) ? RED : WHITE;
        
        // Kar yağarken kerblerin rengi biraz solar
        if (weather == SNOWY) kerbColor = Fade(kerbColor, 0.7f);

        DrawRectangle(roadX - 15, posY, 15, rH, kerbColor);
        DrawRectangle(roadX + roadWidth, posY, 15, rH, kerbColor);
    }
}