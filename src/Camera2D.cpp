#include "Camera2D.h"

Camera2D::~Camera2D()
{
    glDeleteVertexArrays(1, &axisVAO);
    glDeleteBuffers(1, &axisVBO);
}

void Camera2D::setViewport(int width, int height)
{
    W = width;
    H = height;
    maintainAspectRatio();
    glViewport(0, 0, W, H);
    updAxes();
}

void Camera2D::maintainAspectRatio() {
    float worldWidth = R - L;
    float aspect = float(W) / H;
    float newWorldHeight = worldWidth / aspect;
    float center = (T + B) * 0.5f;
    
    T = center + newWorldHeight * 0.5f;
    B = center - newWorldHeight * 0.5f;
}

glm::vec2 Camera2D::screenToWorld(const glm::vec2& screenPos) const {
    float wx = L + (screenPos.x / W) * (R - L);
    float wy = B + ((H - screenPos.y) / H) * (T - B); 
    return glm::vec2(wx, wy);
}

glm::vec2 Camera2D::worldToScreen(const glm::vec2& worldPos) const {
    float sx = ((worldPos.x - L) / (R - L)) * W;
    float sy = ((T - worldPos.y) / (T - B)) * H;
    return glm::vec2(sx, sy);
}

void Camera2D::zoom(float factor, const glm::vec2& worldPoint) {
    float newWidth = (R - L) / factor;
    float newHeight = (T - B) / factor;
    
    L = worldPoint.x - (worldPoint.x - L) / factor;
    R = L + newWidth;
    B = worldPoint.y - (worldPoint.y - B) / factor;
    T = B + newHeight;
    
    maintainAspectRatio();
    setViewport(W, H);
}

void Camera2D::startDrag(const glm::vec2& worldPos) {
    isDragging = true;
    dragStartWorld = worldPos;
    startBounds = glm::vec4(L, R, B, T);
}

void Camera2D::drag(const glm::vec2& worldPos) {
    if (!isDragging) return;
    
    glm::vec2 delta = worldPos - dragStartWorld;
    
    L = startBounds.x - delta.x;
    R = startBounds.y - delta.x;
    B = startBounds.z - delta.y;
    T = startBounds.w - delta.y;
    
    updAxes();
}

void Camera2D::endDrag() {
    isDragging = false;
}

void Camera2D::clear() const {
    glClear(GL_COLOR_BUFFER_BIT);
}

void Camera2D::updAxes() {
    float axisVertices[] = {
        L, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        R, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        0.0f, B, 0.0f,  0.0f, 0.0f, 1.0f,
        0.0f, T, 0.0f,  0.0f, 0.0f, 1.0f,
    };

    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    drawAxes();
}

void Camera2D::drawAxes() const{
    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 4);
}