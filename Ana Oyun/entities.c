#include "entities.h"
#include <stdlib.h>
#include <math.h>

EntityManager* EntityManager_Create(void) {
    EntityManager* em = (EntityManager*)malloc(sizeof(EntityManager));
    em->rival = (Rival){{480, 384}, true, 0.0f, 8.8f, 480.0f, DARKBLUE};
    for (int i = 0; i < MAX_OBSTACLES; i++) em->backmarkers[i].active = false;
    for (int i = 0; i < MAX_COINS; i++) em->sponsors[i].active = false;
    for (int i = 0; i < MAX_REPAIRS; i++) em->pitStops[i].active = false;
    return em;
}

void EntityManager_Destroy(EntityManager* em) {
    if (em) free(em);
}

void EntityManager_UpdateRival(EntityManager* em, Vector2 playerPos, float playerMaxSpeed) {
    float distToPlayer = playerPos.y - em->rival.pos.y;
    if (distToPlayer < -500) em->rival.speed = playerMaxSpeed * 0.85f;
    else if (distToPlayer > 800) em->rival.speed = playerMaxSpeed * 1.35f;
    else em->rival.speed = playerMaxSpeed;
    em->rival.pos.y -= em->rival.speed;
}

void EntityManager_UpdateBackmarkers(EntityManager* em, Vector2 playerPos, float roadLeft, float roadRight) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!em->backmarkers[i].active || em->backmarkers[i].pos.y > playerPos.y + 800) {
            em->backmarkers[i].pos = (Vector2){
                (float)GetRandomValue(roadLeft + 60, roadRight - 60),
                playerPos.y - GetRandomValue(800, 2500)
            };
            em->backmarkers[i].speed = (float)GetRandomValue(4, 7);
            em->backmarkers[i].active = true;
        }
        if (em->backmarkers[i].active) em->backmarkers[i].pos.y -= em->backmarkers[i].speed;
    }
}

void EntityManager_UpdateSponsors(EntityManager* em, Vector2 playerPos, float roadLeft, float roadRight) {
    for (int i = 0; i < MAX_COINS; i++) {
        if (!em->sponsors[i].active || em->sponsors[i].pos.y > playerPos.y + 600) {
            em->sponsors[i].pos = (Vector2){
                (float)GetRandomValue(roadLeft + 60, roadRight - 60),
                playerPos.y - GetRandomValue(600, 3000)
            };
            em->sponsors[i].active = true;
        }
    }
}

void EntityManager_UpdatePitStops(EntityManager* em, Vector2 playerPos, float roadLeft, float roadRight) {
    for (int i = 0; i < MAX_REPAIRS; i++) {
        if (!em->pitStops[i].active || em->pitStops[i].pos.y > playerPos.y + 600) {
            em->pitStops[i].pos = (Vector2){
                (float)GetRandomValue(roadLeft + 60, roadRight - 60),
                playerPos.y - GetRandomValue(4000, 9000)
            };
            em->pitStops[i].active = true;
        }
    }
}

void EntityManager_Reset(EntityManager* em) {
    em->rival.pos = (Vector2){480, 384};
    em->rival.speed = 0.0f;
    em->rival.targetX = 480.0f;
    for (int i = 0; i < MAX_OBSTACLES; i++) em->backmarkers[i].active = false;
    for (int i = 0; i < MAX_COINS; i++) em->sponsors[i].active = false;
    for (int i = 0; i < MAX_REPAIRS; i++) em->pitStops[i].active = false;
}
