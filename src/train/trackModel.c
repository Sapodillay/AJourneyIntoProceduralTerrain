#include "trackModel.h"

#include "raymath.h"
#include "rlgl.h"

Model frontCarriage;
Model carriage;
Model track;


void InitTrackModels()
{
	rlDisableBackfaceCulling();
	frontCarriage = LoadModel("resources/models/train.glb");
	track = LoadModel("resources/track_board.obj");
	//carriage = LoadModel("resources/models/train_carriage.obj");
	carriage = frontCarriage;

}

Model GetFrontTrainModel()
{
	return frontCarriage;
}

Model GetCarriageModel()
{
	return carriage;
}

Model GetTrackModel(Vector2 ChunkSize)
{
	//we want seperate model for each track?
	return track;
}

void SetModelRotation(Vector3 position, Vector3 forwardPosition, Model *model)
{
		Vector3 diff        = Vector3Subtract(position, forwardPosition);
		float y_angle       = -(atan2(diff.z, diff.x) + PI / 2.0);
		Vector3 newRotation = (Vector3) { 0, y_angle - NINTYDEGREES, 0 };
		Quaternion end   = QuaternionFromEuler(newRotation.z, newRotation.y, newRotation.x);
		model->transform = QuaternionToMatrix(end);
}
