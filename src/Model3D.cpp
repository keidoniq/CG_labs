#include "Model3D.h"

// glm::mat3 Model3D::axisTransform(const glm::vec2 &p1, const glm::vec2 &p2, const glm::mat3 &M)
// {
//     float dx = p2.x - p1.x;
//     float dy = p2.y - p1.y;

//     glm::mat3 T1 = AffineTransform3D::translation(-p1.x, -p1.y);
//     glm::mat3 R1 = AffineTransform3D::rotation(dx,-dy);
//     glm::mat3 R2 = AffineTransform3D::rotation(dx,dy);
//     glm::mat3 T2 = AffineTransform3D::translation(p1.x, p1.y);

//     return T2 * R2 * M * R1 * T1;
// }

void Model3D::applyTransformation()
{
    VerticesMatrix initVertices = initMatrix.getVertices();
    Vertices newVertices;
    
    for (size_t i = 0; i < initVertices.size(); ++i) {
        glm::vec4 homoCoord = initVertices[i].getHomogeneousCoordinates();
        glm::vec4 transformed = accumulatedTransform * homoCoord;
        newVertices.addVertex(transformed.x,transformed.y, transformed.z);
    }

    currMatrix = newVertices;
}

void Model3D::resetTransformation()
{
    accumulatedTransform = AffineTransform3D::identity();
}

void Model3D::translate(float tx, float ty, float tz)
{
    accumulatedTransform = AffineTransform3D::translation(tx,ty,tz) * accumulatedTransform;
}

void Model3D::scale(float sx, float sy, float sz)
{
    accumulatedTransform = AffineTransform3D::scaling(sx, sy, sz) * accumulatedTransform;
}

void Model3D::rotate(float angle, Axis rotationAxis)
{
    accumulatedTransform = AffineTransform3D::rotation(angle, rotationAxis) * accumulatedTransform;
}

void Model3D::shear(float shx, float shy, float shz)
{
    accumulatedTransform = AffineTransform3D::shearing(shx, shy, shz) * accumulatedTransform;
}

void Model3D::reflect(bool reflectX, bool reflectY, bool reflectZ)
{
    accumulatedTransform = AffineTransform3D::reflection(reflectX, reflectY, reflectZ) * accumulatedTransform;
}

// void Model3D::reflectWithAxis(const glm::vec2 &p1, const glm::vec2 &p2, bool reflectX, bool reflectY)
// {
//     glm::mat3 R = AffineTransform3D::reflection(reflectX, reflectY);
//     accumulatedTransform = axisTransform(p1, p2, R) * accumulatedTransform;
// }

// void Model3D::scaleWithAxis(const glm::vec2 &p1, const glm::vec2 &p2, float sx, float sy)
// {
//     glm::mat3 S = AffineTransform3D::scaling(sx, sy);
//     accumulatedTransform = axisTransform(p1, p2, S) * accumulatedTransform;
// }

// void Model3D::shearWithAxis(const glm::vec2 &p1, const glm::vec2 &p2, float shx, float shy)
// {
//     glm::mat3 S = AffineTransform3D::shearing(shx, shy);
//     accumulatedTransform = axisTransform(p1, p2, S) * accumulatedTransform;
// }

void Model3D::startDrag(const glm::vec3 &worldPos)
{
    isDragging = true;
    dragPos = worldPos;
}

void Model3D::drag(const glm::vec3 &worldPos)
{
    if (!isDragging) return;
    
    glm::vec3 delta = worldPos - dragPos;
    translate(delta.x, delta.y, delta.z);
    dragPos = worldPos; 
}

void Model3D::endDrag()
{
    isDragging = false;
}