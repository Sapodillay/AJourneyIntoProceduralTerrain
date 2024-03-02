#include "raylib.h"

#define NINTYDEGREES 1.5708


void InitTrackModels();

Model GetFrontTrainModel();
Model GetCarriageModel();

Model GetTrackModel(Vector2 ChunkSize);
void SetModelRotation(Vector3 position, Vector3 forwardPosition, Model* model);