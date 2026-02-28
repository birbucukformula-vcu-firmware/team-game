#ifndef WEATHER_H
#define WEATHER_H

#include "external/raylib/include/raylib.h"

typedef enum { SUNNY, SUNSET, NIGHT, RAINY, SNOWY } WeatherType;

Color Weather_GetBackgroundColor(WeatherType weather);
float Weather_GetGripModifier(WeatherType weather);
const char* Weather_GetName(WeatherType weather);
void Weather_UpdateParticles(WeatherType weather, Vector2 playerPos, int screenWidth, int screenHeight);

#endif
