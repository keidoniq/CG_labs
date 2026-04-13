#include "Model2D.h"

glm::mat3 Model2D::axisTransform(const glm::vec2 &p1, const glm::vec2 &p2, const glm::mat3 &M)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;

    glm::mat3 T1 = AffineTransform2D::translation(-p1.x, -p1.y);
    glm::mat3 R1 = AffineTransform2D::rotation(dx,-dy);
    glm::mat3 R2 = AffineTransform2D::rotation(dx,dy);
    glm::mat3 T2 = AffineTransform2D::translation(p1.x, p1.y);

    return T2 * R2 * M * R1 * T1;
}

void Model2D::applyTransformation()
{
    VerticesMatrix initVertices = initMatrix.getVertices();
    Vertices2D newVertices;
    
    for (size_t i = 0; i < initVertices.size(); ++i) {
        glm::vec3 homoCoord = initVertices[i].getHomogeneousCoordinates();
        glm::vec3 transformed = accumulatedTransform * homoCoord;
        newVertices.addVertex(transformed.x,transformed.y);
    }

    currMatrix = newVertices;
}

void Model2D::resetTransformation()
{
    accumulatedTransform = AffineTransform2D::identity();
}

void Model2D::translate(float tx, float ty)
{
    accumulatedTransform = AffineTransform2D::translation(tx,ty) * accumulatedTransform;
}

void Model2D::scale(float sx, float sy)
{
    accumulatedTransform = AffineTransform2D::scaling(sx, sy) * accumulatedTransform;
}

void Model2D::rotate(float angle)
{
    accumulatedTransform = AffineTransform2D::rotation(angle) * accumulatedTransform;
}

void Model2D::shear(float shx, float shy)
{
    accumulatedTransform = AffineTransform2D::shearing(shx, shy) * accumulatedTransform;
}

void Model2D::reflect(bool reflectX, bool reflectY)
{
    accumulatedTransform = AffineTransform2D::reflection(reflectX, reflectY) * accumulatedTransform;
}

void Model2D::reflectWithAxis(const glm::vec2 &p1, const glm::vec2 &p2, bool reflectX, bool reflectY)
{
    glm::mat3 R = AffineTransform2D::reflection(reflectX, reflectY);
    accumulatedTransform = axisTransform(p1, p2, R) * accumulatedTransform;
}

void Model2D::scaleWithAxis(const glm::vec2 &p1, const glm::vec2 &p2, float sx, float sy)
{
    glm::mat3 S = AffineTransform2D::scaling(sx, sy);
    accumulatedTransform = axisTransform(p1, p2, S) * accumulatedTransform;
}

void Model2D::shearWithAxis(const glm::vec2 &p1, const glm::vec2 &p2, float shx, float shy)
{
    glm::mat3 S = AffineTransform2D::shearing(shx, shy);
    accumulatedTransform = axisTransform(p1, p2, S) * accumulatedTransform;
}

void Model2D::rotateAroundPoint(const glm::vec2& point, float angle)
{
    glm::mat3 T1 = AffineTransform2D::translation(-point.x, -point.y);
    glm::mat3 R  = AffineTransform2D::rotation(angle);
    glm::mat3 T2 = AffineTransform2D::translation(point.x, point.y);

    accumulatedTransform = T2 * R * T1 * accumulatedTransform;
}

void Model2D::scaleAroundPoint(const glm::vec2& point, float sx, float sy)
{
    glm::mat3 T1 = AffineTransform2D::translation(-point.x, -point.y);
    glm::mat3 S  = AffineTransform2D::scaling(sx, sy);
    glm::mat3 T2 = AffineTransform2D::translation(point.x, point.y);

    accumulatedTransform = T2 * S * T1 * accumulatedTransform;
}

void Model2D::shearAroundPoint(const glm::vec2& point, float shx, float shy)
{
    glm::mat3 T1 = AffineTransform2D::translation(-point.x, -point.y);
    glm::mat3 Sh = AffineTransform2D::shearing(shx, shy);
    glm::mat3 T2 = AffineTransform2D::translation(point.x, point.y);

    accumulatedTransform = T2 * Sh * T1 * accumulatedTransform;
}

void Model2D::rotateAroundCenter(float angle)
{
    VerticesMatrix vertices = currMatrix.getVertices();

    if (vertices.empty()) return;

    float sumX = 0.0f;
    float sumY = 0.0f;

    for (const auto& v : vertices) {
        glm::vec3 p = v.getHomogeneousCoordinates();
        sumX += p.x;
        sumY += p.y;
    }

    float cx = sumX / vertices.size();
    float cy = sumY / vertices.size();

    glm::mat3 T1 = AffineTransform2D::translation(-cx, -cy);
    glm::mat3 R  = AffineTransform2D::rotation(angle);
    glm::mat3 T2 = AffineTransform2D::translation(cx, cy);

    accumulatedTransform = T2 * R * T1 * accumulatedTransform;
}

void Model2D::startDrag(const glm::vec2 &worldPos)
{
    isDragging = true;
    dragPos = worldPos;
}

void Model2D::drag(const glm::vec2 &worldPos)
{
    if (!isDragging) return;
    
    glm::vec2 delta = worldPos - dragPos;
    translate(delta.x, delta.y);
    dragPos = worldPos; 
}

void Model2D::endDrag()
{
    isDragging = false;
}