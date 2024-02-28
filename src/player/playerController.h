#pragma once

#include "raylib.h"
#include "rcamera.h"

typedef struct PlayerController PlayerController;



void InitPlayerController();
void handePlayerInput();
void updatePlayerController();
void DrawPlayerDebug();

Camera* GetPlayerCamera();