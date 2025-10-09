#pragma once
#include <set>
#include <utility>
#include <glm/glm.hpp>

// Рёбра модели хранятся в целочисленной матрице размера K×2 (где K – число рёбер)
// – при этом в каждой строке матрицы хранятся номера двух вершин, соединённых рёбрами;
using Edge = std::pair<int,int>;
class Edges {
private:
    std::set<Edge> edges; // K×2 матрица

public:
    void addEdge(int vertex1, int vertex2);
    void addEdge(Edge e);
    bool delEdge(int vertex1, int vertex2);
    bool delEdge(Edge e);
    void clear();
    const std::set<Edge>& getEdges() const { return edges; }
};