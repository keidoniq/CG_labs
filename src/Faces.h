#pragma once
#include "FaceStruct.h"
#include "EdgeStruct.h"
#include <vector>
#include <set>

class Faces {
private:
    std::set<FaceStruct> faces; 

public:
    void addFace(int vertex1, int vertex2, int vertex3);
    void addFace(FaceStruct face);
    bool delFace(int vertex1, int vertex2, int vertex3);
    bool delFace(FaceStruct face);
    void clear();
    
    const std::set<FaceStruct>& getFaces() const { return faces; }
    std::set<EdgeStruct> getEdgesFromFaces() const;
};