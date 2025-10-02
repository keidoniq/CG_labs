#include "Edges2D.h"

void Edges2D::addEdge(int vertex1, int vertex2)
{
    edges.emplace(vertex1, vertex2);
}

void Edges2D::addEdge(Edge e)
{
    edges.emplace(e);
}

bool Edges2D::delEdge(int vertex1, int vertex2)
{
    return edges.erase(Edge(vertex1, vertex2)) == 0 ? false:true;
}

bool Edges2D::delEdge(Edge e)
{
    return edges.erase(e) == 0 ? false:true;
}

void Edges2D::clear()
{ 
    edges.clear();
}