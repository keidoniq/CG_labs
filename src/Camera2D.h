#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>
// Примерная структура класса Camera2D. Поля класса Camera2D:
// • L, R, B, T (мировые координаты границ окна для первого способа) 
    //либо X0, Y0, px, py (для второго способа);
// • W, H – разрешение рабочей области окна;
// • posX, posY – позиция графического курсора в мировых координатах, управляемая методами MoveTo и LiveTo;
// • возможно добавление дополнительных параметров, например, ширина и высота одного пикселя, выраженная в мировых координатах;
// • при реализации некоторых методов могут понадобиться и другие параметры 
    //(например, при реализации перетаскивания графика могут понадобиться параметры, в которых хранится положение курсора в момент начала перетаскивания);
// • по желанию можно добавить поля PenColor, BrushColor, AxesColor и т.д.
// Методы класса Camera2D:
// • очистка рабочей области окна;
// • определение разрешения рабочей области
//  (данный метод вызывается каждый раз при изменении размеров окна);
// • процедуры для отрисовки отрезков, заданных в мировых координатах 
//    (пользователь вашего класса при построении линий и графиков не должен ничего знать про экранные координаты);
// • процедуры для построения координатных осей;
// • при реализации могут понадобиться и другие методы
//  (например, процедуры, реализующие перетаскивание графика мышью и масштабирование графика).
const static float DEFAULT_DIST = 5.0f;

class Camera2D {
private:
    GLuint axisVAO = 0, axisVBO = 0;

    float L, R, B, T;
    int W, H;

    int posX, posY;
    
    bool isDragging;
    glm::vec2 dragStartWorld;
    glm::vec4 startBounds;
    
    void maintainAspectRatio();

public:
    Camera2D(float L = -DEFAULT_DIST, float R = DEFAULT_DIST, 
        float B=-DEFAULT_DIST, float T = DEFAULT_DIST, 
        int W = 800, int H = 600, bool isDragging = false):
        L(L), R(R), B(B), T(T), W(W), H(H), isDragging(isDragging) {
        glGenVertexArrays(1, &axisVAO);
        glGenBuffers(1, &axisVBO);
    };
    
    void setViewport(int width, int height);
    
    glm::vec2 screenToWorld(const glm::vec2& screenPos) const;
    glm::vec2 worldToScreen(const glm::vec2& worldPos) const;
    
    void zoom(float factor, const glm::vec2& worldPoint);
    void startDrag(const glm::vec2& worldPos);
    void drag(const glm::vec2& worldPos);
    void endDrag();
    
    void clear() const;
    void updAxes();
    void drawAxes() const;
    
    float getLeft() const { return L; }
    float getRight() const { return R; }
    float getBottom() const { return B; }
    float getTop() const { return T; }
    
    glm::vec4 getViewport() const { return glm::vec4(L, R, B, T); }

};