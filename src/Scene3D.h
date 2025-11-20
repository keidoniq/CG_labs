
#pragma once
#include "Camera3D.h"
#include "Model3D.h"
#include <vector>

/*

Класс Scene3D содержит в себе камеру и
модель (или набор моделей) и
метод Render() отрисовки всех моделей.
*/ 

enum class DragMode {
    None, Scene, Model
};


class Scene3D {
private:
    Camera3D camera;
    std::vector<Model3D*> models;
    DragMode dragMode;

public:
    Scene3D(): camera() { dragMode = DragMode::None; }
    
    void addModel(Model3D& model);
    void clearModels();
    Camera3D& getCamera() { return camera; }
    
    void render() const;
    void updModels() const;
    
    void handleMouseClick(const glm::vec2& screenPos, DragMode newDragMode);
    void handleMouseDrag(const glm::vec2& screenPos);
    void handleMouseRelease();
    void handleZoom(float factor, const glm::vec2& screenPos);
};