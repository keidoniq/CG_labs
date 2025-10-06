#include "Edges.h"

void Edges::addEdge(int vertex1, int vertex2)
{
    edges.emplace(vertex1, vertex2);
}

void Edges::addEdge(Edge e)
{
    edges.emplace(e);
}

bool Edges::delEdge(int vertex1, int vertex2)
{
    return edges.erase(Edge(vertex1, vertex2)) == 0 ? false:true;
}

bool Edges::delEdge(Edge e)
{
    return edges.erase(e) == 0 ? false:true;
}

void Edges::clear()
{ 
    edges.clear();
}