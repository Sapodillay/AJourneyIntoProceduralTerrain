#include "playerController.h"

#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"

#include <stdlib.h>

#include "world/world.h"
#include "world/chunk.h"

Camera camera = { 0 };
Ray groundRay = { 0 }; 


static float playerHeight = 7.5f;

void InitPlayerController()
{
    camera.position = (Vector3){ 64.0f, 64.0f, 64.0f }; // Camera position
    camera.target = (Vector3){ 1.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;  

}

void handePlayerInput()
{
	Vector3 moveVector = { 0 }; 

	if (IsKeyDown(KEY_A))
	{
		moveVector.x -= 1.0f;
	}
	if (IsKeyDown(KEY_D))
	{
		moveVector.x += 1.0f;
	}
	if (IsKeyDown(KEY_W))
	{
		moveVector.y += 1.0f;
	}
	if (IsKeyDown(KEY_S))
	{
		moveVector.y -= 1.0f;
	}

	if (IsKeyDown(KEY_Q))
	{
		moveVector.z += 1.0f;
	}
	if (IsKeyDown(KEY_E))
	{
		moveVector.z -= 1.0f;
	}
	moveVector = Vector3Normalize(moveVector);
	//update physics.
	updatePlayerController(moveVector);
	//handleGroundCollision();
}

void handleGroundCollision()
{
	//create ray at players position
	groundRay.position = camera.position;
	//set ray in the sky.
	groundRay.position.y = 256.0f;
	//point ray down.
	groundRay.direction = (Vector3){0, -1, 0};

	Vector2 chunkPos = WorldToChunkV3(groundRay.position);
	//Ground check
	Chunk chunk = GetChunk(chunkPos);
	if (!chunk.isEmpty)
	{
		Mesh chunkMesh = chunk.chunkModel.meshes[0];
		RayCollision rayCollision = GetRayCollisionMesh(groundRay, chunkMesh, MatrixTranslate(chunk.worldPosition.x, chunk.worldPosition.y, chunk.worldPosition.z));
		if (rayCollision.hit)
		{
			//if chunk is above water level

			float groundDistance =Vector3Distance(camera.position, rayCollision.point) - playerHeight;
			float waterDistance = abs((GetWaterLevel() - camera.position.y)) - playerHeight;

			if (groundDistance < waterDistance)
			{
				SetPlayerPosition(rayCollision.point);
				TraceLog(LOG_INFO, "Ground");
			}
			else
			{
				Vector3 waterPosition = (Vector3){camera.position.x, GetWaterLevel(), camera.position.z};
				SetPlayerPosition(waterPosition);
				TraceLog(LOG_INFO, "Water");
			}
		}



	}


}

void DrawPlayerDebug()
{
	DrawRay(groundRay, RED);
}

void SetPlayerPosition(Vector3 position)
{
	Vector3 difference = Vector3Subtract(camera.position, position);

	Vector3 playerPosition = Vector3Subtract(camera.position, difference);
	Vector3 targetPosition = Vector3Subtract(camera.target, difference);
	//add player height to position.
	playerPosition.y += playerHeight;
	targetPosition.y += playerHeight;
	//update camera
	camera.position = playerPosition;
	camera.target = targetPosition;
}

void updatePlayerController(Vector3 moveVector)
{


	CameraMoveForward(&camera, moveVector.y, true);
	CameraMoveRight(&camera, moveVector.x, true);
	CameraMoveUp(&camera, moveVector.z);

	Vector2 mouseDelta = GetMouseDelta();
	CameraYaw(&camera, -(mouseDelta.x * 0.01), false);
	CameraPitch(&camera, -(mouseDelta.y * 0.01), true, false, false);

}

Camera* GetPlayerCamera()
{
	return &camera;
}
