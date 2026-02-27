#include "track.h"
#include <math.h>

float Track_GetRoadLeft(void) {
    return (SCREEN_WIDTH / 2.0f) - (ROAD_WIDTH / 2.0f);
}

float Track_GetRoadRight(void) {
    return (SCREEN_WIDTH / 2.0f) + (ROAD_WIDTH / 2.0f);
}

bool Track_IsOffRoad(Vector2 pos) {
    float roadLeft = Track_GetRoadLeft();
    float roadRight = Track_GetRoadRight();
    return (pos.x < roadLeft + 25 || pos.x > roadRight - 25);
}

void Track_Draw(Texture2D grass, Texture2D road, Vector2 cameraTarget, int weatherType) {
    int gW = 256;
    int rH = 100;
    float roadX = Track_GetRoadLeft();

    Color grassColor, roadColor;
    switch (weatherType) {
        case 0: grassColor = (Color){34, 139, 34, 255}; roadColor = (Color){70, 70, 70, 255}; break;
        case 1: grassColor = (Color){80, 100, 60, 255}; roadColor = (Color){80, 75, 75, 255}; break;
        case 2: grassColor = (Color){20, 40, 20, 255}; roadColor = (Color){30, 30, 30, 255}; break;
        case 3: grassColor = (Color){40, 60, 40, 255}; roadColor = (Color){50, 50, 60, 255}; break;
        case 4: grassColor = (Color){230, 240, 255, 255}; roadColor = (Color){180, 190, 200, 255}; break;
        default: grassColor = GREEN; roadColor = DARKGRAY; break;
    }

    for (int x = -2; x < 3; x++) {
        for (int y = -3; y < 4; y++) {
            float posX = (floor(cameraTarget.x / gW) + x) * gW;
            float posY = (floor(cameraTarget.y / gW) + y) * gW;
            DrawRectangle(posX, posY, gW, gW, grassColor);
        }
    }

    DrawRectangle(roadX, cameraTarget.y - SCREEN_HEIGHT, ROAD_WIDTH, SCREEN_HEIGHT * 3, roadColor);

    for (int y = -10; y < 10; y++) {
        float posY = (floor(cameraTarget.y / rH) + y) * rH;
        Color kerbColor = ((int)(floor(cameraTarget.y / rH) + y) % 2 == 0) ? RED : WHITE;
        if (weatherType == 4) kerbColor = Fade(kerbColor, 0.7f);
        DrawRectangle(roadX - 15, posY, 15, rH, kerbColor);
        DrawRectangle(roadX + ROAD_WIDTH, posY, 15, rH, kerbColor);
    }
}

void Track_DrawFinishLine(Vector2 cameraTarget, float raceDistance) {
    float roadX = Track_GetRoadLeft();
    if (raceDistance - cameraTarget.y > -1500 && raceDistance - cameraTarget.y < 1500) {
        for (int w = 0; w < ROAD_WIDTH; w += 40) {
            DrawRectangle(roadX + w, raceDistance, 20, 20, WHITE);
            DrawRectangle(roadX + w + 20, raceDistance, 20, 20, BLACK);
            DrawRectangle(roadX + w, raceDistance + 20, 20, 20, BLACK);
            DrawRectangle(roadX + w + 20, raceDistance + 20, 20, 20, WHITE);
        }
    }
}
