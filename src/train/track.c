#include "track.h"
#include "raymath.h"
#include "utils/mathutils.h"
#include "utils/imageutils.h"
#include "world/chunk.h"
#include "trackModel.h"

bool IS_DEBUG = true;

//keep track of the end track index
static int endTrackIndex = 0;

static int cursorAmount = 0;
static int trackAmount = 0;


int MAX_TRACK_SEGMENTS = 25;
static TrackSegment segments[25];

int MAX_CURSORS = 5;
static TrackCursor cursors[5];

static Model trackModel;

void InitTrack()
{

	trackModel = GetTrackModel(GetChunkSize());

	//create first segment
	AddLineSegment((Vector3){GetChunkSize().x / 2, 0, -GetChunkSize().x}, (Vector3){GetChunkSize().x / 2, 0, 0});


	for (int i = 0; i < MAX_TRACK_SEGMENTS; ++i)
	{
		
		float length = GetChunkSize().x;
		if (i % 2)
		{

			//bool isLeft = (bool)GetRandomValue(0, 1);
			bool isLeft = true;

			ExtendTrackCorner(isLeft, length);
		}
		else
		{
			ExtendTrackForward(length);
		}
	}
	for (int i = 0; i < MAX_CURSORS; ++i)
	{
		TrackCursor cursor = { 0 };
		cursor.currentSegment = segments[5 + i];
		cursor.progress = 0.5f;
		cursors[i] = cursor;
		cursorAmount++;
	}
}

void RenderTrack()
{
	if (IS_DEBUG)
	{
		RenderDebugTrack();
		RenderDebugCursor();
	}
	//Go along track, render the train boards.

	//set cursor to start of track
	TrackCursor cursor = { 0 };
	cursor.currentSegment = segments[GetNextTrack(endTrackIndex)];
	cursor.progress = 0.0f;

	bool hasReachedEnd = false;
	while (!hasReachedEnd)
	{
		cursor = GetTrackForward(cursor, 5.0f);
		if (cursor.currentSegment.index == endTrackIndex) {hasReachedEnd = true;}
		if (!cursor.currentSegment.isRender){continue;}

		Vector3 pos = TrackToWorld(cursor);
		Vector3 forwardPos = TrackToWorld(GetTrackForward(cursor, 0.1f));
		SetModelRotation(pos, forwardPos, &trackModel);

		pos.y = GetTrackHeight();
		
		DrawSphere(pos, 0.1f, YELLOW);
		DrawModel(trackModel, pos, 1.0f, WHITE);
	}
}

void RenderDebugTrack()
{

	int currentTrack = endTrackIndex;
	for (int i = 0; i < trackAmount; i++)
	{
		if (segments[currentTrack].isRender == false) 
		{ 
			currentTrack = GetNextTrack(currentTrack);
			continue; 
		}

		TrackSegment currentSeg = segments[currentTrack];
		if (currentSeg.isCurve == true)
		{

			float prog = 0;
			for (int j = 0; j < 5; j++)
			{
				prog += 0.2f;
				Vector3 p1 = BezierCurveLerp(currentSeg.startSegment, currentSeg.curvePoint, currentSeg.endSegment, prog);
				Vector3 p2 = BezierCurveLerp(currentSeg.startSegment, currentSeg.curvePoint, currentSeg.endSegment, prog - 0.2);
				DrawLine3D(p1, p2, RED);
			}

			Vector2 chunkPos = WorldToChunkV3(currentSeg.middlePoint);

			DrawSphere(currentSeg.endSegment, 0.5f, GREEN);
			DrawSphere(currentSeg.curvePoint, 0.5f, GREEN);
			DrawSphere(currentSeg.middlePoint, 1.0f,  PINK);
		}
		else
		{
			Vector2 chunkPos = WorldToChunkV3(currentSeg.middlePoint);
			//draw track segment
			DrawSphere(currentSeg.endSegment, 0.5f, GREEN);
			DrawLine3D(currentSeg.startSegment, currentSeg.endSegment, RED);
			DrawSphere(currentSeg.middlePoint, 1.0f, PINK);
		}
		currentTrack = GetNextTrack(currentTrack);
	}



}

void RenderDebugCursor()
{
	for (int i = 0; i < MAX_CURSORS; ++i)
	{
		TrackCursor cursor = cursors[i];
		//find world position
		DrawSphere(TrackToWorld(cursor), 1.0f, RED);
	}
}

void UpdateTrainPosition()
{
	for (int i = 0; i < cursorAmount; ++i)
	{
		cursors[i] = GetTrackForward(cursors[i], 0.1f);
	}
}


void AddLineSegment(Vector3 start, Vector3 end)
{
	endTrackIndex = GetNextTrack(endTrackIndex);
	if (trackAmount < MAX_TRACK_SEGMENTS) trackAmount++;

	//add segment
	TrackSegment segment = { 0 };
	segment.isRender = false;
	segment.startSegment = start;
	segment.index = endTrackIndex;
	segment.endSegment = end;
	segment.isCurve = false;
	segment.distance = Vector3Distance(start, end);
	
	segment.middlePoint = Vector3Lerp(start, end, 0.5);
	segment.chunkPosition = WorldToChunkV3(segment.middlePoint);

	segments[endTrackIndex] = segment;
}

void AddCurveSegment(Vector3 start, Vector3 end, Vector3 curveHandle)
{
	endTrackIndex = GetNextTrack(endTrackIndex);
	if (trackAmount < MAX_TRACK_SEGMENTS) trackAmount++;

	//add segment
	TrackSegment segment = { 0 };
	segment.isRender = false;
	segment.startSegment = start;
	segment.index = endTrackIndex;
	segment.endSegment = end;

	segment.curvePoint = curveHandle;
	segment.isCurve = true;

	segment.middlePoint = curveHandle;
	segment.chunkPosition = WorldToChunkV3(segment.middlePoint);

	//fix to calculate curve distance.
	segment.distance = Vector3Distance(start, end);
	segments[endTrackIndex] = segment;
}

void ExtendTrackForward(float length)
{
	TrackSegment lastSegment = segments[endTrackIndex];
	Vector3 directionVector = GetSegmentDirection(lastSegment);

	AddLineSegment(lastSegment.endSegment, Vector3Add(lastSegment.endSegment, Vector3Scale(directionVector, length)));
}

void ExtendTrackCorner(bool direction, float length)
{
	length = length / 2;
	TrackSegment lastSegment = segments[endTrackIndex];
	Vector3 directionVector = GetSegmentDirection(lastSegment);

	Vector3 leftVector = (Vector3){directionVector.z, directionVector.y, directionVector.x};
	Vector3 leftScalar = Vector3Scale(leftVector, length);

	Vector3 endVector = Vector3Add(lastSegment.endSegment, Vector3Scale(directionVector, length));
	Vector3 handle = endVector;

	//left direction#
	if (direction)
	{
		endVector = Vector3Add(endVector, leftScalar);
	}
	else
	{
		endVector = Vector3Subtract(endVector, leftScalar);
	}

	AddCurveSegment(lastSegment.endSegment, endVector, handle);
}

void DrawTrackLine(TrackSegment track, float height, float width)
{
	float scale = GetScale();

	Vector2 startLine = (Vector2){track.startSegment.x, track.startSegment.z};
	//scale to correct resolution.
	startLine = Vector2Scale(startLine, scale);
	startLine.x = (startLine.x - (track.chunkPosition.x * GetNoiseResolution().x));
	startLine.y = (startLine.y - (track.chunkPosition.y * GetNoiseResolution().y));


	Vector2 endLine = (Vector2){track.endSegment.x, track.endSegment.z};
	endLine = Vector2Scale(endLine, scale);
	endLine.x = (endLine.x - (track.chunkPosition.x * GetNoiseResolution().x));
	endLine.y = (endLine.y - (track.chunkPosition.y * GetNoiseResolution().y));
	
	Color drawColor = HeightToGrayColor(height);
	if (track.isCurve)
	{

		Vector2 curveHandle = (Vector2){track.curvePoint.x, track.curvePoint.z};
		curveHandle = Vector2Scale(curveHandle, scale);
		curveHandle.x = (curveHandle.x - (track.chunkPosition.x * GetNoiseResolution().x));
		curveHandle.y = (curveHandle.y - (track.chunkPosition.y * GetNoiseResolution().y));
		//scale

		Vector2* points = malloc(sizeof(Vector2) * 3);
		points[0] = startLine;
		points[1] = curveHandle;
		points[2] = endLine;

		//fix rounding error
		Vector2 nearEnd = GetSplinePointBezierQuad(startLine, curveHandle, endLine, 0.99);
		Vector2 direction = Vector2Direction(nearEnd, endLine);
		endLine = Vector2Subtract(endLine, direction);
		points[2] = endLine;

		DrawSplineBezierQuadratic(points, 3, width, drawColor);
		free(points);
	}
	else
	{

		//account for pixel rounding error
		Vector2 dir = Vector2Direction(startLine, endLine);
		endLine = Vector2Subtract(endLine, dir);

		DrawLineEx(startLine, endLine, width, drawColor);
	}
}

Vector3 TrackToWorld(TrackCursor cursor)
{

	if (cursor.currentSegment.isCurve)
	{
		return BezierCurveLerp(cursor.currentSegment.startSegment, cursor.currentSegment.curvePoint, cursor.currentSegment.endSegment, cursor.progress);
	}
	return Vector3Lerp(cursor.currentSegment.startSegment, cursor.currentSegment.endSegment, cursor.progress);
}

TrackCursor GetTrackForward(TrackCursor cursor, float distance)
{

	float segmentDistance = cursor.currentSegment.distance;
	float currentDistance = segmentDistance * cursor.progress;
	float distanceNeeded = segmentDistance - currentDistance;

	//add distance in world space.
	currentDistance += distance;

	//calculate new percentage.
	float newProgress = currentDistance / segmentDistance;

	TrackCursor newCursor = { 0 };

	//if progress is less than one, we don't need to account for overflow.
	if (newProgress < 1.0f)
	{
		newCursor.currentSegment = cursor.currentSegment;
		newCursor.progress = newProgress;
		return newCursor;
	}

	//recalculate distance
	float remainingDistance = distance - distanceNeeded;

	TrackCursor recursiveCursor = { 0 };
	recursiveCursor.currentSegment = segments[GetNextTrack(cursor.currentSegment.index)];
	recursiveCursor.progress = 0.0f;

	newCursor = GetTrackForward(recursiveCursor, remainingDistance);
	return newCursor;
}

Vector3 GetSegmentDirection(TrackSegment segment)
{
	if (segment.isCurve)
	{
		return Vector3Direction(segment.endSegment, segment.curvePoint);
	}
	return Vector3Direction(segment.endSegment, segment.startSegment);
}

int GetLastTrack(int trackIndex)
{
	if (trackIndex == 0)
	{
		return MAX_TRACK_SEGMENTS - 1;
	}
	else
	{
		return trackIndex - 1;
	}
}

int GetNextTrack(int trackIndex)
{
	if (trackIndex == MAX_TRACK_SEGMENTS - 1)
	{
		return 0;
	}
	else
	{
		return trackIndex + 1;
	}
}

void ToggleSegment(Vector2 position, bool state)
{
	for (int i = 0; i < trackAmount; ++i)
	{
		Vector2 segChunkPos = WorldToChunkV3(segments[i].middlePoint);

		if (Vector2Equals(position, segChunkPos))
		{
			segments[i].isRender = state;
		}
	}
}

TrackSegment GetSegmentInChunk(Vector2 position)
{
	for (int i = 0; i < trackAmount; ++i)
	{
		Vector2 segChunkPos = WorldToChunkV3(segments[i].middlePoint);
		if (Vector2Equals(position, segChunkPos))
		{
			return segments[i];
		}
	}

	//create empty segment
	TrackSegment nullSeg = { 0 };
	nullSeg.index = -1;
	return nullSeg;
}
