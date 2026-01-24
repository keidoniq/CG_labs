#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "AffineTransform3D.h"
#include <vector>

using TriangleFace = std::vector<glm::vec3>;
const static float DEFAULT_DIST = 5.f;

class Camera3D {
private:
    float m_aspectRatio;
    glm::vec3 O_vector, N_vector, T_vector;
    float F, D;
    int W, H;
    float L, R, B, T;

    glm::vec3 default_O_vector, default_N_vector, default_T_vector;
    float default_L, default_R, default_B, default_T, default_F, default_D;
    int default_W, default_H;
    void maintainAspectRatio();
public:
    Camera3D(int W = 800, int H = 600,
        glm::vec3 N_vector = glm::vec3(0.f, 0.f, 3.f),
        glm::vec3 O_vector = glm::vec3(1.5f, 1.f, 1.f),
        glm::vec3 T_vector = glm::vec3(0.f, 1.f, 0.f),
        float distance = 10.f, float f = 7.f,
        float L = -DEFAULT_DIST, float R = DEFAULT_DIST, 
        float B = -DEFAULT_DIST, float T = DEFAULT_DIST):
        L(L), R(R), B(B), T(T), W(W), H(H), D(distance), F(f),
        O_vector(O_vector), N_vector(N_vector), T_vector(T_vector) {
            setViewport(W, H);
            default_O_vector = O_vector;
            default_T_vector = T_vector;
            default_N_vector = N_vector;
            default_L = L; default_R = R; default_B = B; default_T = T; 
            default_F = F; default_D = D;
            default_W = W; default_H = H;

        };
    
    void zoom(float factor, const glm::vec2& screenPoint);
    void zoomByFocusDistance(float factor){ F *= factor; }

    float getLeft() const { return L; }
    float getRight() const { return R; }
    float getBottom() const { return B; }
    float getTop() const { return T; }
    float getFocusDistance() const { return F; }
    float getDistancce() const { return D; }
    glm::vec3 getO() const { return O_vector; }
    glm::vec3 getN() const { return N_vector; }
    glm::vec3 getT() const { return T_vector; }
    glm::vec4 getViewport() const { return glm::vec4(L, R, B, T); }
    
    void resetCamera();
    void setFocusDistance(float FocusDistance){ F = FocusDistance; }
    void setViewport(int width, int height);

    void moveForward(float distance);
    void moveBackward(float distance);
    void moveLeft(float distance);
    void moveRight(float distance);
    void moveUp(float distance);
    void moveDown(float distance);
    
    glm::mat4 getViewMatrix () const {//из мировых в видовые
        glm::vec3 k = glm::normalize(N_vector);
        glm::vec3 i = glm::normalize(glm::cross(T_vector, N_vector));
        glm::vec3 j = glm::normalize(glm::cross(k, i));
        
        return glm::mat4(
            glm::vec4(i.x, j.x, k.x, 0.f),
            glm::vec4(i.y, j.y, k.y, 0.f), 
            glm::vec4(i.z, j.z, k.z, 0.f),
            glm::vec4(-glm::dot(i, O_vector),  
                      -glm::dot(j, O_vector),  
                      -glm::dot(k, O_vector),  
                      1.f)                      
        );
    }
    glm::mat4 getNormalizedProjectionMatrix() const {//перспективная проекции нормализованные
        float div_f = 1.f/(F);
        float r_l = 1.f/(R - L);
        float t_b = 1.f/(T - B);
        return glm::mat4(
            glm::vec4(2*r_l, 0.f, 0.f, 0.f),
            glm::vec4(0.f, 2*t_b, 0.f, 0.f),
            glm::vec4((L+R)*r_l*div_f, (T+B)*t_b*div_f, -div_f*(2*F+D)/D, -div_f),
            glm::vec4(-(L+R)*r_l, -(T+B)*t_b, -1.f, 1.f)                      
        );
    }
    glm::vec4 worldToView(const glm::vec3& worldPos) const {
        return getViewMatrix() * glm::vec4(worldPos, 1.0f);
    }
    glm::vec4 viewToNormalized(const glm::vec4& viewPos) const {
        glm::vec4 normalizedPos = getNormalizedProjectionMatrix() * viewPos;
        return normalizedPos;
    }
    glm::vec2 normalizedToScreen(const glm::vec4& normalizedPos) const {
        glm::vec2 screenPos;
        screenPos.x = 0.5 * W * (1 + normalizedPos.x);
        screenPos.y = 0.5 * H * (1 + normalizedPos.y);
        return screenPos;
    }
    glm::vec4 normalizedToWorld(const glm::vec4& normalizedPos) const {
        glm::mat4 invProjection = glm::inverse(getNormalizedProjectionMatrix());
        glm::mat4 invView = glm::inverse(getViewMatrix());
        glm::vec4 viewPos = invProjection * normalizedPos;
        glm::vec4 worldPos = invView * viewPos;
        
        return worldPos;
    }
    glm::vec2 screenToProj(const glm::vec2& screenPos) const {
        glm::vec2 projPos;
        projPos.x = 0.5 * (L + R + (R - L)*(2*screenPos.x/W - 1));
        projPos.y = 0.5 * (T + B + (T - B)*(1 - 2*screenPos.y/H));
        return projPos;
    }
    glm::vec2 projToScreen(const glm::vec2& projPos) const {
        float r_l = 1.f/(R - L);
        float t_b = 1.f/(T - B);
        glm::vec2 screenPos;
        screenPos.x = 0.5 * W * (1 + (2 * projPos.x - (L + R)) * r_l);
        screenPos.y = 0.5 * H * (1 + ((T + B) - 2 * projPos.y) * t_b);
        return screenPos;
    }
    glm::vec2 worldToScreen(const glm::vec3& worldPos) const {
        glm::vec4 viewPos = worldToView(worldPos);
        glm::vec4 normalizedPos = viewToNormalized(viewPos);
        glm::vec2 screenPos =  normalizedToScreen(normalizedPos);
        return screenPos;
    }
    
    TriangleFace projectTriangle(const TriangleFace& worldTri) const {
        return {
            worldToScreenWithDepth(worldTri[0]),
            worldToScreenWithDepth(worldTri[1]),
            worldToScreenWithDepth(worldTri[2])
        };
    }

    glm::vec3 worldToScreenWithDepth(const glm::vec3& worldPos) const {
        glm::vec4 viewPos = worldToView(worldPos);
        glm::vec4 clip = getNormalizedProjectionMatrix() * viewPos;

        if (std::abs(clip.w) < 1e-6f)
            return glm::vec3(0);

        glm::vec3 ndc = glm::vec3(clip) / clip.w;

        glm::vec2 screen = normalizedToScreen(glm::vec4(ndc, 1.0f));
        return glm::vec3(screen, viewPos.z); // z — depth
    }
};