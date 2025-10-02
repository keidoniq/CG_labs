#include "AffineTransform2D.h"

glm::mat3 AffineTransform2D::identity()
{
    return glm::mat3(
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    );
}

glm::mat3 AffineTransform2D::translation(float tx, float ty)
{
    return glm::mat3(
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        tx, ty, 1.0f
    );
}

glm::mat3 AffineTransform2D::scaling(float sx, float sy) {
    return glm::mat3(
        sx,   0.0f, 0.0f,
        0.0f, sy,   0.0f,
        0.0f, 0.0f, 1.0f
    );
}

glm::mat3 AffineTransform2D::rotation(float angle) {
    float cos_phi = glm::cos(angle);
    float sin_phi = glm::sin(angle);
    
    return glm::mat3(
        cos_phi, -sin_phi, 0.0f,
        sin_phi,  cos_phi,  0.0f,
        0.0f,  0.0f,  1.0f
    );
}

glm::mat3 AffineTransform2D::shearing(float shx, float shy) {
    return glm::mat3(
        1.0f, shy,   0.0f,
        shx,  1.0f,  0.0f,
        0.0f, 0.0f,  1.0f
    );
}

glm::mat3 AffineTransform2D::reflection(bool reflectX, bool reflectY) {
    float sx = reflectX ? -1.0f : 1.0f;
    float sy = reflectY ? -1.0f : 1.0f;
    return scaling(sx, sy);
}