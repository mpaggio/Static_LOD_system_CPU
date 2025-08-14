#include "utilities.h"

// Funzione per generare float casuale tra min e max
float randomFloat(float min, float max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

// Genera posizione random dentro quadrato di lato L
vec3 randomPosition(float L) {
    return vec3(
        randomFloat(0, L), // solo coordinate positive in X
        randomFloat(-L, 0), // y rimane come preferisci (ad esempio altezza)
        randomFloat(L / 5, L / 2) // coordinate negative in Z (perché la mappa va verso -Z)
    );
}


float getHeightFromFBM(float u, float v, int width, int height, const vector<float>& fbmData) {
    float heightScale = 1.5f;
    int ix = clamp(int(u * (width - 1)), 0, width - 1);
    int iy = clamp(int(v * (height - 1)), 0, height - 1);
    return fbmData[iy * width + ix] * heightScale;
}

void applyDisplacement(vector<Vertex>& vertices, int texWidth, int texHeight, const vector<float>& fbmData) {
    for (auto& v : vertices) {
        float h = getHeightFromFBM(v.uv.x, v.uv.y, texWidth, texHeight, fbmData);
        v.pos.y = h;
    }
}