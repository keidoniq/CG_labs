#pragma once
#include <vector>
#include <array>
#include <limits>
#include <algorithm>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shaders/ShaderModule.h"

// x,y — screen space, z — depth
using ScreenTriangle = std::array<glm::vec3, 3>;

class ZBufferRenderer {
private:
    int width, height;

    std::vector<float> zBuffer;
    std::vector<glm::vec3> colorBuffer;
    std::vector<ScreenTriangle> trianglesToDraw;
    std::vector<std::array<glm::vec3, 2>> linesToDraw;
    std::vector<glm::vec3> trianglesToDrawColors;
    std::vector<glm::vec3> linesToDrawColors;

    GLuint textureID;
    GLuint quadVAO, quadVBO, quadEBO;
    ShaderModule* screenShader = nullptr;    
public:
    ZBufferRenderer(int w, int h, ShaderModule* shader) : width(w), height(h), screenShader(shader) {
        zBuffer.resize(width * height, std::numeric_limits<float>::max());
        colorBuffer.resize(width * height, glm::vec3(0.0f));

        initFullscreenQuad();
        initTexture();
        clear();
    }
    ~ZBufferRenderer() {
        glDeleteTextures(1, &textureID);
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
        glDeleteBuffers(1, &quadEBO);
    }

    void resize(int w, int h) {
        width = w;
        height = h;
        zBuffer.resize(width * height, std::numeric_limits<float>::max());
        colorBuffer.resize(width * height, glm::vec3(0.89f, 0.93f, 0.98f));
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    
    void clear(const glm::vec3& backgroundColor = glm::vec3(0.89f, 0.93f, 0.98f)) {
        trianglesToDraw.clear();
        linesToDraw.clear();
        trianglesToDrawColors.clear();
        linesToDrawColors.clear();;
        std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::max());
        std::fill(colorBuffer.begin(), colorBuffer.end(), backgroundColor);
    }

    void addModelTriangles(const Model3D& model, const Camera3D& camera, const glm::vec3& color) {
        std::vector<TriangleFace> worldTriangles = model.getWorldTriangles();
        
        for (const auto& worldTri : worldTriangles) {
            std::array<glm::vec3, 3> screenTri;
            for (int i = 0; i < 3; i++)
                screenTri[i] = camera.worldToScreenWithDepth(worldTri[i]);

            trianglesToDraw.push_back(screenTri);
            trianglesToDrawColors.push_back(color);
        }
    }

    void addModelEdges(const Model3D& model, const Camera3D& camera, const glm::vec3& color = glm::vec3(0.f, 0.f, 0.f)) {
        VerticesMatrix vertices = model.getVertices();
        auto& edges = model.getEdges().getEdges();
        
        for (const auto& edge : edges) {
            glm::vec3 v1 = vertices[edge.getFirst()].getCartesianCoordinates();
            glm::vec3 v2 = vertices[edge.getSecond()].getCartesianCoordinates();
            
            glm::vec3 screen1 = camera.worldToScreenWithDepth(v1);
            glm::vec3 screen2 = camera.worldToScreenWithDepth(v2);
            
            linesToDraw.push_back({screen1, screen2});
            linesToDrawColors.push_back(color);
        }
    }
    
    void drawElements() {
        for (size_t i = 0; i < trianglesToDraw.size(); i++)
            rasterizeTriangle(trianglesToDraw[i], trianglesToDrawColors[i]);
        
        for (size_t i = 0; i < linesToDraw.size(); i++)
            drawLine(linesToDraw[i][0], linesToDraw[i][1], linesToDrawColors[i]);
    }

    void renderToScreen() {
        screenShader->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, colorBuffer.data());
        
        glBindVertexArray(quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        
        glBindVertexArray(0);
        screenShader->release();
    }

    void drawAxes(const Camera3D& camera, float length = 2.0f) {
        glm::vec3 origin(0.0f, 0.0f, 0.0f);
        glm::vec3 xAxis(length, 0.0f, 0.0f);
        glm::vec3 yAxis(0.0f, length, 0.0f);
        glm::vec3 zAxis(0.0f, 0.0f, length);

        glm::vec3 screenOrigin = camera.worldToScreenWithDepth(origin);
        glm::vec3 screenX = camera.worldToScreenWithDepth(xAxis);
        glm::vec3 screenY = camera.worldToScreenWithDepth(yAxis);
        glm::vec3 screenZ = camera.worldToScreenWithDepth(zAxis);

        drawLine(screenOrigin, screenX, glm::vec3(1.0f, 0.0f, 0.0f)); // X красный
        drawLine(screenOrigin, screenY, glm::vec3(0.0f, 1.0f, 0.0f)); // Y зелёный
        drawLine(screenOrigin, screenZ, glm::vec3(0.0f, 0.0f, 1.0f)); // Z синий
    }
    
    
private:
    void rasterizeTriangle(const ScreenTriangle& tri, const glm::vec3& color) {
        float minDepth = 0.f;
        float maxDepth = 20.f;
        //bounding box
        int minX = std::max(0, (int)std::floor(std::min({tri[0].x, tri[1].x, tri[2].x})));
        int minY = std::max(0, (int)std::floor(std::min({tri[0].y, tri[1].y, tri[2].y})));
        int maxX = std::min(width-1, (int)std::ceil(std::max({tri[0].x, tri[1].x, tri[2].x})));
        int maxY = std::min(height-1, (int)std::ceil(std::max({tri[0].y, tri[1].y, tri[2].y})));
        // векторы для барицентрических координат
        glm::vec2 v0(tri[0].x, tri[0].y);
        glm::vec2 v1(tri[1].x, tri[1].y);
        glm::vec2 v2(tri[2].x, tri[2].y);

        float invDenom = 1.0f / ((v1.y - v2.y) * (v0.x - v2.x) + (v2.x - v1.x) * (v0.y - v2.y));
        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                glm::vec2 p(x + 0.5f, y + 0.5f);
                
                float w0 = ((v1.y - v2.y) * (p.x - v2.x) + (v2.x - v1.x) * (p.y - v2.y)) * invDenom;
                float w1 = ((v2.y - v0.y) * (p.x - v2.x) + (v0.x - v2.x) * (p.y - v2.y)) * invDenom;
                float w2 = 1.0f - w0 - w1;
                
                if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                    float depth  = w0 * tri[0].z + w1 * tri[1].z + w2 * tri[2].z;
                    int idx = y * width + x;

                    if (depth < zBuffer[idx]) {
                        zBuffer[idx] = depth;
                        colorBuffer[idx] = color;
                    }
                }
            }
        }
    }
    void drawLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& color) {        
        int x1 = (int)p1.x, y1 = (int)p1.y;
        int x2 = (int)p2.x, y2 = (int)p2.y;
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;
        
        while (true) {
            if (x1 >= 0 && x1 < width && y1 >= 0 && y1 < height) {
                int idx = y1 * width + x1;
                float t = glm::length(glm::vec2(x1 - p1.x, y1 - p1.y)) / 
                         glm::length(glm::vec2(p2.x - p1.x, p2.y - p1.y));
                float depth = p1.z + t * (p2.z - p1.z);
                
                if (depth < zBuffer[idx]) {
                    zBuffer[idx] = depth;
                    colorBuffer[idx] = color;
                }
            }
            
            if (x1 == x2 && y1 == y2) break;
            
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x1 += sx; }
            if (e2 < dx) { err += dx; y1 += sy; }
        }
    }
    void initTexture()
    {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, 
                    GL_RGB, GL_FLOAT, nullptr);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    void initFullscreenQuad()
    {
        float quadVertices[] = {
             // pos         // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };
        unsigned int quadIndices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glGenBuffers(1, &quadEBO);
        
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
        // position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
};