#pragma once
#include "lib.h"
#include "strutture.h"

vector<float> generateSkyboxCube();
void generateTerrain(
    int division, 
    float width, 
    const vector<float>& fbmData, 
    int fbmWidth,
    int fbmHeight, 
    vector<Vertex>& vertices, 
    vector<unsigned int>& indices
);
vector<Vertex> generateSphereCPU(const vec3& center, float radius, int latSegments, int lonSegments);
vector<unsigned int> generateSphereIndices(int latSegments, int lonSegments);
vector<float> generatePatches(const vector<float>& plane, int division);
void generateGrassBlade(
    const vec3& pos, 
    const vec3& normal, 
    vector<Vertex>& vertices, 
    vector<unsigned int>& indices, 
    unsigned int& baseIndex
);
void generateKelp(
    const vec3& baseCenter,
    vector<Vertex>& vertices,
    vector<unsigned int>& indices,
    unsigned int& baseIndex,
    float u_time = 0.0f,
    int kelpSegments = 4,
    float kelpWidth = 0.005f,
    float kelpSegmentLength = 0.02f,
    float kelpOscStrength = 0.2f
);