#include "imageutils.h"
#include "raylib.h"
#include "raymath.h"

#include "world/chunk.h"
#include "mathutils.h"
#include "logger.h"

//library includes
#include "external/stb_perlin.h" //noise
#include <stdlib.h> //malloc

float ImageGetGrayPixel(Image *source, int x, int y)
{
	float gray = MathInverseLerp(0, 255, GRAY_VALUE(GetImageColor(*source, x, y)));
	return gray;
}

Color HeightToGrayColor(float height)
{
	float gray_rgb = Lerp(0.0f, 255.0f, height);
	return (Color) { gray_rgb, gray_rgb, gray_rgb, 255.0f };
}

void ImageOverlay(Image *source, Image overlay)
{
	if (source->height != overlay.height || source->width != overlay.width) { TraceLog(LOG_ERROR, "ImageOverlay: overlay and source not same size"); };
	for (int x = 0; x < source->width; x++)
	{
		for (int y = 0; y < source->height; y++)
		{
			Color overlayPixel = GetImageColor(overlay, x, y);
			if (overlayPixel.a != 0)
			{
				ImageDrawPixelV(source, (Vector2) { x, y }, overlayPixel);
			}
		}
	}
}

void ImageDampen(Image *source, Image dampenImage)
{
	if (source->height != dampenImage.height || source->width != dampenImage.width) { TraceLog(LOG_ERROR, "ImageDampen: dampenImage and source not same size"); };
	for (int x = 0; x < source->width; x++)
	{
		for (int y = 0; y < source->height; y++)
		{
			Color dampenPixel = GetImageColor(dampenImage, x, y);
			if (dampenPixel.a == 0) {continue;};

			Color sourcePixel = GetImageColor(*source, x, y);

			Color lerped;
			lerped.r = Lerp(sourcePixel.r, dampenPixel.r, (dampenPixel.a / 255));
			lerped.g = Lerp(sourcePixel.g, dampenPixel.g, (dampenPixel.a / 255));
			lerped.b = Lerp(sourcePixel.b, dampenPixel.b, (dampenPixel.a / 255));
			lerped.a = 255.0f;
			ImageDrawPixelV(source, (Vector2) { x, y }, lerped);
		}
	}
}

Image GenImagePerlinNoiseChunk(int width, int height, int offsetX, int offsetY, float scale, int octaves, float persistence, float lacunarity)
{
    Color *pixels = (Color*)malloc(width * height * sizeof(Color));

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {


            float nx = (float)(x + offsetX) * (scale / (float)width);
            float ny = (float)(y + offsetY) * (scale / (float)height);

            // Basic perlin noise implementation (not used)
            //float p = (stb_perlin_noise3(nx, ny, 0.0f, 0, 0, 0);

            // Calculate a better perlin noise using fbm (fractal brownian motion)
            // Typical values to start playing with:
            //   lacunarity = ~2.0   -- spacing between successive octaves (use exactly 2.0 for wrapping output)
            //   gain       =  0.5   -- relative weighting applied to each successive octave
            //   octaves    =  6     -- number of "octaves" of noise3() to sum


			//corners
			//TODO: average each corner based on the sourrundings
			float p = stb_perlin_fbm_noise3(nx, ny, 1.0f, lacunarity, persistence, octaves);

            // Clamp between -1.0f and 1.0f
            if (p < -1.0f) p = -1.0f;
            if (p > 1.0f) p = 1.0f;

            // We need to normalize the data from [-1..1] to [0..1]
            float np = (p + 1.0f) / 2.0f;

            int intensity = (int)(np * 255.0f);
            pixels[y * width + x] = (Color){ intensity, intensity, intensity, 255 };
        }
    }

    Image image = {
        .data = pixels,
        .width = width,
        .height = height,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };

    return image;
}

Image GenImageNoiseToColor(Image *source)
{
	Color* pixels = (Color*)malloc(source->width * source->height * sizeof(Color));
	for (int y = 0; y < source->height; y++)
	{
		for (int x = 0; x < source->width; x++)
		{

			pixels[y * source->width + x] = (Color){ 255, 0, 0, 255 };
			float sourceGray = ImageGetGrayPixel(source, x, y);

			if (sourceGray < 0.3)
			{
				pixels[y * source->width + x] = COLOR_DEEP_WATER;
			}
			else if (sourceGray < 0.5)
			{
				//color water
				pixels[y * source->width + x] = COLOR_WATER;
			}
			else if (sourceGray < 0.55)
			{
				pixels[y * source->width + x] = COLOR_SAND;
			}
			else
			{
				//color land
				pixels[y * source->width + x] = COLOR_LAND;
			}
		}
	}

	Image image = {
		.data = pixels,
		.width = source->width,
		.height = source->height,
		.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
		.mipmaps = 1
	};

	return image;
}

Image DrawTrackLine(TrackSegment track, Image source, float height, float width)
{

	float scale = GetScale();

	Vector2 startLine = (Vector2){track.startSegment.x, track.startSegment.z};
	PrintVector2(startLine);
		//scale to correct resolution.
	startLine = Vector2Scale(startLine, scale);
	startLine.x = (startLine.x - (track.chunkPosition.x * GetNoiseResolution().x));
	startLine.y = (startLine.y - (track.chunkPosition.y * GetNoiseResolution().y));


	Vector2 endLine = (Vector2){track.endSegment.x, track.endSegment.z};
	endLine = Vector2Scale(endLine, scale);
	endLine.x = (endLine.x - (track.chunkPosition.x * GetNoiseResolution().x));
	endLine.y = (endLine.y - (track.chunkPosition.y * GetNoiseResolution().y));
	


	Color drawColor = HeightToGrayColor(height);
	RenderTexture2D tex = LoadRenderTexture(source.width, source.height);
	BeginTextureMode(tex);
	ClearBackground((Color){0.0f, 0.0f, 0.0f, 0.0f});

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

	EndTextureMode();

	Image texImage = LoadImageFromTexture(tex.texture);
	ImageFlipVertical(&texImage);
	return texImage;
}
