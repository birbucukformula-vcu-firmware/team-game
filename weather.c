#include "weather.h"

Color Weather_GetBackgroundColor(WeatherType weather) {
    switch (weather) {
        case SUNNY: return SKYBLUE;
        case SUNSET: return (Color){40, 20, 30, 255};
        case NIGHT: return BLACK;
        case RAINY: return (Color){30, 35, 40, 255};
        case SNOWY: return (Color){200, 210, 220, 255};
        default: return BLACK;
    }
}

float Weather_GetGripModifier(WeatherType weather) {
    switch (weather) {
        case SNOWY: return 0.65f;
        case RAINY: return 0.80f;
        default: return 1.0f;
    }
}

const char* Weather_GetName(WeatherType weather) {
    const char* names[] = { "GUNESLI", "GUN BATIMI", "GECE YARISI", "YAGMURLU", "KARLI" };
    return names[weather];
}
