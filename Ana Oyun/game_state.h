#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "weather.h"

typedef enum GameState { MENU, GARAGE, PLAYING, GAMEOVER } GameState;

typedef struct {
    GameState current;
    WeatherType weather;
    int score;
    int teamBudget;
    int aeroLevel;
    int brakeLevel;
    int ersLevel;
    int durabilityLevel;
    int selectedColorIndex;
    bool playerWon;
    bool raceFinished;
    bool raceStarted;
    float cameraShake;
} GameState_t;

GameState_t* GameState_Create(void);
void GameState_Destroy(GameState_t* state);
void GameState_Reset(GameState_t* state);

#endif
