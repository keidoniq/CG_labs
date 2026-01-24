#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include "AffineTransform3D.h"
#include "Quaternion.h"

using TriangleFace = std::vector<glm::vec3>;
const static float DEFAULT_DIST = 5.f;

class Camera3D {
private:
    float m_aspectRatio;
    glm::vec3 O_vector;
    Quaternion orientation;
    float F, D;
    int W, H;
    float L, R, B, T;

    glm::vec3 default_O_vector;
    Quaternion default_orientation;
    float default_L, default_R, default_B, default_T, default_F, default_D;
    int default_W, default_H;

    void maintainAspectRatio();
public:
    Camera3D(int W = 800, int H = 600,
        glm::vec3 O_vector = glm::vec3(1.5f, 1.f, 1.f),
        Quaternion orientation = Quaternion::fromEulerAngles(0.f, 0.f, 0.f),
        float distance = 10.f, float f = 7.f,
        float L = -DEFAULT_DIST, float R = DEFAULT_DIST, 
        float B = -DEFAULT_DIST, float T = DEFAULT_DIST):
        L(L), R(R), B(B), T(T), W(W), H(H), D(distance), F(f),
        O_vector(O_vector), orientation(orientation) {
            setViewport(W, H);
            default_O_vector = O_vector;
            default_orientation = orientation;
            default_L = L; default_R = R; default_B = B; default_T = T; 
            default_F = F; default_D = D;
            default_W = W; default_H = H;

        };
    Camera3D(int W = 800, int H = 600,
        glm::vec3 O_vector = glm::vec3(1.5f, 1.f, 1.f),
        glm::vec3 T_vector = glm::vec3(0.f, 1.f, 0.f),
        glm::vec3 N_vector = glm::vec3(0.f, 0.f, 3.f),
        float distance = 10.f, float f = 7.f,
        float L = -DEFAULT_DIST, float R = DEFAULT_DIST, 
        float B = -DEFAULT_DIST, float T = DEFAULT_DIST):
        L(L), R(R), B(B), T(T), W(W), H(H), D(distance), F(f),
        O_vector(O_vector) {
            // Вычисляем ориентацию из векторов look и up
            glm::vec3 forward = glm::normalize(N_vector - O_vector);
            glm::vec3 right = glm::normalize(glm::cross(forward, T_vector));
            glm::vec3 up = glm::normalize(glm::cross(right, forward));
            
            // Создаем матрицу вида и извлекаем кватернион
            glm::mat4 viewMatrix = glm::lookAt(O_vector, N_vector, up);
            orientation = Quaternion::fromRotationMatrix(viewMatrix);
            
            setViewport(W, H);
            default_O_vector = O_vector;
            default_orientation = orientation;
            default_L = L; default_R = R; default_B = B; default_T = T; 
            default_F = F; default_D = D;
            default_W = W; default_H = H;
        };
    
    void zoom(float factor, const glm::vec2& screenPoint);
    void zoomByFocusDistance(float factor){ F *= factor; }

    float getScreenLeft() const { return L; }
    float getScreenRight() const { return R; }
    float getScreenBottom() const { return B; }
    float getScreenTop() const { return T; }
    float getFocusDistance() const { return F; }
    float getDistancce() const { return D; }
    glm::vec3 getO() const { return O_vector; }
    Quaternion getOrientation() const { return orientation; }
    glm::vec4 getViewport() const { return glm::vec4(L, R, B, T); }
    
    void resetCamera();
    void setFocusDistance(float FocusDistance){ F = FocusDistance; }
    void setViewport(int width, int height);
    void setO(const glm::vec3& new_O_vector) { O_vector = new_O_vector; }
    void setOrientation(const Quaternion& new_Orientation) { orientation = new_Orientation; }

    void moveForward(float distance);
    void moveBackward(float distance);
    void moveLeft(float distance);
    void moveRight(float distance);
    void moveUp(float distance);
    void moveDown(float distance);

    void rotate(float angle, const glm::vec3& axis);
    void yaw(float angle) { rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f)); }
    void pitch(float angle) { rotate(angle, glm::vec3(1.0f, 0.0f, 0.0f)); }
    void roll(float angle) { rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f)); }
    
    void rotateLocalX(float angle);
    void rotateLocalY(float angle);
    void rotateLocalZ(float angle);

    glm::vec3 getForward() const;
    glm::vec3 getUp() const;
    glm::vec3 getRight() const;
    void lookAt(const glm::vec3& target);
    glm::mat4 getViewMatrix () const;
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
    
    glm::vec2 normalizedToScreen(const glm::vec4& normalizedPos) const;
    glm::vec4 normalizedToWorld(const glm::vec4& normalizedPos) const;
    glm::vec2 screenToProj(const glm::vec2& screenPos) const;
    glm::vec2 projToScreen(const glm::vec2& projPos) const;
    glm::vec2 worldToScreen(const glm::vec3& worldPos) const;
    glm::vec4 worldToView(const glm::vec3& worldPos) const { return getViewMatrix() * glm::vec4(worldPos, 1.0f); }
    glm::vec4 viewToNormalized(const glm::vec4& viewPos) const { return getNormalizedProjectionMatrix() * viewPos; }
    TriangleFace projectTriangle(const TriangleFace& worldTri) const;
    glm::vec3 worldToScreenWithDepth(const glm::vec3& worldPos) const;
};