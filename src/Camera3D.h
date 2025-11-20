#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>
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

    float aspectRatio;
    float L, R, B, T;
    int W, H;

    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float fov;
    float F, N;

    bool isDragging;
    glm::vec3 dragStartWorld;
    glm::vec3 dragStartPosition;
    glm::vec3 dragStartTarget;
    std::map<std::string, float> startBounds;

    void updStartBounds(float startL, float startR, float startT, float startB);
    void maintainAspectRatio();
    
    float readDepthBuffer(const glm::vec2& screenPos) const {
        float depth;
        glReadPixels(
            (GLint)screenPos.x, 
            (GLint)(H - screenPos.y),
            1, 1, 
            GL_DEPTH_COMPONENT, 
            GL_FLOAT, 
            &depth
        );
        return depth;
    }
public:
    Camera3D(float L = -DEFAULT_DIST, float R = DEFAULT_DIST, float B = -DEFAULT_DIST, float T = DEFAULT_DIST, 
        int W = 800, int H = 600, bool isDragging = false,
        glm::vec3 position = glm::vec3(0.f, 0.f, DEFAULT_DIST),
        glm::vec3 target = glm::vec3(0.f),
        glm::vec3 up = glm::vec3(0.f, 1.f, 0.f),
        float fov = 45.0f, float nearPlane = 0.1f, float farPlane = 100.0f):
        L(L), R(R), B(B), T(T), W(W), H(H), isDragging(isDragging),
        position(position), target(target), up(up), 
        fov(fov), N(nearPlane), F(farPlane) {
            startBounds.emplace("L", L);
            startBounds.emplace("R", R);
            startBounds.emplace("B", B);
            startBounds.emplace("T", T);
            glGenVertexArrays(1, &axisVAO);
            glGenBuffers(1, &axisVBO);
    };
    ~Camera3D();
    
    void zoom(float factor, const glm::vec3& worldPoint);
    void startDrag(const glm::vec3& worldPos);
    void drag(const glm::vec3& worldPos);
    void endDrag();

    float getLeft() const { return L; }
    float getRight() const { return R; }
    float getBottom() const { return B; }
    float getTop() const { return T; }
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getTarget() const { return target; }
    glm::vec3 getUp() const { return up; }
    float getFOV() const { return fov; }
    
    void clear() const;
    void resetCamera();

    void updAxes();
    void drawAxes() const;
    
    void setViewport(int width, int height);
    glm::vec4 getViewport() const { return glm::vec4(L, R, B, T); }
    glm::mat4 getViewMatrix() const {//из мировых в видовые
        return glm::lookAt(position, target, up);
    }
    glm::mat4 getProjectionMatrix() const {//перспективная проекции
        return glm::perspective(glm::radians(fov), aspectRatio, N, F);
    }

    glm::vec3 screenToWorld_GLM(const glm::vec2& screenPos) const {
        glm::mat4 view = getViewMatrix();
        glm::mat4 projection = getProjectionMatrix();
        glm::vec4 viewport(0.0f, 0.0f, W, H);
        float depth = readDepthBuffer(screenPos);
        
        return glm::unProject(
            glm::vec3(screenPos.x, H - screenPos.y, depth),
            view,
            projection,
            viewport
        );
    }
    glm::vec3 worldToScreen_GLM(const glm::vec3& worldPos) const {
        glm::mat4 view = getViewMatrix();
        glm::mat4 projection = getProjectionMatrix();
        glm::vec4 viewport(0.0f, 0.0f, W, H);
        
        glm::vec3 screenPos = glm::project(
            worldPos,
            view,
            projection,
            viewport
        );
        return glm::vec3(screenPos.x, H - screenPos.y, screenPos.z);
    }
};