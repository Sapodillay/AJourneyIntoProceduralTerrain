#include "mathutils.h"

#include <raymath.h>

float MathInverseLerp(float a, float b, float v)
{
	return (v - a) / (b - a);
}

Vector3 BezierCurveLerp(Vector3 p1, Vector3 p2, Vector3 p3, float progress)
{
	
	Vector3 lerp1 = Vector3Lerp(p1, p2, progress);
	Vector3 lerp2 = Vector3Lerp(p2, p3, progress);
	return Vector3Lerp(lerp1, lerp2, progress);
}



Vector2 Vector2Direction(Vector2 p1, Vector2 p2)
{
	return Vector2Normalize(Vector2Subtract(p1, p2));
}

Vector3 Vector3Direction(Vector3 p1, Vector3 p2)
{
	return Vector3Normalize(Vector3Subtract(p1, p2));
}
