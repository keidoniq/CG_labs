#pragma once
#include <glm/glm.hpp>
#include <cmath>

enum Axis {
    X, Y, Z
};
class AffineTransform3D {
    static glm::mat4 rotationOnAngleAroundAxis(float cos_phi, float sin_phi, Axis rAxis);
    
public:
    static glm::mat4 identity();
    static glm::mat4 translation(float tx, float ty, float tz);
    static glm::mat4 scaling(float sx, float sy, float sz);
    static glm::mat4 rotation(float angle, Axis rAxis);
    static glm::mat4 rotation(float c, float s, Axis rAxis);
    static glm::mat4 shearing(float shx, float shy, float shz);
    static glm::mat4 reflection(bool reflectX, bool reflectY, bool reflectZ);
};