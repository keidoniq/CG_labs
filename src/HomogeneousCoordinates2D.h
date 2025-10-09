#pragma once
#include <glm/glm.hpp>

class HomogeneousCoordinates2D {
    glm::vec3 coords;
public:

    HomogeneousCoordinates2D(float x = 0.0, float y = 0.0) : coords(x, y, 1.0f) {}
    HomogeneousCoordinates2D(const glm::vec3& vec) : coords(vec) {}
    
    float x() const { return coords.x; }
    float y() const { return coords.y; }
    float z() const { return coords.z; }

    const glm::vec3 getHomogeneousCoordinates() const { return coords; }
    const glm::vec2 getCartesianCoordinates() const {return glm::vec2(coords.x,coords.y);}
};