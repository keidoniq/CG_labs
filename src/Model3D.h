#pragma once
#include "Vertices.h"
#include "Edges.h"
#include "Faces.h"
#include "AffineTransform3D.h"

using TriangleFace = std::vector<glm::vec3>;
class Model3D {
private:
    Edges modelEdges;
    Faces modelFaces;
    Vertices initAxis;
    Vertices currAxis;
    Vertices initMatrix;
    Vertices currMatrix;
    glm::mat4 accumulatedTransform;

    void updAxis();
public:
    Model3D(const Vertices& vertices, const Faces& faces, const Edges& edges, const Vertices& axis):
        initMatrix(vertices), currMatrix(vertices), initAxis(axis) {
            modelEdges = edges;
            modelFaces = faces;
            accumulatedTransform = glm::mat4(1.0f);
    }

    Model3D(const Vertices& vertices, const Faces& faces, const Vertices& axis) {
        Model3D(vertices, faces, faces.getEdgesFromFaces(), axis);
    }

    void applyTransformation();
    void resetTransformation();

    void translate(float tx, float ty, float tz);
    void scale(float sx, float sy, float sz);
    void rotate(float angle, Axis rotationAxis);
    void shear(float shx, float shy, float shz);
    void reflect(bool reflectX, bool reflectY, bool reflectZ);
    void rotateWithAxis(const glm::vec3 &p1, const glm::vec3 &p2, float angle);

    VerticesMatrix getVertices() const { return currMatrix.getVertices(); }
    VerticesMatrix getAxis() const { return currAxis.getVertices(); }
    const Edges& getEdges() const { return modelEdges; }
    const Faces& getFaces() const { return modelFaces; }
    std::vector<TriangleFace> getWorldTriangles() const;
};