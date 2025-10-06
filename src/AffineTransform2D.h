#pragma once
#include <glm/glm.hpp>
#include <cmath>

class AffineTransform2D {
public:
    static glm::mat3 identity();
    static glm::mat3 translation(float tx, float ty);
    static glm::mat3 scaling(float sx, float sy);
    static glm::mat3 rotation(float angle);
    //DONE another rotation -> c, s -> norm them like sin cos
    static glm::mat3 rotation(float c, float s);
    static glm::mat3 shearing(float shx, float shy);
    static glm::mat3 reflection(bool reflectX, bool reflectY);
};