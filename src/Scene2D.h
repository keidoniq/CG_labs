
// Класс Scene2D содержит в себе камеру (или набор камер)
//  и модель (или набор моделей) и метод 
//  Render() отрисовки всех моделей.
#pragma once
#include "Camera2D.h"
#include "Model2D.h"
#include <vector>

enum class DragMode {
    None, Scene, Model
};

class Scene2D {
private:
    Camera2D camera;
    DragMode dragMode;
    std::vector<Model2D*> models;
    int iCurrModel = 0;

public:
    Scene2D(): camera() { dragMode = DragMode::None; }
    ~Scene2D() { for (auto m : models) delete m; }
    
    void addModel(Model2D& model);
    void toNextModel();
    void clearModels();
    void updModels() const;
    Model2D* getCurrModel() { return models[iCurrModel];}
    Camera2D& getCamera() { return camera; }
    int getiNextModel() const;

    void render() const;
    
    void handleMouseClick(const glm::vec2& screenPos, DragMode newDragMode);
    void handleMouseDrag(const glm::vec2& screenPos);
    void handleMouseRelease();
    void handleZoom(float factor, const glm::vec2& screenPos);
};