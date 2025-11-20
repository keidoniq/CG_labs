#include "AffineTransform3D.h"

glm::mat4 AffineTransform3D::identity()
{
    return glm::mat4(1.f);
}

glm::mat4 AffineTransform3D::translation(float tx, float ty, float tz)
{
    return glm::mat4(
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        tx,   ty,  tz, 1.f
    );
}

glm::mat4 AffineTransform3D::scaling(float sx, float sy, float sz) {
    return glm::mat4(
         sx, 0.f, 0.f, 0.f,
        0.f,  sy, 0.f, 0.f,
        0.f, 0.f,  sz, 0.f,
        0.f, 0.f, 0.f, 1.f
    );
}

glm::mat4 AffineTransform3D::rotationOnAngleAroundAxis(float cos_phi, float sin_phi, Axis rAxis)
{
    switch(rAxis) {
        case X:
            return glm::mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, cos_phi, sin_phi, 0.0f,
                0.0f, -sin_phi, cos_phi, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            );
        case Y:
            return glm::mat4(
                cos_phi, 0.0f, -sin_phi, 0.0f,
                0.0f, 1.0f, 0.0f,  0.0f,
                sin_phi, 0.0f, cos_phi,  0.0f,
                0.0f, 0.0f, 0.0f,  1.0f
            );
        case Z:
            return glm::mat4(
                cos_phi,  sin_phi, 0.0f, 0.0f,
                -sin_phi, cos_phi, 0.0f, 0.0f,
                0.0f,  0.0f, 1.0f, 0.0f,
                0.0f,  0.0f, 0.0f, 1.0f
            );
        default:
            return identity();
    }
}

glm::mat4 AffineTransform3D::rotation(float angle, Axis rAxis)
{
    return rotationOnAngleAroundAxis(glm::cos(angle), glm::sin(angle), rAxis);
}

glm::mat4 AffineTransform3D::rotation(float c, float s, Axis rAxis)
{
    float norm = 1.f/glm::sqrt(c*c + s*s);
    float cos_phi = c*norm;
    float sin_phi = s*norm;

    return rotationOnAngleAroundAxis(cos_phi, sin_phi, rAxis);
}

glm::mat4 AffineTransform3D::shearing(float shx, float shy, float shz) {
    return glm::mat4(
        1.f,  shy,  shz,  0.f,
        shx,  1.f,  shz,  0.f,
        shx,  shy,  1.f,  0.f,
        0.f,  0.f,  0.f,  1.f
    );
}

glm::mat4 AffineTransform3D::reflection(bool reflectX, bool reflectY, bool reflectZ) {
    float sx = reflectX ? 1.0f : -1.0f;
    float sy = reflectY ? 1.0f : -1.0f;
    float sz = reflectZ ? 1.0f : -1.0f;

    return scaling(sx, sy, sz);
}