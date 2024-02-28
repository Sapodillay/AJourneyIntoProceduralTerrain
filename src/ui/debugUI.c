#include "debugUI.h"

#include "player/playerController.h"
#include "world/chunk.h"

#include <stdlib.h>
#include <stdio.h>

void RenderDebugUI()
{
	RenderPlayerChunkText();
}

static void RenderPlayerChunkText()
{
	Camera* camera = GetPlayerCamera();
	Vector2 chunkPosition = WorldToChunkV3(camera->position);

	char chunkText[100];
	sprintf(chunkText, "x:%i y:%i", (int)chunkPosition.x, (int)chunkPosition.y);
	DrawText(chunkText, 20, 20, 20, RED);
}
