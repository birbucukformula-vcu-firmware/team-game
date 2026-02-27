#include "include/raylib.h"
#include "include/raymath.h"
#include "game_state.h"
#include "car.h"
#include "particles.h"
#include "weather.h"
#include "track.h"
#include "entities.h"
#include "ui.h"
#include "graphics.h"
#include <math.h>

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "F1 Grand Prix: Formula Heyecani");
    UI_Init();
    
    Texture2D grassTex = LoadTexture("resources/grass.png");
    Texture2D roadTex = LoadTexture("resources/yol.png");

    GameState_t* gameState = GameState_Create();
    Car player = Car_Create((Vector2){512, 384}, 9.0f);
    EntityManager* entities = EntityManager_Create();
    ParticleSystem* particles = ParticleSystem_Create();

    Camera2D camera = {0};
    camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- INPUT & UPDATE ---
        if (gameState->current == MENU) {
            if (IsKeyPressed(KEY_ENTER)) gameState->current = PLAYING;
            if (IsKeyPressed(KEY_G)) gameState->current = GARAGE;
        }
        else if (gameState->current == GARAGE) {
            if (IsKeyPressed(KEY_RIGHT)) gameState->selectedColorIndex = (gameState->selectedColorIndex + 1) % TOTAL_COLORS;
            if (IsKeyPressed(KEY_LEFT)) {
                gameState->selectedColorIndex--;
                if (gameState->selectedColorIndex < 0) gameState->selectedColorIndex = TOTAL_COLORS - 1;
            }
            if (IsKeyPressed(KEY_UP)) gameState->weather = (gameState->weather + 1) % 5;
            if (IsKeyPressed(KEY_DOWN)) {
                gameState->weather = (gameState->weather - 1);
                if (gameState->weather < 0) gameState->weather = 4;
            }
            if (IsKeyPressed(KEY_ENTER)) {
                // Yeni yarışa başlamadan önce reset et
                Car_Reset(&player);
                player.maxSpeed = 9.0f + (gameState->aeroLevel * 0.6f);
                player.brakingPower = 0.18f + (gameState->brakeLevel * 0.05f);
                player.maxBoost = 100.0f + (gameState->ersLevel * 20.0f);
                player.boost = player.maxBoost;
                player.maxHealth = 3 + gameState->durabilityLevel;
                player.health = player.maxHealth;
                player.position = (Vector2){480, 384};  // Oyuncu sol tarafta
                
                EntityManager_Reset(entities);
                entities->rival.maxSpeed = player.maxSpeed * 0.96f;
                entities->rival.pos = (Vector2){544, 384};  // Rakip sağ tarafta
                ParticleSystem_Reset(particles);
                GameState_Reset(gameState);
                
                gameState->current = PLAYING;
            }
            if (IsKeyPressed(KEY_M)) gameState->current = MENU;

            int aeroCost = gameState->aeroLevel * 1500;
            int brakeCost = gameState->brakeLevel * 1200;
            int ersCost = gameState->ersLevel * 1800;

            if (IsKeyPressed(KEY_ONE) && gameState->teamBudget >= aeroCost && gameState->aeroLevel < 5) {
                gameState->teamBudget -= aeroCost;
                gameState->aeroLevel++;
            }
            if (IsKeyPressed(KEY_TWO) && gameState->teamBudget >= brakeCost && gameState->brakeLevel < 5) {
                gameState->teamBudget -= brakeCost;
                gameState->brakeLevel++;
            }
            if (IsKeyPressed(KEY_THREE) && gameState->teamBudget >= ersCost && gameState->ersLevel < 5) {
                gameState->teamBudget -= ersCost;
                gameState->ersLevel++;
            }
            
            int durCost = 1500;
            if (IsKeyPressed(KEY_FOUR) && gameState->teamBudget >= durCost && gameState->durabilityLevel < 3) {
                gameState->teamBudget -= durCost;
                gameState->durabilityLevel++;
            }
        }
        else if (gameState->current == GAMEOVER) {
            if (IsKeyPressed(KEY_ENTER)) {
                Car_Reset(&player);
                player.maxSpeed = 9.0f + (gameState->aeroLevel * 0.6f);
                player.brakingPower = 0.18f + (gameState->brakeLevel * 0.05f);
                player.maxBoost = 100.0f + (gameState->ersLevel * 20.0f);
                player.boost = player.maxBoost;
                player.maxHealth = 3 + gameState->durabilityLevel;
                player.health = player.maxHealth;
                
                EntityManager_Reset(entities);
                entities->rival.maxSpeed = player.maxSpeed * 0.96f;
                ParticleSystem_Reset(particles);
                GameState_Reset(gameState);
                gameState->current = PLAYING;
            }
            if (IsKeyPressed(KEY_M)) {
                gameState->current = MENU;
            }
        }
        else if (gameState->current == PLAYING) {
            // Yarış başlangıcı - W tuşu ile başla
            if (!gameState->raceStarted) {
                if (IsKeyPressed(KEY_W)) {
                    gameState->raceStarted = true;
                }
            }
            
            // Yarış başlamadıysa oyun mantığını atla
            if (gameState->raceStarted) {

                if (player.position.y <= RACE_DISTANCE || entities->rival.pos.y <= RACE_DISTANCE) {
                    gameState->raceFinished = true;
                    gameState->playerWon = (player.position.y <= entities->rival.pos.y);
                    if (gameState->playerWon) gameState->score += 2500;
                    gameState->teamBudget += gameState->score;
                    gameState->current = GAMEOVER;
                }

            float currentMaxSpeed = player.maxSpeed;
            bool isDrafting = false;

            // DRS Check
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                if (entities->backmarkers[i].active && entities->backmarkers[i].pos.y < player.position.y &&
                    entities->backmarkers[i].pos.y > player.position.y - 300) {
                    if (fabs(entities->backmarkers[i].pos.x - player.position.x) < 30) isDrafting = true;
                }
            }
            if (entities->rival.pos.y < player.position.y && entities->rival.pos.y > player.position.y - 350 &&
                fabs(entities->rival.pos.x - player.position.x) < 30) isDrafting = true;

            if (isDrafting && player.speed > 5.0f) {
                currentMaxSpeed = player.maxSpeed * 1.35f;
                player.speed += 0.08f;
            }

            // Boost
            if (IsKeyDown(KEY_LEFT_SHIFT) && player.boost > 0 && player.speed > 1.0f) {
                currentMaxSpeed = player.maxSpeed * 1.5f;
                Car_ApplyBoost(&player, &camera.zoom);
            } else {
                Car_RechargeBoost(&player);
            }

            // Acceleration/Braking
            bool isBraking = false, isTurningHard = false;
            if (IsKeyDown(KEY_W)) Car_ApplyAcceleration(&player, currentMaxSpeed);
            else if (IsKeyDown(KEY_S)) {
                Car_ApplyBraking(&player);
                isBraking = true;
            }
            else Car_ApplyDamping(&player);

            if (player.speed < -4.0f) player.speed = -4.0f;
            if (player.speed > currentMaxSpeed) player.speed -= 0.2f;

            float gripModifier = Weather_GetGripModifier(gameState->weather);
            
            // Steering
            if (IsKeyDown(KEY_A)) {
                Car_Turn(&player, -1.0f, gripModifier);
                if (player.speed > 5.0f) isTurningHard = true;
            }
            if (IsKeyDown(KEY_D)) {
                Car_Turn(&player, 1.0f, gripModifier);
                if (player.speed > 5.0f) isTurningHard = true;
            }

            // Off-road check
            bool isOffRoad = Track_IsOffRoad(player.position);
            if (isOffRoad) {
                player.speed *= (gameState->weather == SNOWY ? 0.92f : 0.85f);
                if (player.speed > 2.0f) player.position.x += GetRandomValue(-3, 3);
            }

            // Movement
            Vector2 dir = Vector2Rotate((Vector2){0, -1}, player.rotation * DEG2RAD);
            player.position = Vector2Add(player.position, Vector2Scale(dir, player.speed));

            // Rival AI
            EntityManager_UpdateRival(entities, player.position, player.maxSpeed);
            
            // Rival steering
            entities->rival.targetX = entities->rival.pos.x;
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                if (entities->backmarkers[i].active && entities->backmarkers[i].pos.y < entities->rival.pos.y &&
                    entities->backmarkers[i].pos.y > entities->rival.pos.y - 400) {
                    if (fabs(entities->backmarkers[i].pos.x - entities->rival.pos.x) < 60) {
                        if (entities->rival.pos.x > SCREEN_WIDTH / 2.0f) entities->rival.targetX = entities->rival.pos.x - 140.0f;
                        else entities->rival.targetX = entities->rival.pos.x + 140.0f;
                    }
                }
            }
            entities->rival.pos.x = Lerp(entities->rival.pos.x, entities->rival.targetX, 0.05f * gripModifier);
            
            float roadLeft = Track_GetRoadLeft();
            float roadRight = Track_GetRoadRight();
            if (entities->rival.pos.x < roadLeft + 50) entities->rival.pos.x = roadLeft + 50;
            if (entities->rival.pos.x > roadRight - 50) entities->rival.pos.x = roadRight - 50;

            // Collision with rival
            if (CheckCollisionCircles(player.position, 25, entities->rival.pos, 25)) {
                player.speed *= 0.7f;
                entities->rival.speed *= 0.7f;
                if (player.position.x < entities->rival.pos.x) {
                    player.position.x -= 8;
                    entities->rival.pos.x += 8;
                } else {
                    player.position.x += 8;
                    entities->rival.pos.x -= 8;
                }
                gameState->cameraShake = 8.0f;
            }

            // Weather particles
            if (gameState->weather == RAINY || gameState->weather == SNOWY) {
                for (int i = 0; i < MAX_WEATHER_PARTICLES; i++) {
                    if (!particles->weatherParticles[i].active) {
                        float speed = (gameState->weather == RAINY) ? (float)GetRandomValue(20, 30) : (float)GetRandomValue(5, 10);
                        float size = (gameState->weather == RAINY) ? (float)GetRandomValue(1, 2) : (float)GetRandomValue(2, 4);
                        ParticleSystem_AddWeatherParticle(particles,
                            (Vector2){player.position.x + GetRandomValue(-SCREEN_WIDTH, SCREEN_WIDTH), player.position.y - GetRandomValue(400, 800)},
                            speed, size);
                        break;
                    }
                }
            }
            for (int i = 0; i < MAX_WEATHER_PARTICLES; i++) {
                if (particles->weatherParticles[i].active) {
                    particles->weatherParticles[i].pos.y += particles->weatherParticles[i].speed;
                    if (gameState->weather == SNOWY) particles->weatherParticles[i].pos.x += sin(GetTime() + i) * 3.0f;
                    if (particles->weatherParticles[i].pos.y > player.position.y + 600) particles->weatherParticles[i].active = false;
                }
            }

            // Skid marks
            if (!isOffRoad && (isBraking || isTurningHard) && player.speed > 3.0f) {
                Vector2 leftTire = Vector2Add(player.position, Vector2Rotate((Vector2){-18, 20}, player.rotation * DEG2RAD));
                Vector2 rightTire = Vector2Add(player.position, Vector2Rotate((Vector2){18, 20}, player.rotation * DEG2RAD));
                ParticleSystem_AddSkidMark(particles, leftTire);
                ParticleSystem_AddSkidMark(particles, rightTire);
            }

            // Exhaust
            if (player.speed > 2.0f && GetRandomValue(0, 100) < (player.speed * 4)) {
                Vector2 pPos = Vector2Add(player.position, Vector2Scale(dir, -40.0f));
                Color pColor = (isOffRoad || isBraking) ? YELLOW : (isDrafting ? SKYBLUE : Fade(WHITE, 0.2f));
                ParticleSystem_AddExhaust(particles, pPos, dir, pColor);
            }

            ParticleSystem_Update(particles);

            // Camera
            float targetZoom = 1.0f - (player.speed * 0.012f);
            if (targetZoom < 0.70f) targetZoom = 0.70f;
            camera.zoom = Lerp(camera.zoom, targetZoom, 0.05f);
            camera.target = Vector2Lerp(camera.target, player.position, 0.1f);

            if (gameState->cameraShake > 0) {
                camera.offset.x = (SCREEN_WIDTH / 2.0f) + GetRandomValue(-(int)gameState->cameraShake, (int)gameState->cameraShake);
                camera.offset.y = (SCREEN_HEIGHT / 2.0f) + GetRandomValue(-(int)gameState->cameraShake, (int)gameState->cameraShake);
                gameState->cameraShake *= 0.85f;
                if (gameState->cameraShake < 0.5f) gameState->cameraShake = 0.0f;
            } else {
                camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
            }

            // Entities
            EntityManager_UpdateBackmarkers(entities, player.position, roadLeft, roadRight);
            EntityManager_UpdateSponsors(entities, player.position, roadLeft, roadRight);
            EntityManager_UpdatePitStops(entities, player.position, roadLeft, roadRight);

            // Backmarker collisions
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                if (entities->backmarkers[i].active && CheckCollisionCircles(player.position, 22, entities->backmarkers[i].pos, 22)) {
                    player.speed *= -0.2f;
                    entities->backmarkers[i].active = false;
                    Car_TakeDamage(&player);
                    gameState->cameraShake = 25.0f;
                    if (player.health <= 0) {
                        gameState->teamBudget += gameState->score;
                        gameState->current = GAMEOVER;
                    }
                }
                if (entities->backmarkers[i].active && CheckCollisionCircles(entities->rival.pos, 22, entities->backmarkers[i].pos, 22)) {
                    entities->rival.speed *= 0.4f;
                    entities->backmarkers[i].active = false;
                }
            }

            // Sponsor pickups
            for (int i = 0; i < MAX_COINS; i++) {
                if (entities->sponsors[i].active && CheckCollisionCircles(player.position, 25, entities->sponsors[i].pos, 15)) {
                    gameState->score += 5000;
                    player.boost += 25.0f;
                    if (player.boost > player.maxBoost) player.boost = player.maxBoost;
                    entities->sponsors[i].active = false;
                }
            }

            // Pit stop pickups
            for (int i = 0; i < MAX_REPAIRS; i++) {
                if (entities->pitStops[i].active && CheckCollisionCircles(player.position, 25, entities->pitStops[i].pos, 20)) {
                    Car_Heal(&player);
                    entities->pitStops[i].active = false;
                }
            }
            }
        }

        // --- DRAWING ---
        BeginDrawing();
            ClearBackground(Weather_GetBackgroundColor(gameState->weather));

            if (gameState->current == MENU) {
                UI_DrawMenu(SCREEN_WIDTH, SCREEN_HEIGHT, gameState->teamBudget);
            }
            else if (gameState->current == GARAGE) {
                UI_DrawGarage(SCREEN_WIDTH, SCREEN_HEIGHT, gameState->teamBudget, gameState->selectedColorIndex,
                    gameState->weather, gameState->aeroLevel, gameState->brakeLevel, gameState->ersLevel, gameState->durabilityLevel);
                Graphics_DrawFormulaCar((Vector2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f - 60}, 0.0f, carColors[gameState->selectedColorIndex]);
            }
            else if (gameState->current == GAMEOVER) {
                UI_DrawGameOver(SCREEN_WIDTH, SCREEN_HEIGHT, gameState->playerWon, gameState->score, gameState->teamBudget, gameState->raceFinished);
            }
            else if (gameState->current == PLAYING) {
                BeginMode2D(camera);
                    Track_Draw(grassTex, roadTex, camera.target, gameState->weather);
                    Track_DrawFinishLine(camera.target, RACE_DISTANCE);

                    ParticleSystem_Draw(particles);

                    // Pit stops
                    float pulse = sin(GetTime() * 5.0f) * 2.0f;
                    for (int i = 0; i < MAX_REPAIRS; i++) {
                        if (entities->pitStops[i].active) {
                            DrawCircle(entities->pitStops[i].pos.x, entities->pitStops[i].pos.y, 16 + pulse, BLACK);
                            DrawCircle(entities->pitStops[i].pos.x, entities->pitStops[i].pos.y, 8, DARKGRAY);
                            DrawText("PIT", entities->pitStops[i].pos.x - 12, entities->pitStops[i].pos.y - 6, 15, WHITE);
                        }
                    }

                    // Sponsors
                    for (int i = 0; i < MAX_COINS; i++) {
                        if (entities->sponsors[i].active) {
                            DrawCircle(entities->sponsors[i].pos.x, entities->sponsors[i].pos.y, 14 + pulse, GREEN);
                            DrawText("S", entities->sponsors[i].pos.x - 5, entities->sponsors[i].pos.y - 10, 20, WHITE);
                        }
                    }

                    // Backmarkers
                    for (int i = 0; i < MAX_OBSTACLES; i++) {
                        if (entities->backmarkers[i].active) {
                            float carBright = (gameState->weather == NIGHT) ? -0.3f : (gameState->weather == RAINY ? -0.1f : 0.0f);
                            Graphics_DrawFormulaCar(entities->backmarkers[i].pos, 0.0f, ColorBrightness(entities->backmarkers[i].color, carBright));
                            if (gameState->weather == NIGHT || gameState->weather == RAINY) {
                                DrawCircle(entities->backmarkers[i].pos.x - 10, entities->backmarkers[i].pos.y + 25, 4, RED);
                            }
                        }
                    }

                    // Rival
                    float carBright = (gameState->weather == NIGHT) ? -0.3f : (gameState->weather == RAINY ? -0.1f : 0.0f);
                    Graphics_DrawFormulaCar(entities->rival.pos, 0.0f, ColorBrightness(entities->rival.color, carBright));

                    // Player
                    Graphics_DrawFormulaCar(player.position, player.rotation, ColorBrightness(carColors[gameState->selectedColorIndex], carBright));
                    Graphics_DrawHeadlights(player.position, player.rotation, gameState->weather);

                    ParticleSystem_DrawWeather(particles, gameState->weather);

                EndMode2D();

                if (IsKeyDown(KEY_LEFT_SHIFT) && player.boost > 0 && player.speed > 5.0f) {
                    for (int i = 0; i < 25; i++) {
                        float lineX = GetRandomValue(0, SCREEN_WIDTH);
                        float lineY = GetRandomValue(0, SCREEN_HEIGHT);
                        float lineLen = GetRandomValue(100, 300);
                        DrawLineEx((Vector2){lineX, lineY}, (Vector2){lineX, lineY + lineLen}, 2.0f, Fade(WHITE, 0.2f));
                    }
                }

                bool isDrafting = false;
                for (int i = 0; i < MAX_OBSTACLES; i++) {
                    if (entities->backmarkers[i].active && entities->backmarkers[i].pos.y < player.position.y &&
                        entities->backmarkers[i].pos.y > player.position.y - 300 && fabs(entities->backmarkers[i].pos.x - player.position.x) < 30) {
                        isDrafting = true;
                    }
                }
                if (entities->rival.pos.y < player.position.y && entities->rival.pos.y > player.position.y - 350 &&
                    fabs(entities->rival.pos.x - player.position.x) < 30) isDrafting = true;

                UI_DrawHUD(SCREEN_WIDTH, SCREEN_HEIGHT, gameState->score, player.speed, player.health, player.boost, player.maxBoost, isDrafting);

                float totalDist = 384.0f - RACE_DISTANCE;
                float pProgress = (384.0f - player.position.y) / totalDist;
                float rProgress = (384.0f - entities->rival.pos.y) / totalDist;
                if (pProgress > 1.0f) pProgress = 1.0f;
                if (rProgress > 1.0f) rProgress = 1.0f;

                UI_DrawMinimap(SCREEN_WIDTH, SCREEN_HEIGHT, pProgress, rProgress, entities->rival.color, carColors[gameState->selectedColorIndex]);
                
                // Yarış başlangıcı yazısı
                if (!gameState->raceStarted) {
                    UI_DrawRaceStart(SCREEN_WIDTH, SCREEN_HEIGHT);
                }
            }
        EndDrawing();
    }

    ParticleSystem_Destroy(particles);
    EntityManager_Destroy(entities);
    GameState_Destroy(gameState);
    CloseWindow();
    return 0;
}
