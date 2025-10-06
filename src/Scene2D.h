
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
    std::vector<Model2D*> models;
    DragMode dragMode;

public:
    Scene2D(): camera() { dragMode = DragMode::None; }
    
    void addModel(Model2D& model);
    void clearModels();
    Camera2D& getCamera() { return camera; }
    
    void render() const;
    void updModels() const;
    //DONE from if to switch
    void handleMouseClick(const glm::vec2& screenPos, DragMode newDragMode);
    void handleMouseDrag(const glm::vec2& screenPos);
    void handleMouseRelease();
    void handleZoom(float factor, const glm::vec2& screenPos);
};