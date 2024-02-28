#include "raylib.h"

#define NINTYDEGREES 1.5708

Model GetTrackModel(Vector2 ChunkSize);
void SetModelRotation(Vector3 position, Vector3 forwardPosition, Model* model);