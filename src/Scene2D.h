
// Класс Scene2D содержит в себе камеру (или набор камер)
//  и модель (или набор моделей) и метод 
//  Render() отрисовки всех моделей.
#pragma once
#include "Camera2D.h"
#include "Model2D.h"
#include <vector>

class Scene2D {
private:
    Camera2D camera;
    std::vector<Model2D*> models;

public:
    Scene2D(): camera() {}
    
    void addModel(Model2D& model);
    void updModels();
    void clearModels();
    Camera2D& getCamera() { return camera; }
    
    void render() const;
    void handleMouseClick(const glm::vec2& screenPos);
    void handleMouseDrag(const glm::vec2& screenPos);
    void handleMouseRelease();
    void handleZoom(float factor, const glm::vec2& screenPos);
};