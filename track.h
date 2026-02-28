#ifndef TRACK_H
#define TRACK_H

#include "external/raylib/include/raylib.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define RACE_DISTANCE -30000.0f
#define ROAD_WIDTH 500

void Track_Draw(Texture2D grass, Texture2D road, Vector2 cameraTarget, int weatherType);
void Track_DrawFinishLine(Vector2 cameraTarget, float raceDistance);
float Track_GetRoadLeft(void);
float Track_GetRoadRight(void);
bool Track_IsOffRoad(Vector2 pos);

#endif
