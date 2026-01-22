#include "Faces.h"

void Faces::addFace(int vertex1, int vertex2, int vertex3) {
    faces.emplace_back(vertex1, vertex2, vertex3);
}

void Faces::addFace(FaceStruct face) {
    faces.emplace_back(face);
} 

void Faces::delFace(int vertex1, int vertex2, int vertex3) {
    faces.erase(std::remove(faces.begin(), faces.end(), FaceStruct(vertex1, vertex2, vertex3)), faces.end());
}

void Faces::delFace(FaceStruct face) {
    faces.erase(std::remove(faces.begin(), faces.end(), face), faces.end());
}

void Faces::clear() {
    faces.clear();
}

Edges Faces::getEdgesFromFaces() const {
    Edges edges = Edges();
    std::vector<FaceStruct> sorted_faces;
        
    for (const auto& face : faces) {        
        std::vector<int> vertices = face.getFaceVertices();
        std::sort(vertices.begin(), vertices.end());

        edges.addEdge(vertices[0], vertices[1]);
        edges.addEdge(vertices[1], vertices[2]);
        edges.addEdge(vertices[0], vertices[2]);
    }
    
    return edges;
}