#include "Faces.h"

void Faces::addFace(int vertex1, int vertex2, int vertex3) {
    faces.emplace(vertex1, vertex2, vertex3);
}

void Faces::addFace(FaceStruct face) {
    faces.emplace(face);
}

bool Faces::delFace(int vertex1, int vertex2, int vertex3) {
    return faces.erase(FaceStruct(vertex1, vertex2, vertex3)) == 0 ? false:true;
}

bool Faces::delFace(FaceStruct face) {
    return faces.erase(face) == 0 ? false:true;;
}

void Faces::clear() {
    faces.clear();
}

std::set<EdgeStruct> Faces::getEdgesFromFaces() const {
    std::set<EdgeStruct> edges;
    
    for (const auto& face : faces) {
        edges.emplace(face.getV1(), face.getV2());
        edges.emplace(face.getV2(), face.getV3());
        edges.emplace(face.getV1(), face.getV3());
    }
    
    return edges;
}