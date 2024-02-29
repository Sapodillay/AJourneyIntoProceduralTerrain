
#pragma once

#include <raylib.h>
#include "utils/mathutils.h"


typedef struct TrackSegment TrackSegment;
typedef struct TrackCursor TrackCursor;

struct TrackSegment
{
	int index;
	Vector3 endSegment;
	Vector3 startSegment;
	//cache this for easier chunk calculation
	Vector3 middlePoint;
	Vector2 chunkPosition;
	//for curves
	bool isCurve;
	Vector3 curvePoint;

	//should this track be rendered?
	bool isRender;

	float distance;
};

struct TrackCursor
{
	TrackSegment currentSegment;
	float progress;
};

void InitTrack();
void RenderTrack();
void RenderDebugTrack();
void RenderDebugCursor();
void UpdateTrainPosition();



void AddLineSegment(Vector3 start, Vector3 end);
void AddCurveSegment(Vector3 start, Vector3 end, Vector3 curveHandle);

//chunk extends, used to generate tracks on chunks
void ExtendTrackForward(float length);
void ExtendTrackCorner(bool direction, float length);



void DrawTrackLine(TrackSegment track, float height, float width);


//track helper functions
Vector3 TrackToWorld(TrackCursor cursor);
TrackCursor GetTrackForward(TrackCursor cursor, float distance);
Vector3 GetTrackForwardVector(TrackCursor cursor);
Vector3 GetSegmentDirection(TrackSegment segment);
int GetLastTrack(int trackIndex);
int GetNextTrack(int trackIndex);

void ToggleSegment(Vector2 position, bool state);

//returns first segment found in a chunk
TrackSegment GetSegmentInChunk(Vector2 position);