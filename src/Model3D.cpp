#include "Model3D.h"
#include <iostream>

void Model3D::rotateWithAxis(const glm::vec3 &p1, const glm::vec3 &p2, float angle)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float dz = p2.z - p1.z;
    glm::mat4 T1 = AffineTransform3D::translation(-p1.x, -p1.y, -p1.z);
    glm::mat4 R_m = AffineTransform3D::rotation(angle, Axis::X);
    glm::mat4 T2 = AffineTransform3D::translation(p1.x, p1.y, p1.z);

    float d = glm::sqrt(dx*dx + dy*dy + dz*dz); 
    float d_xz = glm::sqrt(dx*dx + dz*dz);
    glm::mat4 Rotation_complex;

    if (abs(dy) < 1e-10 and d_xz > 1e-10){        
        Rotation_complex = 
            AffineTransform3D::rotation(dx, -dz, Axis::Z) * R_m * 
            AffineTransform3D::rotation(dx, dz, Axis::Z);  
    }
    else if (d_xz > 1e-10){
        Rotation_complex = 
            AffineTransform3D::rotation(dx, -dz, Axis::Y) *
            AffineTransform3D::rotation(d_xz, dy, Axis::Z) *
            R_m * 
            AffineTransform3D::rotation(d_xz, -dy, Axis::Z) *
            AffineTransform3D::rotation(dx, dz, Axis::Y);
    }

    glm::mat4 complexTransform = T2 * Rotation_complex * T1;
    accumulatedTransform = complexTransform * accumulatedTransform;
}
    

Model3D::~Model3D(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Model3D::updAxis()
{
    VerticesMatrix initAxisVertices = initAxis.getVertices();
    Vertices newAxis;
    
    for (size_t i = 0; i < initAxisVertices.size(); ++i) {
        glm::vec4 homoCoord = initAxisVertices[i].getHomogeneousCoordinates();
        glm::vec4 transformed = accumulatedTransform * homoCoord;
        newAxis.addVertex(transformed.x,transformed.y,transformed.z);
    }
    currAxis = newAxis;
}

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
    updAxis();
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

void Model3D::draw()
{
    indices.clear();
    vertices.clear();
    dataToDraw();

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceIndices.size() * sizeof(unsigned int), faceIndices.data(), GL_DYNAMIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO);
    //glDrawElements(GL_TRIANGLES, faceIndices.size(), GL_UNSIGNED_INT, 0);
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Model3D::initGL(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}
void Model3D::dataToDraw(){
    VerticesMatrix transformedVertices = currMatrix.getVertices();
    for (auto edge : modelEdges.getEdges()) {
        indices.push_back(edge.getFirst());
        indices.push_back(edge.getSecond());
    }
    for (const auto& vertex : transformedVertices) {
        vertices.push_back(vertex.x());
        vertices.push_back(vertex.y());
        vertices.push_back(vertex.z());
        vertices.insert(vertices.end(), {0.5, 0, 0.5});
    }
}