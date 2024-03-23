#include "raylib.h"
#include "game.h"

#include "world/chunk.h"
#include "world/world.h"

#include "train/track.h"
#include "train/trackModel.h"

#include "player/playerController.h"
#include "ui/debugUI.h"

Vector3 cubePosition = {0};


Model chunkTest;

Camera* playerCamera;

void InitGame()
{
	//initalize
	InitPlayerController();
	InitChunkAssets();
	InitWorld();
	InitTrackModels();
	InitTrack();

	

	//game settings
	DisableCursor();
	//SetTraceLogLevel(LOG_WARNING);


	playerCamera = GetPlayerCamera();

}

void UpdateDrawFrame()
{

	handePlayerInput();
	//----------------------------------------------------------------------------------
	// Draw
	//----------------------------------------------------------------------------------
	BeginDrawing();

	ClearBackground((Color){50.0f, 200.0f, 255.0f, 255.0f});

	BeginMode3D(*playerCamera);

	UpdateWorld((Vector2){playerCamera->position.x, playerCamera->position.z});


	UpdateTrainPosition();
	RenderWorld();
	DrawGrid(128, GetChunkSize().x);
	RenderTrack();
	DrawPlayerDebug();


	EndMode3D();
	RenderDebugUI();
	EndDrawing();
	//----------------
}