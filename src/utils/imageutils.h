#pragma once
#include "raylib.h"
#include "train/track.h"

//Terrain colors for colormap.
#define COLOR_DEEP_WATER    CLITERAL(Color){135, 115, 0, 255}
#define COLOR_WATER			CLITERAL(Color){190, 170, 5, 255}
#define COLOR_SAND			CLITERAL(Color){250, 211, 63, 255}
#define COLOR_LAND			CLITERAL(Color){72, 245, 66, 255}

//gray value of a rgb pixel.
#define GRAY_VALUE(c) ((float)(c.r + c.g + c.b)/3.0f)


//Image data
float ImageGetGrayPixel(Image* source, int x, int y);

//height from 0-1
Color HeightToGrayColor(float height);


void ImageOverlay(Image* source, Image overlay);
void ImageDampen(Image* source, Image dampenImage);


//Image gens
Image GenImagePerlinNoiseChunk(int width, int height, int offsetX, int offsetY, float scale, int octaves, float persistence, float lacunarity);
Image GenImageNoiseToColor(Image* source);


Image DrawTrackLine(TrackSegment track, Image source, float height, float width);
