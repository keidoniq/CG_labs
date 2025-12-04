#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
#include "AffineTransform3D.h"
// Примерная структура класса Camera3D. Поля класса Camera3D:
// • L, R, B, T (мировые координаты границ окна для первого способа) 
    //либо X0, Y0, px, py (для второго способа);
// • W, H – разрешение рабочей области окна;
// • posX, posY – позиция графического курсора в мировых координатах, управляемая методами MoveTo и LiveTo;
// • возможно добавление дополнительных параметров, например, ширина и высота одного пикселя, выраженная в мировых координатах;
// • при реализации некоторых методов могут понадобиться и другие параметры 
    //(например, при реализации перетаскивания графика могут понадобиться параметры, в которых хранится положение курсора в момент начала перетаскивания);
// Методы класса Camera3D:
// • очистка рабочей области окна;
// • определение разрешения рабочей области
//  (данный метод вызывается каждый раз при изменении размеров окна);
// • процедуры для отрисовки отрезков, заданных в мировых координатах 
//    (пользователь вашего класса при построении линий и графиков не должен ничего знать про экранные координаты);
// • процедуры для построения координатных осей;
// • при реализации могут понадобиться и другие методы
//  (например, процедуры, реализующие перетаскивание графика мышью и масштабирование графика).
const static float DEFAULT_DIST = 5.f;

class Camera3D {
private:
    GLuint axisVAO = 0, axisVBO = 0;

    float m_aspectRatio;
    glm::vec3 O_vector, N_vector, T_vector;
    float F, D;
    int W, H;

    float L, R, B, T;
    void maintainAspectRatio();
public:
    Camera3D(float L = -DEFAULT_DIST, float R = DEFAULT_DIST, float B = -DEFAULT_DIST, float T = DEFAULT_DIST, 
        int W = 800, int H = 600,
        glm::vec3 O_vector = glm::vec3(0.f, 0.f, 2.5f),
        glm::vec3 N_vector = glm::vec3(0.f, 0.f, 3.f),
        glm::vec3 T_vector = glm::vec3(0.f, 1.f, 0.f),
        float distance = 5.f, float f = 5.f):
        L(L), R(R), B(B), T(T), W(W), H(H), D(distance), F(f),
        O_vector(O_vector), N_vector(N_vector), T_vector(T_vector) {

            glGenVertexArrays(1, &axisVAO);
            glGenBuffers(1, &axisVBO);
    };
    ~Camera3D();
    
    void zoom(float factor, const glm::vec2& screenPoint);

    float getLeft() const { return L; }
    float getRight() const { return R; }
    float getBottom() const { return B; }
    float getTop() const { return T; }
    float getFocusDistance() const { return F; }
    float getDistancce() const { return D; }
    glm::vec4 getViewport() const { return glm::vec4(L, R, B, T); }
    
    void clear() const;
    void resetCamera();
    void setViewport(int width, int height);

    void updAxes();
    void drawAxes() const;

    
    void moveForward(float distance) {
        O_vector += N_vector * distance;
    }
    void moveBackward(float distance) {
        moveForward(-distance);
    }
    void moveLeft(float distance) {
        glm::vec3 right = glm::normalize(glm::cross(N_vector, T_vector));
        O_vector += right * distance;
    }
    void moveRight(float distance) {
        moveLeft(-distance);
    }
    void moveUp(float distance) {
        O_vector += T_vector * distance;
    }
    void moveDown(float distance) {
        moveUp(-distance);
    }
    
    glm::mat4 getViewMatrix () const {//из мировых в видовые
        glm::vec3 k = -glm::normalize(N_vector);
        glm::vec3 i = glm::normalize(glm::cross(T_vector, N_vector));
        glm::vec3 j = glm::normalize(glm::cross(k, i));
        
        return glm::mat4(
            glm::vec4(i.x, j.x, k.x, 0.f),
            glm::vec4(i.y, j.y, k.y, 0.f), 
            glm::vec4(i.z, j.z, k.z, 0.f),
            glm::vec4(glm::dot(i, O_vector),  
                      glm::dot(j, O_vector),  
                      glm::dot(k, O_vector),  
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
    glm::vec2 normalizedToScreen(const glm::vec4& normalizedPos) {
        glm::vec2 screenPos;
        screenPos.x = 0.5 * W * (1 + normalizedPos.x);
        screenPos.y = 0.5 * H * (1 - normalizedPos.y);
        return screenPos;
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
    glm::vec2 worldToScreen(const glm::vec3& worldPos) {
        glm::vec4 viewPos = worldToView(worldPos);
        glm::vec4 normalizedPos = viewToNormalized(viewPos);
        glm::vec2 screenPos =  normalizedToScreen(normalizedPos);
        return screenPos;
    }
    void rotateAroundFocus(float horizontalAngle, float verticalAngle){
    }
    void pitchRight(float phi)
    {//ось вращения - B
        glm::vec4 dir = AffineTransform3D::rotation(phi, Axis::X) * glm::vec4(N_vector, 0.0f);
        N_vector = glm::normalize(glm::vec3(dir));
        glm::vec3 right = glm::normalize(glm::cross(N_vector, T_vector));
        T_vector = glm::normalize(glm::cross(right, N_vector));
    }
    void yawUp(float phi)
    {//ось вращения - T
        glm::vec4 dir = AffineTransform3D::rotation(phi, Axis::Y) * glm::vec4(N_vector, 0.0f);
        N_vector = glm::normalize(glm::vec3(dir));

        glm::vec3 right = glm::normalize(glm::cross(N_vector, T_vector));
        T_vector = glm::normalize(glm::cross(right, N_vector));
    }
    void roll(float phi)
    {//ось вращения - N
        glm::vec4 up = AffineTransform3D::rotation(phi, Axis::Z) * glm::vec4(T_vector, 0.0f);
        T_vector = glm::normalize(glm::vec3(up));
    }
    void rotateAroundFocusWithMouse(float deltaX, float deltaY, float sensitivity = 0.01f) {
        rotateAroundFocus(deltaX * sensitivity, deltaY * sensitivity);
    }
    void zoomByDistance(float factor){
        D = D * factor;
    }
    void setFocusDistance(float FocusDistance){
        F = FocusDistance;
    }
};