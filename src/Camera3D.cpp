#include "Camera3D.h"

void Camera3D::setViewport(int width, int height)
{
    W = width;
    H = height;
    m_aspectRatio = float(W) / H;
    maintainAspectRatio();
}

void Camera3D::maintainAspectRatio() {
    float worldWidth = R - L;
    float newWorldHeight = worldWidth / m_aspectRatio;
    float center = (T + B) * 0.5f;
    
    T = center + newWorldHeight * 0.5f;
    B = center - newWorldHeight * 0.5f;
}

void Camera3D::zoom(float factor, const glm::vec2& projPoint) {
    float newWidth = (R - L) / factor;
    float newHeight = (T - B) / factor;
    
    L = projPoint.x - (projPoint.x - L) / factor;
    R = L + newWidth;
    B = projPoint.y - (projPoint.y - B) / factor;
    T = B + newHeight;
    
    maintainAspectRatio();
    setViewport(W, H);
}

void Camera3D::resetCamera() {
    O_vector = glm::vec3(1.5f, 1.f, 1.f);
    N_vector = glm::vec3(0.f, 0.f, 3.f);
    T_vector = glm::vec3(0.f, 1.f, 0.f);
    D = 10.f;
    F = 7.f;

    L = -DEFAULT_DIST;
    R = DEFAULT_DIST;
    B = -DEFAULT_DIST;
    T = DEFAULT_DIST;
    maintainAspectRatio();
}

void Camera3D::moveForward(float distance) {
    O_vector += glm::normalize(N_vector) * distance;
}

void Camera3D::moveBackward(float distance) {
    moveForward(-distance);
}

void Camera3D::moveLeft(float distance) {
    glm::vec3 right = glm::normalize(glm::cross(N_vector, T_vector));
    O_vector -= glm::normalize(right) * distance;
}

void Camera3D::moveRight(float distance) {
    moveLeft(-distance);
}

void Camera3D::moveUp(float distance) {
    O_vector += glm::normalize(T_vector) * distance;
}

void Camera3D::moveDown(float distance) {
    moveUp(-distance);
}