
#pragma once
#include "Camera3D.h"
#include "Model3D.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <Windows.h>

/*
Класс Scene3D содержит в себе камеру и
модель (или набор моделей) и
метод Render() отрисовки всех моделей.
*/ 

class Scene3D {
private:
    Camera3D camera;
    std::vector<Model3D*> models;
    int iCurrModel = 0;

    //done normalized
    float getRandomAngle();
    glm::vec3 getRandomOffset(float coeff = 1e-3, int pow = 3);
    float getRandomScaleFactor(float coeff = 1e-1);
public:
    Scene3D(): camera() { srand(time(nullptr)); }
    
    void addModel(Model3D& model);
    void toNextModel();
    void clearModels();

    Model3D* getCurrModel() { return models[iCurrModel];}
    Camera3D& getCamera() { return camera; }
    int getNModels() { return models.size(); }
    int getiCurrModel() { return iCurrModel; }
    
    void render() const;
    void updModels() const;
    
    void handleZoom(float factor, const glm::vec2& screenPos);
};