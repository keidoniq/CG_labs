#pragma once
#include "HomogeneousCoordinates2D.h"
#include <glm/glm.hpp>
#include <vector>
//DONE VerticesMatrix
using VerticesMatrix = std::vector<HomogeneousCoordinates2D>;

class Vertices2D {
private:
    VerticesMatrix verticesMatrix;
    
public:
    Vertices2D() {};
    
    void addVertex(float x, float y){
        verticesMatrix.push_back(HomogeneousCoordinates2D(x,y));
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