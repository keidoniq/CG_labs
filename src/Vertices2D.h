#pragma once
#include "HomogeneousCoordinates2D.h"
#include <glm/glm.hpp>
#include <vector>
using Matrix = std::vector<HomogeneousCoordinates2D>;

class Vertices2D {
private:
    Matrix verticesMatrix;
    
public:
    Vertices2D() {};
    
    void addVertex(float x, float y){
        verticesMatrix.push_back(HomogeneousCoordinates2D(x,y));
    }
    
    Matrix getVertices() {
        return verticesMatrix; 
    }
    void setVertices(Matrix otherVerticesMatrix){
        verticesMatrix = otherVerticesMatrix;
    }
    const Matrix& getVertices() const {
        return verticesMatrix; 
    }
};