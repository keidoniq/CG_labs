#pragma once
#include "FaceStruct.h"
#include "Edges.h"
#include <vector>

class Faces {
private:
    std::vector<FaceStruct> faces; 

public:
    void addFace(int vertex1, int vertex2, int vertex3);
    void addFace(FaceStruct face);
    void delFace(int vertex1, int vertex2, int vertex3);
    void delFace(FaceStruct face);
    void clear();
    
    const std::vector<FaceStruct>& getFaces() const { return faces; }
    Edges getEdgesFromFaces() const;
};