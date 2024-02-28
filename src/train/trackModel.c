#include "trackModel.h"

#include "raymath.h"

Model GetTrackModel(Vector2 ChunkSize)
{
	return LoadModel("resources/track_board.obj");
}

void SetModelRotation(Vector3 position, Vector3 forwardPosition, Model *model)
{
		Vector3 diff        = Vector3Subtract(position, forwardPosition);
		float y_angle       = -(atan2(diff.z, diff.x) + PI / 2.0);
		Vector3 newRotation = (Vector3) { 0, y_angle - NINTYDEGREES, 0 };
		Quaternion end   = QuaternionFromEuler(newRotation.z, newRotation.y, newRotation.x);
		model->transform = QuaternionToMatrix(end);
}
