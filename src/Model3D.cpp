#include "Model3D.h"
#include <iostream>

void Model3D::calculateFaceNormals()
{
    faceNormalsInit.clear();

    auto verts = initMatrix.getVertices();
    for (const auto& face : modelFaces.getFaces()) {
        glm::vec3 v1 = verts[face.getV1()].getCartesianCoordinates();
        glm::vec3 v2 = verts[face.getV2()].getCartesianCoordinates();
        glm::vec3 v3 = verts[face.getV3()].getCartesianCoordinates();

        glm::vec3 n = glm::normalize(glm::cross(v2 - v1, v3 - v1));
        faceNormalsInit.push_back(n);
    }

    faceNormalsCurr = faceNormalsInit;
}

void Model3D::updateFaceNormals()
{
    faceNormalsCurr.clear();

    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(accumulatedTransform)));
    for (const auto& n : faceNormalsInit) {
        faceNormalsCurr.push_back(glm::normalize(normalMatrix * n));
    }
}

bool Model3D::isFaceVisible(
    const glm::vec3& facePoint,
    const glm::vec3& normal,
    const glm::vec3& cameraPos) const
{
    glm::vec3 viewDir = cameraPos - facePoint;
    return glm::dot(glm::normalize(normal), glm::normalize(viewDir)) > 0.0f;
}

void Model3D::rotateWithAxis(const glm::vec3 &p1, const glm::vec3 &p2, float angle)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float dz = p2.z - p1.z;
    glm::mat4 T1 = AffineTransform3D::translation(-p1.x, -p1.y, -p1.z);
    glm::mat4 R_m = AffineTransform3D::rotation(angle, Axis::X);
    glm::mat4 T2 = AffineTransform3D::translation(p1.x, p1.y, p1.z);

    float d = glm::sqrt(dx*dx + dy*dy + dz*dz); 
    float d_xz = glm::sqrt(dx*dx + dz*dz);
    glm::mat4 Rotation_complex;

    if (abs(dy) < 1e-10 and d_xz > 1e-10){        
        Rotation_complex = 
            AffineTransform3D::rotation(dx, -dz, Axis::Z) * R_m * 

            AffineTransform3D::rotation(dx, dz, Axis::Z);  
    }
    else if (d_xz > 1e-10){
        Rotation_complex = 
            AffineTransform3D::rotation(dx, -dz, Axis::Y) *
            AffineTransform3D::rotation(d_xz, dy, Axis::Z) *
            R_m * 
            AffineTransform3D::rotation(d_xz, -dy, Axis::Z) *
            AffineTransform3D::rotation(dx, dz, Axis::Y);
    }

    glm::mat4 complexTransform = T2 * Rotation_complex * T1;
    accumulatedTransform = complexTransform * accumulatedTransform;
}
    

Model3D::~Model3D(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Model3D::updAxis()
{
    VerticesMatrix initAxisVertices = initAxis.getVertices();
    Vertices newAxis;
    
    for (size_t i = 0; i < initAxisVertices.size(); ++i) {
        glm::vec4 homoCoord = initAxisVertices[i].getHomogeneousCoordinates();
        glm::vec4 transformed = accumulatedTransform * homoCoord;
        newAxis.addVertex(transformed.x,transformed.y,transformed.z);
    }
    currAxis = newAxis;
}

void Model3D::applyTransformation()
{
    VerticesMatrix initVertices = initMatrix.getVertices();
    Vertices newVertices;
    
    for (size_t i = 0; i < initVertices.size(); ++i) {
        glm::vec4 homoCoord = initVertices[i].getHomogeneousCoordinates();
        glm::vec4 transformed = accumulatedTransform * homoCoord;
        newVertices.addVertex(transformed.x,transformed.y, transformed.z);
    }
    currMatrix = newVertices;
    updAxis();    
    updateFaceNormals();
}

void Model3D::resetTransformation()
{
    accumulatedTransform = AffineTransform3D::identity();
}

void Model3D::translate(float tx, float ty, float tz)
{
    accumulatedTransform = AffineTransform3D::translation(tx,ty,tz) * accumulatedTransform;
}

void Model3D::scale(float sx, float sy, float sz)
{
    accumulatedTransform = AffineTransform3D::scaling(sx, sy, sz) * accumulatedTransform;
}

void Model3D::rotate(float angle, Axis rotationAxis)
{
    accumulatedTransform = AffineTransform3D::rotation(angle, rotationAxis) * accumulatedTransform;
}

void Model3D::shear(float shx, float shy, float shz)
{
    accumulatedTransform = AffineTransform3D::shearing(shx, shy, shz) * accumulatedTransform;
}

void Model3D::reflect(bool reflectX, bool reflectY, bool reflectZ)
{
    accumulatedTransform = AffineTransform3D::reflection(reflectX, reflectY, reflectZ) * accumulatedTransform;
}

void Model3D::draw(const glm::vec3& cameraPos)
{
    indices.clear();
    vertices.clear();

    dataToDraw(cameraPos);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO);

    if (drawingMode == DRAW_WIREFRAME)
        glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
    else
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Model3D::initGL(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

void Model3D::dataToDraw(const glm::vec3& cameraPos)
{
    indices.clear();
    vertices.clear();

    VerticesMatrix transformedVertices = currMatrix.getVertices();

    for (const auto& v : transformedVertices) {
        vertices.insert(vertices.end(), {v.x(), v.y(),v.z()});
        vertices.insert(vertices.end(), {0.f, 0.f, 0.f});
    }

    std::vector<FaceDrawInfo> faces;
    float dMin =  1e9f;
    float dMax = -1e9f;

    size_t faceIndex = 0;
    for (const auto& face : modelFaces.getFaces()) {
        int i1 = face.getV1();
        int i2 = face.getV2();
        int i3 = face.getV3();

        glm::vec3 v1 = transformedVertices[i1].getCartesianCoordinates();
        glm::vec3 v2 = transformedVertices[i2].getCartesianCoordinates();
        glm::vec3 v3 = transformedVertices[i3].getCartesianCoordinates();

        glm::vec3 center = (v1 + v2 + v3) / 3.0f;
        float depth = glm::length(cameraPos - center);

        bool visible = isFaceVisible(center, faceNormalsCurr[faceIndex], cameraPos);

        if (drawingMode == DRAW_ALL_FACES ||
           (drawingMode == DRAW_VISIBLE_ONLY && visible) ||
           (drawingMode == DRAW_INVISIBLE_ONLY && !visible))
        {
            faces.push_back({ i1, i2, i3, depth, visible });
            dMin = std::min(dMin, depth);
            dMax = std::max(dMax, depth);
        }

        ++faceIndex;
    }

    // Painter’s Algorithm
    std::sort(faces.begin(), faces.end(),
        [](const FaceDrawInfo& a, const FaceDrawInfo& b) {
            return a.depth > b.depth; 
        });

    for (const auto& f : faces) {
        float t = (dMax > dMin) ? (f.depth - dMin) / (dMax - dMin) : 0.0f;

        glm::vec3 color = glm::mix(
            glm::vec3(1.f, 0.f, 0.f), // ближний цвет
            glm::vec3(0.f, 0.f, 0.f), // дальний цвет
            t
        );

        for (int idx : {f.i1, f.i2, f.i3}) {
            vertices[idx * 6 + 3] = color.r;
            vertices[idx * 6 + 4] = color.g;
            vertices[idx * 6 + 5] = color.b;
        }

        indices.push_back(f.i1);
        indices.push_back(f.i2);
        indices.push_back(f.i3);
    }

    
    if (drawingMode == DRAW_WIREFRAME) {
        indices.clear();
        for (const auto& e : modelEdges.getEdges()) {
            indices.push_back(e.getFirst());
            indices.push_back(e.getSecond());
        }
    }
}
