#include "Model2D.h"

void Model2D::applyTransformation()
{
    Matrix initVertices = initMatrix.getVertices();
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
    accumulatedTransform = AffineTransform2D::scaling(sx, sy)*accumulatedTransform;
}

void Model2D::rotate(float angle)
{
    accumulatedTransform = AffineTransform2D::rotation(angle)*accumulatedTransform;
}

void Model2D::shear(float shx, float shy)
{
    accumulatedTransform = AffineTransform2D::shearing(shx, shy)*accumulatedTransform;
}

void Model2D::reflect(bool reflectX, bool reflectY)
{
    accumulatedTransform = AffineTransform2D::reflection(reflectX, reflectY)*accumulatedTransform;
}
