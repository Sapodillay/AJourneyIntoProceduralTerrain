#pragma once

#include "raylib.h"

typedef struct Chunk Chunk;

struct Chunk
{
	/* data */
	Model chunkModel;
	Vector2 chunkPosition;
	Vector3 worldPosition;
	bool isEmpty;
};

void InitChunkAssets();

Chunk GenerateChunk(Vector2 chunkPosition);
Chunk EmptyChunk();
void UnloadChunk(Chunk chunk);


Vector2 GetChunkSize();
Vector2 GetNoiseResolution();
//returns difference between size and resolution, to scale things.
float GetScale();
Vector2 WorldToChunk(Vector2 worldPosition);
Vector2 WorldToChunkV3(Vector3 worldPosition);
Vector3 ChunkToWorld(Vector2 chunkPosition);
float GetTrackHeight();

Model GenerateIslandModel(Vector2 position);

