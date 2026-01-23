
#pragma once
#include "Camera3D.h"
#include "Model3D.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include "ZBufferRender.h"

/*
Класс Scene3D содержит в себе камеру и
модель (или набор моделей) и
метод Render() отрисовки всех моделей.
*/ 

class Scene3D {
private:
    ZBufferRenderer renderer;
    Camera3D camera;
    std::vector<Model3D*> models;
    int iCurrModel = 0;

    float getRandomAngle();
    glm::vec3 getRandomOffset(float coeff = 1e-3, int pow = 3);
    float getRandomScaleFactor(float coeff = 1e-1);
public:
    Scene3D(int w, int h, ShaderModule* shader): camera(), renderer(w, h, shader) { srand(time(nullptr)); }
    
    void addModel(Model3D& model);
    void toNextModel();
    void clearModels();

    Model3D* getCurrModel() { return models[iCurrModel];}
    Camera3D& getCamera() { return camera; }
    int getNModels() { return models.size(); }
    int getiCurrModel() { return iCurrModel; }
    
    void render();
    void updModels() const;
    void resize(int width, int height);
    void handleZoom(float factor, const glm::vec2& screenPos);
};