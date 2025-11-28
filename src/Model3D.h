#pragma once
#include "Vertices.h"
#include "Edges.h"
#include "Faces.h"
#include "AffineTransform3D.h"
/*
Примерная структура класса Model3D.

Вершины модели хранятся в матрице размера 4×M (где M – число вершин модели).
В каждом столбце хранятся однородные координаты соответствующей вершины.

Грани модели хранятся в матрице размера L×3 (где L – число рёбер) –
при этом в каждой строке матрицы хранятся номера трёх вершин, относящихся к одной грани. 
Необходимо реализовать метод построения матрицы рёбер по матрице граней.

Рёбра модели хранятся:
в целочисленной матрице размера K×2 (где K – число рёбер) –
при этом в каждой строке матрицы хранятся номера двух вершин, соединённых рёбрами.

Кроме того, необходимо реализовать метод Apply(Matrix AT) применения к модели аффинного преобразования, заданного матрицей AT. 
В памяти в качестве полей должны храниться:
• исходная матрица вершин модели;
• матрица накопленного аффинного преобразования;
• матрица текущих вершин модели.
Необходимо создать обработчики нажатий на клавиши, вызывающие метод Apply применительно к базовым аффинным преобразованиям.
*/ 
class Model3D {
private:
    Edges modelEdges;
    Faces modelFaces;
    Vertices initMatrix;
    Vertices currMatrix;
    glm::mat4 accumulatedTransform;

    //glm::mat3 axisTransform(const glm::vec2& p1, const glm::vec2& p2, const glm::mat3& M);
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

    // void reflectWithAxis(const glm::vec2 &p1, const glm::vec2 &p2, bool reflectX, bool reflectY);
    // void scaleWithAxis(const glm::vec2& p1, const glm::vec2& p2, float sx, float sy);
    // void shearWithAxis(const glm::vec2& p1, const glm::vec2& p2, float shx, float shy);

    Vertices getVertices() const { return currMatrix; }
    const Edges& getEdges() const { return modelEdges; }
    const Faces& getFaces() const { return modelFaces; }
};