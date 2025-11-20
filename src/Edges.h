#pragma once
#include <set>
#include <utility>
#include <glm/glm.hpp>
#include "EdgeStruct.h"

// Рёбра модели хранятся в целочисленной матрице размера K×2 (где K – число рёбер)
// – при этом в каждой строке матрицы хранятся номера двух вершин, соединённых рёбрами;
class Edges {
private:
    std::set<EdgeStruct> edges;

public:
    void addEdge(int vertex1, int vertex2);
    void addEdge(EdgeStruct e);
    bool delEdge(int vertex1, int vertex2);
    bool delEdge(EdgeStruct e);
    void clear();
    
    
    void setEdges(std::set<EdgeStruct>& otherEdges) { edges = otherEdges; }
    const std::set<EdgeStruct>& getEdges() const { return edges; }
};