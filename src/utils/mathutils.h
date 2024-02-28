#pragma once

#include <raylib.h>

float MathInverseLerp(float a, float b, float v);
Vector3 BezierCurveLerp(Vector3 p1, Vector3 p2, Vector3 p3, float progress);

//returns the direction pointing towards vector2 as a unit vector
Vector3 Vector3Direction(Vector3 p1, Vector3 p2);
Vector2 Vector2Direction(Vector2 p1, Vector2 p2);
