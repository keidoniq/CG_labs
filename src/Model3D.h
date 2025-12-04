#pragma once
#include "Vertices.h"
#include "Edges.h"
#include "Faces.h"
#include "AffineTransform3D.h"
class Model3D {
private:
    Edges modelEdges;
    Faces modelFaces;
    Vertices initMatrix;
    Vertices currMatrix;
    glm::mat4 accumulatedTransform;
public:
    Model3D(const Vertices& vertices, const Faces& faces, const Edges& edges):
        initMatrix(vertices), currMatrix(vertices) {
            modelEdges = edges;
            modelFaces = faces;
            accumulatedTransform = glm::mat4(1.0f);
    }

    Model3D(const Vertices& vertices, const Faces& faces) {
        Model3D(vertices, faces, faces.getEdgesFromFaces());
    }
    
    void applyTransformation();
    void resetTransformation();

    void translate(float tx, float ty, float tz);
    void scale(float sx, float sy, float sz);
    void rotate(float angle, Axis rotationAxis);
    void shear(float shx, float shy, float shz);
    void reflect(bool reflectX, bool reflectY, bool reflectZ);

    VerticesMatrix getVertices() const { return currMatrix.getVertices(); }
    const Edges& getEdges() const { return modelEdges; }
    const Faces& getFaces() const { return modelFaces; }
};