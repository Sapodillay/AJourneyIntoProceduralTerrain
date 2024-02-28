#include "world.h"

#include "raylib.h"
#include "raymath.h"

#include "chunk.h"
#include "train/track.h"


#include <stdlib.h>


int renderDistance = 1;
//Active chunks
Chunk* m_chunks;
int chunkArraySize = 0;

//render position
Vector2 center = { 0 };
Vector2 centerChunk = { 0 };
Chunk centerChunkCache;

//water
Shader waterShader;

Model waterModel;
Vector2 waterSize;
float waterLevel = 35.0f;

bool firstPass = true;

void InitWorld()
{
	//set centerChunk to empty.
	centerChunkCache = EmptyChunk();

	int baseRender = 1;
	baseRender += (2 * renderDistance);
	chunkArraySize = baseRender * baseRender;
	m_chunks = (Chunk*)malloc(chunkArraySize * sizeof(Chunk));
	for (int i = 0; i < chunkArraySize; i++)
	{
		Chunk chunk = EmptyChunk();
		m_chunks[i] = chunk;
	}

	waterShader = LoadShader("src/shaders/water.vs", "src/shaders/water.fs");
	//load water
	Mesh waterMesh = GenMeshPlane(128.0f, 128.0f, 16, 16);
	waterModel = LoadModelFromMesh(waterMesh);

	waterModel.materials[0].shader = waterShader;

	//UnloadMesh(waterMesh);
}

void RenderWorld()
{
	for (int i = 0; i < chunkArraySize; i++)
	{
		Chunk chunk = m_chunks[i];
		if (!chunk.isEmpty)
		{
			DrawModel(chunk.chunkModel, chunk.worldPosition, 1.0f, WHITE);
		}
	}
	RenderWater();
}

void RenderWater()
{
	DrawModel(waterModel, (Vector3){center.x, GetTrackHeight() - 2.0f, center.y}, 10.0f, DARKBLUE);
}

Chunk GetChunk(Vector2 chunkPosition)
{
	for (int i = 0; i < chunkArraySize; i++)
	{
		if (!m_chunks[i].isEmpty)
		{
			if (Vector2Equals(m_chunks[i].chunkPosition, chunkPosition))
			{
				return m_chunks[i];
			}
		}
	}

	//if chunk isn't loaded, return empty chunk to prevent null errors.
	Chunk emptyChunk = EmptyChunk();
	return emptyChunk;
}

void UpdateChunks(Vector2 newChunkPosition)
{
	centerChunk = newChunkPosition;

	//unload all unneeded chunks
	for (int i = 0; i < chunkArraySize; i++)
	{
		Chunk chunk = m_chunks[i];

		if (!chunk.isEmpty)
		{
			//only care about furthest distance, don't need to calculate.
			Vector2 chunkDelta = Vector2Subtract(centerChunk, chunk.chunkPosition);
			chunkDelta.x = abs(chunkDelta.x);
			chunkDelta.y = abs(chunkDelta.y);
			int distance = (chunkDelta.x > chunkDelta.y) ? chunkDelta.x : chunkDelta.y;


			if (distance > renderDistance)
			{
				ToggleSegment(chunk.chunkPosition, false);
				UnloadChunk(chunk);

				Chunk emptyChunk = EmptyChunk();
				m_chunks[i] = emptyChunk;
			}
		}
	}


	
	//rerender all chunks. unoptimised.
	for (int x = -renderDistance; x <= renderDistance; x++)
	{
		for (int y = -renderDistance; y <= renderDistance; y++)
		{
			Vector2 chunkPos = centerChunk;
			chunkPos.x -= (float)x;
			chunkPos.y -= (float)y;


			//unoptimised part, use hash table if becomes problem

			//check if this position has a chunk
			bool isEmpty = true;
			for (int i = 0; i < chunkArraySize; i++)
			{
				if (Vector2Equals(chunkPos, m_chunks[i].chunkPosition))
				{
					isEmpty = false;
				}
			}

			if (isEmpty)
			{
				for (int i = 0; i < chunkArraySize; i++)
				{
					if (m_chunks[i].isEmpty)
					{
						Chunk chunk;
						chunk = GenerateChunk(chunkPos);
						ToggleSegment(chunk.chunkPosition, true);

						m_chunks[i] = chunk;
						break;
					}
				}

			}

		}
	}
}

void UpdateWorld(Vector2 position)
{
	center = position;
	Vector2 newCenterChunk = WorldToChunk(center);

	if (firstPass)
	{
		UpdateChunks(newCenterChunk);
		firstPass = false;
	}
	if (!Vector2Equals(newCenterChunk, centerChunk))
	{
		UpdateChunks(newCenterChunk);
	}
}
