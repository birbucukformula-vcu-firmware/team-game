#ifndef ENTITIES_H
#define ENTITIES_H

#include "external/raylib/include/raylib.h"

#define MAX_OBSTACLES 10
#define MAX_COINS 10
#define MAX_REPAIRS 2

typedef struct {
    Vector2 pos;
    bool active;
    float speed;
    float maxSpeed;
    float targetX;
    Color color;
} Rival;

typedef struct {
    Vector2 pos;
    bool active;
    float speed;
    Color color;
} Obstacle;

typedef struct {
    Vector2 pos;
    bool active;
} Entity;

typedef struct {
    Rival rival;
    Obstacle backmarkers[MAX_OBSTACLES];
    Entity sponsors[MAX_COINS];
    Entity pitStops[MAX_REPAIRS];
} EntityManager;

EntityManager* EntityManager_Create(void);
void EntityManager_Destroy(EntityManager* em);
void EntityManager_UpdateRival(EntityManager* em, Vector2 playerPos, float playerMaxSpeed);
void EntityManager_UpdateBackmarkers(EntityManager* em, Vector2 playerPos, float roadLeft, float roadRight);
void EntityManager_UpdateSponsors(EntityManager* em, Vector2 playerPos, float roadLeft, float roadRight);
void EntityManager_UpdatePitStops(EntityManager* em, Vector2 playerPos, float roadLeft, float roadRight);
void EntityManager_Reset(EntityManager* em);

#endif
