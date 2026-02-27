#ifndef PARTICLES_H
#define PARTICLES_H

#include "include/raylib.h"

#define MAX_PARTICLES 150
#define MAX_WEATHER_PARTICLES 150

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

typedef struct {
    Particle particles[MAX_PARTICLES];
    WeatherParticle weatherParticles[MAX_WEATHER_PARTICLES];
} ParticleSystem;

ParticleSystem* ParticleSystem_Create(void);
void ParticleSystem_Destroy(ParticleSystem* ps);
void ParticleSystem_AddSkidMark(ParticleSystem* ps, Vector2 pos);
void ParticleSystem_AddExhaust(ParticleSystem* ps, Vector2 pos, Vector2 dir, Color color);
void ParticleSystem_AddWeatherParticle(ParticleSystem* ps, Vector2 pos, float speed, float size);
void ParticleSystem_Update(ParticleSystem* ps);
void ParticleSystem_Draw(ParticleSystem* ps);
void ParticleSystem_DrawWeather(ParticleSystem* ps, int weatherType);
void ParticleSystem_Reset(ParticleSystem* ps);

#endif
