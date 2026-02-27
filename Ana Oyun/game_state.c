#include "game_state.h"
#include <stdlib.h>

GameState_t* GameState_Create(void) {
    GameState_t* state = (GameState_t*)malloc(sizeof(GameState_t));
    state->current = MENU;
    state->weather = SUNNY;
    state->score = 0;
    state->teamBudget = 0;
    state->aeroLevel = 1;
    state->brakeLevel = 1;
    state->ersLevel = 1;
    state->durabilityLevel = 0;
    state->selectedColorIndex = 0;
    state->playerWon = false;
    state->raceFinished = false;
    state->raceStarted = false;
    state->cameraShake = 0.0f;
    return state;
}

void GameState_Destroy(GameState_t* state) {
    if (state) free(state);
}

void GameState_Reset(GameState_t* state) {
    state->score = 0;
    state->playerWon = false;
    state->raceFinished = false;
    state->raceStarted = false;
    state->cameraShake = 0.0f;
}
