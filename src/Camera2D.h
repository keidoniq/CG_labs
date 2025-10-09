#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>
#include <map>
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

    bool isDragging;
    glm::vec2 dragStartWorld;
    //DONE glm::vec4 startBounds -> readable L,R,B,T
    std::map<std::string, float> startBounds;

    void updStartBounds(float startL, float startR, float startT, float startB);
    void maintainAspectRatio();
public:
    Camera2D(float L = -DEFAULT_DIST, float R = DEFAULT_DIST, 
        float B=-DEFAULT_DIST, float T = DEFAULT_DIST, 
        int W = 800, int H = 600, bool isDragging = false):
        L(L), R(R), B(B), T(T), W(W), H(H), isDragging(isDragging) {

            startBounds.emplace("L", L);
            startBounds.emplace("H", H);
            startBounds.emplace("B", B);
            startBounds.emplace("T", T);
            glGenVertexArrays(1, &axisVAO);
            glGenBuffers(1, &axisVBO);
    };
    ~Camera2D();
    
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
    glm::mat4 getProjectionMatrix(float near = -1.f, float far  =  1.f) const {
        /*
            [ scaleX      0          0      translateX ]
            [ 0         scaleY       0      translateY ]
            [ 0           0       scaleZ    translateZ ]
            [ 0           0          0           1     ]

            [ 2/(R-L)      0          0      -(R+L)/(R-L) ]
            [ 0         2/(T-B)       0      -(T+B)/(T-B) ]
            [ 0           0        -2/(F-N)   -(F+N)/(F-N)]
            [ 0           0           0          1        ]

            [ 2/(R-L)      0          0      -(R+L)/(R-L) ]
            [ 0         2/(T-B)       0      -(T+B)/(T-B) ]
            [ 0           0          -1          0        ]
            [ 0           0           0          1        ]
        */
        float fn = 1.f/(far - near);
        float rl = 1.f/(R - L);
        float tb = 1.f/(T - B);

        glm::mat4 proj(1.0f);
        proj[0][0] = 2.f * rl;
        proj[3][0] = -(R + L) * rl;
        proj[1][1] = 2.f * tb;
        proj[3][1] = -(T + B) * tb;
        proj[2][2] = -2.f * fn;
        proj[3][2] = -(far + near) * fn;

        return proj;
    }
};