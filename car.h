#ifndef CAR_H
#define CAR_H

#include "include/raylib.h"

typedef struct {
    Vector2 position;
    float speed;
    float rotation;
    float maxSpeed;
    float acceleration;
    float brakingPower;
    int health;
    int maxHealth;
    float boost;
    float maxBoost;
} Car;

Car Car_Create(Vector2 pos, float maxSpeed);
void Car_Update(Car* car, float maxSpeedModifier, float gripModifier, bool isOffRoad);
void Car_ApplyAcceleration(Car* car, float currentMaxSpeed);
void Car_ApplyBraking(Car* car);
void Car_ApplyDamping(Car* car);
void Car_Turn(Car* car, float direction, float gripModifier);
void Car_ApplyBoost(Car* car, float* cameraZoom);
void Car_RechargeBoost(Car* car);
void Car_TakeDamage(Car* car);
void Car_Heal(Car* car);
void Car_Reset(Car* car);

#endif
