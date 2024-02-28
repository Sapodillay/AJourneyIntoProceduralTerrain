#include "playerController.h"

#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"

#include <stdlib.h>

#include "world/world.h"
#include "world/chunk.h"

Camera camera = { 0 };
Ray groundRay = { 0 }; 

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
}

void DrawPlayerDebug()
{
	DrawRay(groundRay, RED);
}

void updatePlayerController(Vector3 moveVector)
{


	CameraMoveForward(&camera, moveVector.y, true);
	CameraMoveRight(&camera, moveVector.x, true);
	CameraMoveUp(&camera, moveVector.z);

	Vector2 mouseDelta = GetMouseDelta();
	CameraYaw(&camera, -(mouseDelta.x * 0.01), false);
	CameraPitch(&camera, -(mouseDelta.y * 0.01), true, false, false);



	groundRay.position = camera.position;
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
			Vector3 difference = Vector3Subtract(camera.position, rayCollision.point);
		}
	}


}

Camera* GetPlayerCamera()
{
	return &camera;
}
