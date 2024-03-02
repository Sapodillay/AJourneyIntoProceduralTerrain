#pragma once

#include "raylib.h"
#include "rcamera.h"

typedef struct PlayerController PlayerController;



void InitPlayerController();
void handePlayerInput();
void handleGroundCollision();
void updatePlayerController();
void DrawPlayerDebug();

void SetPlayerPosition(Vector3 position);

Camera* GetPlayerCamera();