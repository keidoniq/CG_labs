#pragma once
#include "Vertices.h"
#include "Edges.h"
#include "Faces.h"
#include "AffineTransform3D.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum DrawingMode {
    DRAW_VISIBLE_ONLY = 0,
    DRAW_INVISIBLE_ONLY = 1,
    DRAW_WIREFRAME = 2,
    DRAW_ALL_FACES = 3
};
class Model3D {
private:
    Edges modelEdges;
    Faces modelFaces;
    std::vector<glm::vec3> faceNormalsInit;
    std::vector<glm::vec3> faceNormalsCurr;
    Vertices initAxis;
    Vertices currAxis;
    Vertices initMatrix;
    Vertices currMatrix;
    glm::mat4 accumulatedTransform;

	unsigned int VBO, VAO, EBO;
    std::vector<unsigned int> indices;
    std::vector<float> vertices;

    DrawingMode drawingMode = DrawingMode::DRAW_VISIBLE_ONLY;

	void dataToDraw(const glm::vec3& cameraPos);
    void updAxis();
    void calculateFaceNormals();
    void updateFaceNormals();
    bool isFaceVisible(const glm::vec3& facePoint, const glm::vec3& normal, const glm::vec3& cameraPos) const;

    struct FaceDrawInfo {
        int i1, i2, i3;
        float depth;
        bool visible;
    };
public:
    Model3D(const Vertices& vertices, const Faces& faces, const Edges& edges, const Vertices& axis):
        initMatrix(vertices), currMatrix(vertices), initAxis(axis) {
            modelEdges = edges;
            modelFaces = faces;
            accumulatedTransform = glm::mat4(1.0f);
            calculateFaceNormals();
            initGL();
    }

    Model3D(const Vertices& vertices, const Faces& faces, const Vertices& axis) {
        Model3D(vertices, faces, faces.getEdgesFromFaces(), axis);
    }
    
    ~Model3D();

    void applyTransformation();
    void resetTransformation();

    void translate(float tx, float ty, float tz);
    void scale(float sx, float sy, float sz);
    void rotate(float angle, Axis rotationAxis);
    void shear(float shx, float shy, float shz);
    void reflect(bool reflectX, bool reflectY, bool reflectZ);
    void rotateWithAxis(const glm::vec3 &p1, const glm::vec3 &p2, float angle);

    DrawingMode getDrawingMode() const { return drawingMode;}
    void setDrawingMode(const DrawingMode otherDrawingMode) { drawingMode = otherDrawingMode;}

    VerticesMatrix getVertices() const { return currMatrix.getVertices(); }
    VerticesMatrix getAxis() const { return currAxis.getVertices(); }
    const Edges& getEdges() const { return modelEdges; }
    const Faces& getFaces() const { return modelFaces; }

	void draw(const glm::vec3& cameraPos = glm::vec3(0.0f, 0.0f, 5.0f));
	void initGL();
};