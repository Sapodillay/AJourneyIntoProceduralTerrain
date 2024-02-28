#include "MeshGen.h"


#include "raylib.h"
#include "raymath.h"

float MapTexture(float value, float startUV, float endUV, float scale)
{   
    return Remap(value, 0.0f, 1.0f, startUV, endUV);
}

Model GenChunk(Image heightmap, Vector3 size)
{
    #define GRAY_VALUE(c) ((float)(c.r + c.g + c.b)/3.0f)

    Mesh mesh = { 0 };

    int mapX = heightmap.width;
    int mapZ = heightmap.height;

    Color *pixels = LoadImageColors(heightmap);

    // NOTE: One vertex per pixel
    mesh.triangleCount = (mapX - 1)*(mapZ - 1)*2;    // One quad every four pixels

    mesh.vertexCount = mesh.triangleCount*3;

    mesh.vertices = (float *)RL_MALLOC(mesh.vertexCount*3*sizeof(float));
    mesh.normals = (float *)RL_MALLOC(mesh.vertexCount*3*sizeof(float));
    mesh.texcoords = (float *)RL_MALLOC(mesh.vertexCount*2*sizeof(float));
    mesh.colors = ((void *)0);

    int vCounter = 0;       // Used to count vertices float by float
    int tcCounter = 0;      // Used to count texcoords float by float
    int nCounter = 0;       // Used to count normals float by float

    Vector3 scaleFactor = { size.x/(mapX - 1), size.y/255.0f, size.z/(mapZ - 1) };

    Vector3 vA = { 0 };
    Vector3 vB = { 0 };
    Vector3 vC = { 0 };
    Vector3 vN = { 0 };

    for (int z = 0; z < mapZ-1; z++)
    {
        for (int x = 0; x < mapX-1; x++)
        {
            // Fill vertices array with data
            //----------------------------------------------------------

            // one triangle - 3 vertex
            mesh.vertices[vCounter] = (float)x*scaleFactor.x;
            mesh.vertices[vCounter + 1] = GRAY_VALUE(pixels[x + z*mapX])*scaleFactor.y;
            mesh.vertices[vCounter + 2] = (float)z*scaleFactor.z;

            mesh.vertices[vCounter + 3] = (float)x*scaleFactor.x;
            mesh.vertices[vCounter + 4] = GRAY_VALUE(pixels[x + (z + 1)*mapX])*scaleFactor.y;
            mesh.vertices[vCounter + 5] = (float)(z + 1)*scaleFactor.z;

            mesh.vertices[vCounter + 6] = (float)(x + 1)*scaleFactor.x;
            mesh.vertices[vCounter + 7] = GRAY_VALUE(pixels[(x + 1) + z*mapX])*scaleFactor.y;
            mesh.vertices[vCounter + 8] = (float)z*scaleFactor.z;

            // Another triangle - 3 vertex
            mesh.vertices[vCounter + 9] = mesh.vertices[vCounter + 6];
            mesh.vertices[vCounter + 10] = mesh.vertices[vCounter + 7];
            mesh.vertices[vCounter + 11] = mesh.vertices[vCounter + 8];

            mesh.vertices[vCounter + 12] = mesh.vertices[vCounter + 3];
            mesh.vertices[vCounter + 13] = mesh.vertices[vCounter + 4];
            mesh.vertices[vCounter + 14] = mesh.vertices[vCounter + 5];

            mesh.vertices[vCounter + 15] = (float)(x + 1)*scaleFactor.x;
            mesh.vertices[vCounter + 16] = GRAY_VALUE(pixels[(x + 1) + (z + 1)*mapX])*scaleFactor.y;
            mesh.vertices[vCounter + 17] = (float)(z + 1)*scaleFactor.z;
            vCounter += 18;     // 6 vertex, 18 floats

            // Fill texcoords array with data
            //--------------------------------------------------------------


            //get the average height of quad.
            float p1 = GRAY_VALUE(pixels[x + z*mapX])*scaleFactor.y;          
            float p2 = GRAY_VALUE(pixels[x + (z + 1)*mapX])*scaleFactor.y;          
            float p3 = GRAY_VALUE(pixels[(x + 1) + z*mapX])*scaleFactor.y;          
            float p4 = GRAY_VALUE(pixels[(x + 1) + (z + 1)*mapX])*scaleFactor.y;  

            float average = (p1 + p2 + p3 + p4) / 4;


            float startUv = 0.0f;
            float endUv = 0.5f;
            float scale = 2.0f;
            if (average < 27.5f)
            {
                startUv = 0.5f;
                endUv = 1.0f;
            }
        

            mesh.texcoords[tcCounter] =     MapTexture((float)x/(mapX - 1), startUv, endUv, scale);
            mesh.texcoords[tcCounter + 1] = MapTexture((float)z/(mapZ - 1), startUv, endUv, scale);

            mesh.texcoords[tcCounter + 2] = MapTexture((float)x/(mapX - 1), startUv, endUv, scale);
            mesh.texcoords[tcCounter + 3] = MapTexture((float)(z + 1)/(mapZ - 1), startUv, endUv, scale);

            mesh.texcoords[tcCounter + 4] = MapTexture((float)(x + 1)/(mapX - 1), startUv, endUv, scale);
            mesh.texcoords[tcCounter + 5] = MapTexture((float)z/(mapZ - 1), startUv, endUv, scale);

            mesh.texcoords[tcCounter + 6] = mesh.texcoords[tcCounter + 4];
            mesh.texcoords[tcCounter + 7] = mesh.texcoords[tcCounter + 5];

            mesh.texcoords[tcCounter + 8] = mesh.texcoords[tcCounter + 2];
            mesh.texcoords[tcCounter + 9] = mesh.texcoords[tcCounter + 3];

            mesh.texcoords[tcCounter + 10] =MapTexture((float)(x + 1)/(mapX - 1), startUv, endUv, scale);
            mesh.texcoords[tcCounter + 11] =MapTexture((float)(z + 1)/(mapZ - 1), startUv, endUv, scale);
            tcCounter += 12;    // 6 texcoords, 12 floats


            // Fill normals array with data
            //--------------------------------------------------------------
            for (int i = 0; i < 18; i += 9)
            {
                vA.x = mesh.vertices[nCounter + i];
                vA.y = mesh.vertices[nCounter + i + 1];
                vA.z = mesh.vertices[nCounter + i + 2];

                vB.x = mesh.vertices[nCounter + i + 3];
                vB.y = mesh.vertices[nCounter + i + 4];
                vB.z = mesh.vertices[nCounter + i + 5];

                vC.x = mesh.vertices[nCounter + i + 6];
                vC.y = mesh.vertices[nCounter + i + 7];
                vC.z = mesh.vertices[nCounter + i + 8];

                vN = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(vB, vA), Vector3Subtract(vC, vA)));

                mesh.normals[nCounter + i] = vN.x;
                mesh.normals[nCounter + i + 1] = vN.y;
                mesh.normals[nCounter + i + 2] = vN.z;

                mesh.normals[nCounter + i + 3] = vN.x;
                mesh.normals[nCounter + i + 4] = vN.y;
                mesh.normals[nCounter + i + 5] = vN.z;

                mesh.normals[nCounter + i + 6] = vN.x;
                mesh.normals[nCounter + i + 7] = vN.y;
                mesh.normals[nCounter + i + 8] = vN.z;
            }

            nCounter += 18;     // 6 vertex, 18 floats
        }
    }

    UnloadImageColors(pixels);  // Unload pixels color data

    // Upload vertex data to GPU (static mesh)
    UploadMesh(&mesh, false);

    Model model = LoadModelFromMesh(mesh);
    return model;
}