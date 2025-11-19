#pragma once
#include "HomogeneousCoordinates3D.h"
#include <glm/glm.hpp>
#include <vector>

using VerticesMatrix = std::vector<HomogeneousCoordinates3D>;

class Vertices {
private:
    VerticesMatrix verticesMatrix;
    
public:
    Vertices() {};
    
    void addVertex(float x, float y, float z){
        verticesMatrix.push_back(HomogeneousCoordinates3D(x,y,z));
    }
    
    VerticesMatrix getVertices() {
        return verticesMatrix; 
    }
    void setVertices(VerticesMatrix otherVerticesMatrix){
        verticesMatrix = otherVerticesMatrix;
    }
    const VerticesMatrix& getVertices() const {
        return verticesMatrix; 
    }
};