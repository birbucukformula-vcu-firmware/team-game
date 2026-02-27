#include "ui.h"

Color carColors[TOTAL_COLORS];
const char* teamNames[] = { "FERRARI", "ALPINE", "MERCEDES", "ASTON MARTIN", "MCLAREN", "RED BULL" };
const char* weatherNames[] = { "GUNESLI", "GUN BATIMI", "GECE YARISI", "YAGMURLU", "KARLI" };

void UI_Init(void) {
    carColors[0] = RED;
    carColors[1] = (Color){0, 121, 241, 255};
    carColors[2] = DARKGRAY;
    carColors[3] = GREEN;
    carColors[4] = ORANGE;
    carColors[5] = PURPLE;
}

void UI_DrawMenu(int screenWidth, int screenHeight, int teamBudget) {
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
    DrawText("F1 GRAND PRIX", screenWidth/2 - 190, 200, 50, RED);
    DrawText(TextFormat("TAKIM BUTCESI: $%i", teamBudget), screenWidth/2 - 120, 280, 30, GREEN);
    DrawText("[ENTER] YARISA BASLA  |  [G] TAKIM GARAJI", screenWidth/2 - 250, 400, 25, WHITE);
}

void UI_DrawGarage(int screenWidth, int screenHeight, int teamBudget, int selectedColor, int weather,
                   int aeroLevel, int brakeLevel, int ersLevel, int durabilityLevel) {
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade((Color){30, 30, 40, 255}, 0.95f));
    DrawText("TAKIM GARAJI", screenWidth/2 - 130, 40, 40, GOLD);
    DrawText(TextFormat("BUTCE: $%i", teamBudget), screenWidth/2 - 90, 90, 30, GREEN);
    
    DrawText("< SOL / SAG OK > (TAKIM SEC)", screenWidth/2 - 160, screenHeight/2 + 50, 20, WHITE);
    DrawText(TextFormat("TAKIM: %s", teamNames[selectedColor]), screenWidth/2 - 100, screenHeight/2 + 20, 25, carColors[selectedColor]);
    DrawText(TextFormat("HAVA DURUMU: %s", weatherNames[weather]), screenWidth/2 - 160, screenHeight/2 + 80, 20, SKYBLUE);

    int aCost = aeroLevel * 1500;
    int bCost = brakeLevel * 1200;
    int eCost = ersLevel * 1800;
    int durCost = 1500;

    DrawText(TextFormat("[1] AERO PAKETI %i/5 - Yukselt: $%i", aeroLevel, aCost), 50, screenHeight/2 + 130, 20,
        aeroLevel < 5 ? (teamBudget >= aCost ? LIME : RED) : GRAY);
    DrawText(TextFormat("[2] KARBON FRENLER %i/5 - Yukselt: $%i", brakeLevel, bCost), 50, screenHeight/2 + 170, 20,
        brakeLevel < 5 ? (teamBudget >= bCost ? LIME : RED) : GRAY);
    DrawText(TextFormat("[3] ERS BATARYASI %i/5 - Yukselt: $%i", ersLevel, eCost), 50, screenHeight/2 + 210, 20,
        ersLevel < 5 ? (teamBudget >= eCost ? LIME : RED) : GRAY);
    DrawText(TextFormat("[4] ARAC DAYANIKLILIGI %i/3 - Yukselt: $%i", durabilityLevel, durCost), 50, screenHeight/2 + 250, 20,
        durabilityLevel < 3 ? (teamBudget >= durCost ? LIME : RED) : GRAY);

    DrawText("[ENTER] PISTE DON  |  [M] ANA MENU", screenWidth/2 - 170, screenHeight - 60, 20, LIGHTGRAY);
}

void UI_DrawGameOver(int screenWidth, int screenHeight, bool playerWon, int score, int teamBudget, bool raceFinished) {
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.9f));
    if (raceFinished) {
        DrawText(playerWon ? "GRAND PRIX KAZANILDI!" : "PODYUM KACIRILDI!", 
            screenWidth/2 - (playerWon? 260 : 250), 180, 50, playerWon ? LIME : RED);
        if (playerWon) DrawText("BİRİNCİLİK ODULU: +$10000", screenWidth/2 - 180, 260, 30, GOLD);
    } else {
        DrawText("ARAC PARCALANDI!", screenWidth/2 - 220, 200, 50, RED);
    }
    DrawText(TextFormat("KAZANILAN SPONSORLUK: $%i", score - (playerWon? 10000:0)), screenWidth/2 - 200, 320, 30, GREEN);
    DrawText(TextFormat("YENI BUTCE: $%i", teamBudget), screenWidth/2 - 130, 370, 30, GOLD);
    DrawText("[ENTER] YENI YARIS", screenWidth/2 - 120, 500, 25, LIGHTGRAY);
    DrawText("[M] ANA MENU", screenWidth/2 - 80, 540, 25, LIGHTGRAY);
}

void UI_DrawHUD(int screenWidth, int screenHeight, int score, float speed, int health, float boost, float maxBoost, bool isDrafting) {
    DrawRectangle(10, 10, 280, 180, Fade(BLACK, 0.8f));
    DrawText(TextFormat("PUAN: %i", score), 20, 20, 20, WHITE);
    DrawText(TextFormat("HIZ: %.0f KM/H", speed * 30), 20, 45, 25, LIME);
    DrawText(TextFormat("ARAC: %i", health), 20, 75, 20, (health == 1) ? RED : ORANGE);
    DrawText(isDrafting ? "DRS AKTIF!" : "DRS BEKLEMEDE", 20, 100, 20, isDrafting ? GREEN : DARKGRAY);
    
    DrawText("ERS:", 20, 135, 20, SKYBLUE);
    DrawRectangle(80, 138, 160, 15, DARKGRAY);
    DrawRectangle(80, 138, (int)((boost / maxBoost) * 160.0f), 15, SKYBLUE);
}

void UI_DrawMinimap(int screenWidth, int screenHeight, float playerProgress, float rivalProgress, Color rivalColor, Color playerColor) {
    int mapX = screenWidth - 40;
    int mapY = 100;
    int mapHeight = screenHeight - 200;
    
    DrawRectangle(mapX, mapY, 15, mapHeight, Fade(DARKGRAY, 0.7f));
    DrawRectangle(mapX - 5, mapY - 10, 25, 10, WHITE);
    DrawRectangle(mapX - 5, mapY + mapHeight, 25, 10, RED);
    
    int pMapY = mapY + mapHeight - (int)(playerProgress * mapHeight);
    int rMapY = mapY + mapHeight - (int)(rivalProgress * mapHeight);
    
    DrawCircle(mapX + 7, rMapY, 10, rivalColor);
    DrawCircle(mapX + 7, pMapY, 10, playerColor);
    DrawCircle(mapX + 7, pMapY, 5, WHITE);
}

void UI_DrawRaceStart(int screenWidth, int screenHeight) {
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
    DrawText("YARISA BASLAMAK ICIN", screenWidth/2 - 200, screenHeight/2 - 50, 40, WHITE);
    DrawText("[W] TUSUNA BAS", screenWidth/2 - 150, screenHeight/2 + 20, 40, LIME);
}
