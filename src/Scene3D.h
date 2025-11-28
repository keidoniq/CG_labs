
#pragma once
#include "Camera3D.h"
#include "Model3D.h"
#include <vector>

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

public:
    Scene3D(): camera() {}
    
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
    void handleMouseClick(const glm::vec2& screenPos);
    void handleMouseRelease();
};