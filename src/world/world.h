#pragma once
#include "chunk.h"

#define MAX_CHUNKS = 64;

void InitWorld();

void RenderWorld();
void RenderWater();


Chunk GetChunk(Vector2 chunkPosition);
Model GetWaterMesh();
float GetWaterLevel();


void UpdateChunks(Vector2 newChunkPosition);
void UpdateWorld(Vector2 position);