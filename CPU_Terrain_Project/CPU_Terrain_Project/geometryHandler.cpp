#include "geometryHandler.h"
#include "utilities.h"

#define M_PI 3.14159265358979323846


// --- SKYBOX --- //
vector<float> generateSkyboxCube() {
    vector<float> skyboxVertices = vector<float>{
        -1.0f,  1.0f, -1.0f,  // fronte
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,  // retro
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,  // destra
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,  // sinistra
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,  // alto
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,  // basso
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    return skyboxVertices;
}




// --- PLANE --- //
void generateTerrain(int division, float width, const vector<float>& fbmData, int fbmWidth, 
    int fbmHeight, vector<Vertex>& vertices, vector<unsigned int>& indices) {
    
    vertices.clear();
    indices.clear();

    float triangleSide = width / division;

    // Crea griglia base
    for (int row = 0; row <= division; row++) {
        for (int col = 0; col <= division; col++) {
            vec3 pos(col * triangleSide, 0.0f, row * -triangleSide);
            vec2 uv((float)col / division, (float)row / division);

            vertices.push_back({ pos, vec3(0,1,0), uv});
        }
    }

    // Applica displacement usando FBM
    applyDisplacement(vertices, fbmWidth, fbmHeight, fbmData);

    // Indici (2 triangoli per cella)
    for (int row = 0; row < division; row++) {
        for (int col = 0; col < division; col++) {
            int start = row * (division + 1) + col;
            indices.push_back(start);
            indices.push_back(start + 1);
            indices.push_back(start + division + 1);

            indices.push_back(start + 1);
            indices.push_back(start + division + 2);
            indices.push_back(start + division + 1);
        }
    }
}




// --- PLANE PATCHES --- //
vector<float> generatePatches(const vector<float>& plane, int division) {
    vector<float> patches;
    int rowLength = division + 1;

    for (int row = 0; row < division; ++row) {
        for (int col = 0; col < division; ++col) {
            int v0 = ((row + 1) * rowLength + col) * 3;       // bottom-left
            int v1 = ((row + 1) * rowLength + col + 1) * 3;   // bottom-right
            int v2 = (row * rowLength + col + 1) * 3;         // top-right
            int v3 = (row * rowLength + col) * 3;             // top-left

            patches.push_back(plane[v0]);
            patches.push_back(plane[v0 + 1]);
            patches.push_back(plane[v0 + 2]);

            patches.push_back(plane[v1]);
            patches.push_back(plane[v1 + 1]);
            patches.push_back(plane[v1 + 2]);

            patches.push_back(plane[v2]);
            patches.push_back(plane[v2 + 1]);
            patches.push_back(plane[v2 + 2]);

            patches.push_back(plane[v3]);
            patches.push_back(plane[v3 + 1]);
            patches.push_back(plane[v3 + 2]);
        }
    }

    return patches;
}




// --- SPHERES --- //
vector<vec3> sphereCorners = {
    vec3(0,  1,  0),
    vec3(0,  -1,  0),
    vec3(0, 0,  1),
    vec3(0, 0,  -1),
    vec3(1,  0, 0),
    vec3(-1,  0, 0)
};

const int ottanteTriangles[8][3] = {
    {0, 2, 4}, // Ottante 1
    {0, 3, 4}, // Ottante 2
    {0, 3, 5}, // Ottante 3
    {0, 2, 5}, // Ottante 4
    {1, 2, 4}, // Ottante 5
    {1, 3, 4}, // Ottante 6
    {1, 3, 5}, // Ottante 7
    {1, 2, 5}  // Ottante 8
};

vector<Vertex> generateSphereCPU(const vec3& center, float radius, int latSegments, int lonSegments) {
    vector<Vertex> vertices;
    for (int lat = 0; lat <= latSegments; ++lat) {
        float theta = lat * M_PI / latSegments; // da 0 a pi
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= lonSegments; ++lon) {
            float phi = lon * 2 * M_PI / lonSegments; // da 0 a 2pi
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            vec3 normal = vec3(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
            vec3 pos = center + radius * normal;
            vertices.push_back({ pos, normal });
        }
    }
    return vertices;
}


vector<unsigned int> generateSphereIndices(int latSegments, int lonSegments) {
    vector<unsigned int> indices;
    for (int lat = 0; lat < latSegments; ++lat) {
        for (int lon = 0; lon < lonSegments; ++lon) {
            int current = lat * (lonSegments + 1) + lon;
            int next = current + lonSegments + 1;

            // Primo triangolo
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            // Secondo triangolo
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }
    return indices;
}


void generateGrassBlade(const vec3& pos, const vec3& normal, vector<Vertex>& vertices, vector<unsigned int>& indices, unsigned int& baseIndex) {
    // Calcola tangente arbitrario ortogonale alla normale
    vec3 tangent = normalize(cross(normal, vec3(0, 0, 1)));
    if (length(tangent) < 0.001f)
        tangent = vec3(1, 0, 0);

    // Angolo random (usa std::rand o meglio una funzione random con seme)
    float angle = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * radians(20.0f);

    vec3 dir = normalize(tangent * cos(angle) + cross(normal, tangent) * sin(angle));

    float bladeHeight = 0.015f;
    float bladeWidth = 0.005f;

    vec3 baseLeft = pos - dir * bladeWidth * 0.5f;
    vec3 baseRight = pos + dir * bladeWidth * 0.5f;
    vec3 tip = pos + normal * bladeHeight;

    // Crea i vertici
    vertices.push_back({ baseLeft, normal, vec2(0, 0) });
    vertices.push_back({ baseRight, normal, vec2(1, 0) });
    vertices.push_back({ tip, normal, vec2(0.5f, 1) });

    // Crea un triangolo
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);

    baseIndex += 3;
}


void generateKelp(const vec3& baseCenter, vector<Vertex>& vertices, vector<unsigned int>& indices, 
    unsigned int& baseIndex, float u_time, int kelpSegments, float kelpWidth, float kelpSegmentLength, float kelpOscStrength) {
    
    vec3 up(0.0f, 1.0f, 0.0f);
    vec3 tangent(1.0f, 0.0f, 0.0f);  // direzione base orizzontale X
    vec3 bitangent = normalize(cross(up, tangent));

    float baseAngle = 0.0f; // puoi modificarlo per variare la direzione iniziale

    vec3 p0 = baseCenter;
    vec3 dir = normalize(tangent * cos(baseAngle) + bitangent * sin(baseAngle) + up * 1.0f);
    vec3 side = normalize(cross(dir, up)) * kelpWidth * 0.5f;

    vec3 v0 = p0 - side;
    vec3 v1 = p0 + side;

    for (int i = 0; i < kelpSegments; ++i) {
        float phase = float(i) * 1.3f;
        float oscAngle = sin(u_time * 1.5f + phase) * radians(25.0f);

        vec3 oscillatedDir = normalize(dir + (tangent * cos(oscAngle) + bitangent * sin(oscAngle)) * kelpOscStrength);

        vec3 p1 = p0 + oscillatedDir * kelpSegmentLength;
        vec3 nextSide = normalize(cross(oscillatedDir, up)) * kelpWidth * 0.5f;

        vec3 v2 = p1 - nextSide;
        vec3 v3 = p1 + nextSide;

        // Normale primo triangolo
        vec3 normal1 = normalize(cross(v1 - v0, v2 - v0));
        // Normale secondo triangolo
        vec3 normal2 = normalize(cross(v3 - v1, v2 - v1));

        // Primo triangolo
        vertices.push_back({ v0, normal1 });
        vertices.push_back({ v1, normal1 });
        vertices.push_back({ v2, normal1 });
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        baseIndex += 3;

        // Secondo triangolo
        vertices.push_back({ v2, normal2 });
        vertices.push_back({ v1, normal2 });
        vertices.push_back({ v3, normal2 });
        indices.push_back(baseIndex);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        baseIndex += 3;

        p0 = p1;
        v0 = v2;
        v1 = v3;
        dir = oscillatedDir;
    }
}
