#pragma once
#include "Vertices2D.h"
#include "Edges.h"
#include "AffineTransform2D.h"
// Примерная структура класса Model2D.
// Вершины модели хранятся в матрице размера 3×M (где M – число вершин модели). В каждом столбце хранятся однородные координаты соответствующей вершины.
// Рёбра модели хранятся:
// в целочисленной матрице размера K×2 (где K – число рёбер) – при этом в каждой строке матрицы хранятся номера двух вершин, соединённых рёбрами;

// Кроме того, необходимо реализовать метод Apply(Matrix AT) применения к модели аффинного преобразования, заданного матрицей AT. В памяти в качестве полей должны храниться:
// • исходная матрица вершин модели;
// • матрица накопленного аффинного преобразования;
// • матрица текущих вершин модели.
class Model2D {
private:
    Vertices2D initMatrix;
    Vertices2D currMatrix;
    Edges edges;
    glm::mat3 accumulatedTransform;

    bool isDragging;
    glm::vec2 dragPos;

    glm::mat3 axisTransform(const glm::vec2& p1, const glm::vec2& p2, const glm::mat3& M);
public:
    Model2D(const Vertices2D& vertices, const Edges& edges):
        initMatrix(vertices), currMatrix(vertices), 
        edges(edges), accumulatedTransform(glm::mat3(1.0f)), isDragging(false) {}
    
    void applyTransformation();
    void resetTransformation();
    Vertices2D getVertices() const { return currMatrix; }

    void translate(float tx, float ty);
    void scale(float sx, float sy);
    void rotate(float angle);
    void shear(float shx, float shy);
    void reflect(bool reflectX, bool reflectY);

    void reflectWithAxis(const glm::vec2 &p1, const glm::vec2 &p2, bool reflectX, bool reflectY);
    void scaleWithAxis(const glm::vec2& p1, const glm::vec2& p2, float sx, float sy);
    void shearWithAxis(const glm::vec2& p1, const glm::vec2& p2, float shx, float shy);

    void startDrag(const glm::vec2& worldPos);
    void drag(const glm::vec2& worldPos);
    void endDrag();
};