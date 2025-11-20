#include "Camera3D.h"

Camera3D::~Camera3D()
{
    glDeleteVertexArrays(1, &axisVAO);
    glDeleteBuffers(1, &axisVBO);
}

void Camera3D::setViewport(int width, int height)
{
    W = width;
    H = height;
    aspectRatio = float(W) / H;
    maintainAspectRatio();
    glViewport(0, 0, W, H);
    updAxes();
}

void Camera3D::maintainAspectRatio() {
    float worldWidth = R - L;
    float newWorldHeight = worldWidth / aspectRatio;
    float center = (T + B) * 0.5f;
    
    T = center + newWorldHeight * 0.5f;
    B = center - newWorldHeight * 0.5f;
}

void Camera3D::updStartBounds(float startL, float startR, float startT, float startB)
{
    startBounds.at("L") = startL;
    startBounds.at("B") = startB;
    startBounds.at("R") = startR;
    startBounds.at("T") = startT;
}

// glm::vec2 Camera3D::screenToWorld(const glm::vec2& screenPos) const {
//     float wx = L + (screenPos.x / W) * (R - L);
//     float wy = B + ((H - screenPos.y) / H) * (T - B); 
//     return glm::vec2(wx, wy);
// }

// glm::vec2 Camera3D::worldToScreen(const glm::vec2& worldPos) const {
//     float sx = ((worldPos.x - L) / (R - L)) * W;
//     float sy = ((T - worldPos.y) / (T - B)) * H;
//     return glm::vec2(sx, sy);
// }

void Camera3D::zoom(float factor, const glm::vec3& worldPoint) {
    float factor_multi = 1./factor;
    float minDist = 0.1f;
    float maxDist = 100.0f;
    float currentDist = glm::length(position - target);

    glm::vec3 toCamera = position - worldPoint;
    glm::vec3 toTarget = target - worldPoint;
    position = worldPoint + toCamera*factor_multi;
    target = worldPoint + toTarget*factor_multi;
    
    if (currentDist < minDist) {
        glm::vec3 dir = glm::normalize(target - position);
        position = target - dir * minDist;
    } else if (currentDist > maxDist) {
        glm::vec3 dir = glm::normalize(target - position);
        position = target - dir * maxDist;
    }
    
    maintainAspectRatio();
    setViewport(W,H);
}

void Camera3D::startDrag(const glm::vec3& worldPos) {
    isDragging = true;
    dragStartWorld = worldPos;
    dragStartPosition = position; 
    dragStartTarget = target;
}

void Camera3D::drag(const glm::vec3& worldPos) {
    if (!isDragging) return;
    
    glm::vec3 delta = worldPos - dragStartWorld;
    position = dragStartPosition - delta;
    target = dragStartTarget - delta;
    
    updAxes();
}

void Camera3D::endDrag() {
    isDragging = false;
}

void Camera3D::clear() const {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Camera3D::resetCamera() {
    position = glm::vec3(0.f, 0.f, DEFAULT_DIST);
    target = glm::vec3(0.f);
    up = glm::vec3(0.f, 1.f, 0.f);
    fov = 45.0f;
        
    L = -DEFAULT_DIST;
    R = DEFAULT_DIST;
    B = -DEFAULT_DIST;
    T = DEFAULT_DIST;
    maintainAspectRatio();
}

void Camera3D::updAxes() {
    float axisLength = DEFAULT_DIST;
    float axisVertices[] = {
        // X-axis (red)
        -axisLength, 0.f, 0.f,  1.f, 0.f, 0.f,
        axisLength, 0.f, 0.f,  1.f, 0.f, 0.f,
        // Y-axis (green)
        0.f, -axisLength, 0.f,  0.f, 1.f, 0.f,
        0.f, axisLength, 0.f,  0.f, 1.f, 0.f,
        // Z-axis (blue)
        0.f, 0.f, -axisLength,  0.f, 0.f, 1.f,
        0.f, 0.f, axisLength,  0.f, 0.f, 1.f,
    };
    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    // for position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // for color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    drawAxes();
}

void Camera3D::drawAxes() const{
    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 6);
}