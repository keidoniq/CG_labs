#include "Edges.h"

void Edges::addEdge(int vertex1, int vertex2)
{
    edges.emplace(vertex1, vertex2);
}

void Edges::addEdge(EdgeStruct e)
{
    edges.emplace(e);
}

bool Edges::delEdge(int vertex1, int vertex2)
{
    return edges.erase(EdgeStruct(vertex1, vertex2)) == 0 ? false:true;
}

bool Edges::delEdge(EdgeStruct e)
{
    return edges.erase(e) == 0 ? false:true;
}

void Edges::clear()
{ 
    edges.clear();
}