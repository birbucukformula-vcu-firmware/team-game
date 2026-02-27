#ifndef UI_H
#define UI_H

#include "include/raylib.h"

#define TOTAL_COLORS 6

extern Color carColors[TOTAL_COLORS];
extern const char* teamNames[TOTAL_COLORS];
extern const char* weatherNames[5];

void UI_Init(void);
void UI_DrawMenu(int screenWidth, int screenHeight, int teamBudget);
void UI_DrawGarage(int screenWidth, int screenHeight, int teamBudget, int selectedColor, int weather,
                   int aeroLevel, int brakeLevel, int ersLevel, int durabilityLevel);
void UI_DrawGameOver(int screenWidth, int screenHeight, bool playerWon, int score, int teamBudget, bool raceFinished);
void UI_DrawHUD(int screenWidth, int screenHeight, int score, float speed, int health, float boost, float maxBoost, bool isDrafting);
void UI_DrawMinimap(int screenWidth, int screenHeight, float playerProgress, float rivalProgress, Color rivalColor, Color playerColor);
void UI_DrawRaceStart(int screenWidth, int screenHeight);

#endif
