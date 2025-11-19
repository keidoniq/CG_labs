#pragma once
#include <vector>
#include <algorithm>

struct FaceStruct {
private:
    std::vector<int> vertices;

public:
    FaceStruct(int v1, int v2, int v3) {
        vertices = {v1, v2, v3};
        std::sort(vertices.begin(), vertices.end());
    }

    int getV1() const { return vertices[0]; }
    int getV2() const { return vertices[1]; }
    int getV3() const { return vertices[2]; }
    
    const std::vector<int>& getFaceVertices() const { return vertices; }
    
    bool operator<(const FaceStruct& other) const {
        return vertices < other.vertices;
    }
    
    bool operator==(const FaceStruct& other) const {
        return vertices == other.vertices;
    }
    bool operator!=(const FaceStruct& other) const {
        return vertices != other.vertices;
    }
};