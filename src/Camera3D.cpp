#include "Camera3D.h"

void Camera3D::lookAt(const glm::vec3& target) {
    // Вычисляем новое направление "вперед"
    glm::vec3 newForward = glm::normalize(target - O_vector);
    
    // Если цель прямо над или под камерой
    if (glm::length(newForward) < 0.001f) {
        newForward = glm::vec3(0.0f, 0.0f, -1.0f);
    }
    
    // Вычисляем кватернион вращения
    glm::vec3 worldForward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 rotationAxis = glm::normalize(glm::cross(worldForward, newForward));
    
    if (glm::length(rotationAxis) < 0.001f) {
        // Если векторы коллинеарны (направление вверх или вниз)
        rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    
    float dot = glm::dot(worldForward, newForward);
    float angle = acos(glm::clamp(dot, -1.0f, 1.0f));
    
    orientation = Quaternion::fromAngleAxis(angle, rotationAxis);
    orientation.normalize();
}

// Создание матрицы вида
glm::mat4 Camera3D::getViewMatrix() const {
    glm::vec3 forward = getForward();
    glm::vec3 up = getUp();
    return glm::lookAt(O_vector, O_vector + forward, up);
}

glm::vec3 Camera3D::getForward() const {
    glm::vec3 localForward(0.0f, 0.0f, -1.0f);
    return glm::normalize(orientation.rotateVector(localForward));
}

glm::vec3 Camera3D::getUp() const {
    glm::vec3 localUp(0.0f, 1.0f, 0.0f);
    return glm::normalize(orientation.rotateVector(localUp));
}

glm::vec3 Camera3D::getRight() const {
    glm::vec3 localRight(1.0f, 0.0f, 0.0f);
    return glm::normalize(orientation.rotateVector(localRight));
}

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
    O_vector = default_O_vector;
    orientation = default_orientation;
    D = default_D;    F = default_F;
    L = default_L;    R = default_R;
    B = default_B;    T = default_T;
    maintainAspectRatio();
}

void Camera3D::moveForward(float distance) {
    O_vector += getForward() * distance;
}

void Camera3D::moveBackward(float distance) {
    moveForward(-distance);
}

void Camera3D::moveLeft(float distance) {
    moveRight(-distance);
}

void Camera3D::moveRight(float distance) {
    O_vector += getRight() * distance;
}

void Camera3D::moveUp(float distance) {
    O_vector += getUp() * distance;
}

void Camera3D::moveDown(float distance) {
    moveUp(-distance);
}

void Camera3D::rotate(float angle, const glm::vec3& axis) {
    Quaternion delta = Quaternion::fromAngleAxis(angle, axis);
    orientation = delta * orientation;
    orientation.normalize();
}

void Camera3D::rotateLocalX(float angle) {
    Quaternion delta = Quaternion::fromAngleAxis(angle, getRight());
    orientation = delta * orientation;
    orientation.normalize();
}

void Camera3D::rotateLocalY(float angle) {
    Quaternion delta = Quaternion::fromAngleAxis(angle, getUp());
    orientation = delta * orientation;
    orientation.normalize();
}

void Camera3D::rotateLocalZ(float angle) {
    Quaternion delta = Quaternion::fromAngleAxis(angle, getForward());
    orientation = delta * orientation;
    orientation.normalize();
}

glm::vec2 Camera3D::normalizedToScreen(const glm::vec4& normalizedPos) const {
    glm::vec2 screenPos;
    screenPos.x = 0.5f * W * (1.0f + normalizedPos.x);
    screenPos.y = 0.5f * H * (1.0f + normalizedPos.y);
    return screenPos;
}

glm::vec4 Camera3D::normalizedToWorld(const glm::vec4& normalizedPos) const {
    glm::mat4 invProjection = glm::inverse(getNormalizedProjectionMatrix());
    glm::mat4 invView = glm::inverse(getViewMatrix());
    glm::vec4 viewPos = invProjection * normalizedPos;
    glm::vec4 worldPos = invView * viewPos;
    
    return worldPos;
}

glm::vec2 Camera3D::screenToProj(const glm::vec2& screenPos) const {
    glm::vec2 projPos;
    projPos.x = 0.5f * (L + R + (R - L) * (2.0f * screenPos.x / W - 1.0f));
    projPos.y = 0.5f * (T + B + (T - B) * (1.0f - 2.0f * screenPos.y / H));
    return projPos;
}

glm::vec2 Camera3D::projToScreen(const glm::vec2& projPos) const {
    float r_l = 1.0f / (R - L);
    float t_b = 1.0f / (T - B);
    glm::vec2 screenPos;
    screenPos.x = 0.5f * W * (1.0f + (2.0f * projPos.x - (L + R)) * r_l);
    screenPos.y = 0.5f * H * (1.0f + ((T + B) - 2.0f * projPos.y) * t_b);
    return screenPos;
}

glm::vec2 Camera3D::worldToScreen(const glm::vec3& worldPos) const {
    glm::vec4 viewPos = worldToView(worldPos);
    glm::vec4 normalizedPos = viewToNormalized(viewPos);
    glm::vec2 screenPos = normalizedToScreen(normalizedPos);
    return screenPos;
}

TriangleFace Camera3D::projectTriangle(const TriangleFace& worldTri) const {
    return {
        worldToScreenWithDepth(worldTri[0]),
        worldToScreenWithDepth(worldTri[1]),
        worldToScreenWithDepth(worldTri[2])
    };
}

glm::vec3 Camera3D::worldToScreenWithDepth(const glm::vec3& worldPos) const {
    glm::vec4 viewPos = worldToView(worldPos);
    glm::vec4 clip = getNormalizedProjectionMatrix() * viewPos;

    if (std::abs(clip.w) < 1e-6f)
        return glm::vec3(0);

    glm::vec3 ndc = glm::vec3(clip) / clip.w;

    glm::vec2 screen = normalizedToScreen(glm::vec4(ndc, 1.0f));
    return glm::vec3(screen, viewPos.z); // z — глубина в видовых координатах
}