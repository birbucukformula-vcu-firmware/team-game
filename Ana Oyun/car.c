#include "car.h"
#include <math.h>
#include "include/raymath.h"

Car Car_Create(Vector2 pos, float maxSpeed) {
    return (Car){
        .position = pos,
        .speed = 0.0f,
        .rotation = 0.0f,
        .maxSpeed = maxSpeed,
        .acceleration = 0.08f,
        .brakingPower = 0.18f,
        .health = 3,
        .maxHealth = 3,
        .boost = 100.0f,
        .maxBoost = 100.0f
    };
}

void Car_ApplyAcceleration(Car* car, float currentMaxSpeed) {
    car->speed += car->acceleration;
    if (car->speed > currentMaxSpeed) car->speed -= 0.2f;
}

void Car_ApplyBraking(Car* car) {
    car->speed -= car->brakingPower;
}

void Car_ApplyDamping(Car* car) {
    car->speed *= 0.985f;
}

void Car_Turn(Car* car, float direction, float gripModifier) {
    float turnFactor = (car->speed / car->maxSpeed) * 4.5f * gripModifier;
    car->rotation += direction * turnFactor;
}

void Car_ApplyBoost(Car* car, float* cameraZoom) {
    if (car->boost > 0 && car->speed > 1.0f) {
        car->speed += car->acceleration * 2.5f;
        car->boost -= 0.6f;
        *cameraZoom = Lerp(*cameraZoom, 0.88f, 0.05f);
    }
}

void Car_RechargeBoost(Car* car) {
    if (car->boost < car->maxBoost) car->boost += 0.05f;
}

void Car_TakeDamage(Car* car) {
    car->health--;
}

void Car_Heal(Car* car) {
    if (car->health < car->maxHealth) car->health++;
}

void Car_Reset(Car* car) {
    car->position = (Vector2){512, 384};
    car->speed = 0.0f;
    car->rotation = 0.0f;
    car->health = car->maxHealth;
    car->boost = car->maxBoost;
}

void Car_Update(Car* car, float maxSpeedModifier, float gripModifier, bool isOffRoad) {
    if (isOffRoad) {
        car->speed *= 0.85f;
        if (car->speed > 2.0f) car->position.x += GetRandomValue(-3, 3);
    }
}
