#include "chunk.h"
#include "train/track.h"
#include "../utils/imageutils.h"
#include "../mesh/MeshGen.h"
#include "raymath.h"


//resolutions
Vector2 noiseResolution = { 256, 256 };
Vector2 chunkSize = { 128, 128 };


//chunk variables
float chunkHeight = 0.4f;

float TRAINTRACK_HEIGHT = 0.55f;

//noise variables
static int octaves = 3;
static float scale = 4.0f;
static float persistence = 0.2f;
static float lacunarity = 2.0f;

Texture2D chunkTexture;


//no seed for now.
static int seed = 0;

Model GenerateIslandModel(Vector2 position)
{

	//get track lines
	int xOffset = (int)position.x * noiseResolution.x;
	int yOffset = (int)position.y * noiseResolution.y;

	Image noiseImage = GenImagePerlinNoiseChunk(noiseResolution.x + 1, noiseResolution.y + 1, seed + xOffset - 1, seed + yOffset - 1, scale, octaves, persistence, lacunarity);
	//Image noiseImage = GenImagePerlinNoise(noiseResolution.x, noiseResolution.y, xOffset, yOffset, 4.0f);

	RenderTexture2D tex = LoadRenderTexture(noiseImage.width, noiseImage.height);
	BeginTextureMode(tex);
	ClearBackground(WHITE);

	DrawTexture(LoadTextureFromImage(noiseImage), 0, 0, WHITE);


	TrackSegment track = GetSegmentInChunk(position);
	if (track.index != -1)
	{
		//ImageDrawLineV(&noiseImage, startLine, endLine, (Color){200, 200, 200, 255});
		if (IsImageReady(noiseImage))
		{
			DrawTrackLine(track, TRAINTRACK_HEIGHT, 10.0f * GetScale());
			//DrawTrackLine(track, TRAINTRACK_HEIGHT, 25.0f * GetScale());
		}
	}

	EndTextureMode();

	Image texImage = LoadImageFromTexture(tex.texture);
	ImageFlipVertical(&texImage);


	//generate model
	Model model = GenChunk(texImage, (Vector3){chunkSize.x, chunkSize.y * chunkHeight, chunkSize.y});
	model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = chunkTexture;

	UnloadImage(noiseImage);

	return model;
}

void InitChunkAssets()
{
	chunkTexture = LoadTexture("resources/palette.png");
}

Chunk GenerateChunk(Vector2 chunkPosition)
{
	Chunk chunk = { 0 };
	chunk.chunkPosition = chunkPosition;
	chunk.worldPosition = ChunkToWorld(chunkPosition);
	chunk.isEmpty = false;
	chunk.chunkModel = GenerateIslandModel(chunkPosition);
	return chunk;
}

Chunk EmptyChunk()
{
	Chunk empty;
	empty.isEmpty = true;
	//prevents 0,0 having false flags as empty
	//will never be equal to any chunk position, chunkPositions cant be decimal.
	empty.chunkPosition = (Vector2){-0.5f, 0.5f};
	return empty;
}

void UnloadChunk(Chunk chunk)
{
	//dont unload texture as its shared between all chunks
	//UnloadTexture(chunk.chunkModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture);
	UnloadModel(chunk.chunkModel);
}

Vector2 GetChunkSize()
{
	return chunkSize;
}

Vector2 GetNoiseResolution()
{
	return noiseResolution;
}

float GetScale()
{
	return (noiseResolution.x + noiseResolution.y) / (chunkSize.x + chunkSize.y);
}

Vector2 WorldToChunk(Vector2 worldPosition)
{
	Vector2 currentChunk = Vector2Divide(worldPosition, GetChunkSize());
	currentChunk.x = floor(currentChunk.x);
	currentChunk.y = floor(currentChunk.y);
	return currentChunk;
}

Vector2 WorldToChunkV3(Vector3 worldPosition)
{
	Vector2 worldPositionV2 = (Vector2){worldPosition.x, worldPosition.z};
	Vector2 currentChunk = Vector2Divide(worldPositionV2, GetChunkSize());
	currentChunk.x = floor(currentChunk.x);
	currentChunk.y = floor(currentChunk.y);
	return currentChunk;
}

Vector3 ChunkToWorld(Vector2 chunkPosition)
{
	Vector2 worldPositionV2 = Vector2Multiply(chunkPosition, GetChunkSize());
	Vector3 worldPosition;
	worldPosition.x = worldPositionV2.x;
	worldPosition.y = 0;
	worldPosition.z = worldPositionV2.y;
	return worldPosition;
}

float GetTrackHeight()
{
	return (TRAINTRACK_HEIGHT * (chunkSize.y * chunkHeight));
}
