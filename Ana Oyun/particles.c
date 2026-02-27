#include "particles.h"
#include <stdlib.h>
#include <math.h>
#include "include/raymath.h"

ParticleSystem* ParticleSystem_Create(void) {
    ParticleSystem* ps = (ParticleSystem*)malloc(sizeof(ParticleSystem));
    for (int i = 0; i < MAX_PARTICLES; i++) ps->particles[i].active = false;
    for (int i = 0; i < MAX_WEATHER_PARTICLES; i++) ps->weatherParticles[i].active = false;
    return ps;
}

void ParticleSystem_Destroy(ParticleSystem* ps) {
    if (ps) free(ps);
}

void ParticleSystem_AddSkidMark(ParticleSystem* ps, Vector2 pos) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!ps->particles[i].active) {
            ps->particles[i] = (Particle){
                .position = pos,
                .velocity = {0, 0},
                .life = 2.0f,
                .maxLife = 2.0f,
                .size = 7.0f,
                .color = Fade(BLACK, 0.5f),
                .active = true,
                .isSkidMark = true
            };
            break;
        }
    }
}

void ParticleSystem_AddExhaust(ParticleSystem* ps, Vector2 pos, Vector2 dir, Color color) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!ps->particles[i].active) {
            ps->particles[i] = (Particle){
                .position = pos,
                .velocity = {(float)GetRandomValue(-10, 10) / 10.0f, (float)GetRandomValue(-10, 10) / 10.0f},
                .life = 1.0f,
                .maxLife = 1.0f,
                .size = (float)GetRandomValue(3, 7),
                .color = color,
                .active = true,
                .isSkidMark = false
            };
            break;
        }
    }
}

void ParticleSystem_AddWeatherParticle(ParticleSystem* ps, Vector2 pos, float speed, float size) {
    for (int i = 0; i < MAX_WEATHER_PARTICLES; i++) {
        if (!ps->weatherParticles[i].active) {
            ps->weatherParticles[i] = (WeatherParticle){
                .pos = pos,
                .speed = speed,
                .size = size,
                .active = true
            };
            break;
        }
    }
}

void ParticleSystem_Update(ParticleSystem* ps) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (ps->particles[i].active) {
            if (!ps->particles[i].isSkidMark) {
                ps->particles[i].position = Vector2Add(ps->particles[i].position, ps->particles[i].velocity);
                ps->particles[i].life -= 0.08f;
            } else {
                ps->particles[i].life -= 0.003f;
            }
            if (ps->particles[i].life <= 0) ps->particles[i].active = false;
        }
    }
}

void ParticleSystem_Draw(ParticleSystem* ps) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (ps->particles[i].active) {
            if (ps->particles[i].isSkidMark) {
                DrawCircleV(ps->particles[i].position, ps->particles[i].size,
                    Fade(ps->particles[i].color, ps->particles[i].life / ps->particles[i].maxLife));
            } else {
                DrawCircleV(ps->particles[i].position, ps->particles[i].size * ps->particles[i].life,
                    Fade(ps->particles[i].color, ps->particles[i].life));
            }
        }
    }
}

void ParticleSystem_DrawWeather(ParticleSystem* ps, int weatherType) {
    for (int i = 0; i < MAX_WEATHER_PARTICLES; i++) {
        if (ps->weatherParticles[i].active) {
            if (weatherType == 3) { // RAINY
                DrawLineEx(ps->weatherParticles[i].pos,
                    (Vector2){ps->weatherParticles[i].pos.x, ps->weatherParticles[i].pos.y + 25},
                    ps->weatherParticles[i].size, Fade(SKYBLUE, 0.5f));
            } else if (weatherType == 4) { // SNOWY
                DrawCircleV(ps->weatherParticles[i].pos, ps->weatherParticles[i].size, Fade(WHITE, 0.8f));
            }
        }
    }
}

void ParticleSystem_Reset(ParticleSystem* ps) {
    for (int i = 0; i < MAX_PARTICLES; i++) ps->particles[i].active = false;
    for (int i = 0; i < MAX_WEATHER_PARTICLES; i++) ps->weatherParticles[i].active = false;
}
