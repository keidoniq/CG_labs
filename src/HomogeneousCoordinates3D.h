#pragma once
#include <glm/glm.hpp>

class HomogeneousCoordinates3D {
    glm::vec4 coords;
public:

    HomogeneousCoordinates3D(float x = 0.f, float y = 0.f, float z = 0.f) : coords(x, y, z, 1.f) {}
    HomogeneousCoordinates3D(const glm::vec4& vec) : coords(vec) {}
    
    float x() const { return coords.x; }
    float y() const { return coords.y; }
    float z() const { return coords.z; }
    float w() const { return coords.w; }

    const glm::vec4 getHomogeneousCoordinates() const { return coords; }
    const glm::vec3 getCartesianCoordinates() const {return glm::vec3(coords.x,coords.y,coords.z);}
};