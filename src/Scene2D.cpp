#include "Scene2D.h"

void Scene2D::addModel(Model2D& model) {
    models.push_back(&model);
}

void Scene2D::updModels() const
{
    for(auto m: models){
        m->applyTransformation();
    }
}

void Scene2D::clearModels() {
    models.clear();
}

void Scene2D::render() const {
    camera.clear();
    camera.drawAxes();

    for(auto m: models){
        m->applyTransformation();
    }
}

void Scene2D::handleMouseClick(const glm::vec2& screenPos, DragMode newDragMode) {
    dragMode = newDragMode;
    glm::vec2 worldPos = camera.screenToWorld(screenPos);
    if(dragMode == DragMode::Model and !models.empty()){
        models[0]->startDrag(worldPos);
    }
    else if (dragMode == DragMode::Scene){
        camera.startDrag(worldPos);
    }
}

void Scene2D::handleMouseDrag(const glm::vec2& screenPos) {
    glm::vec2 worldPos = camera.screenToWorld(screenPos);

    if(dragMode == DragMode::Model and !models.empty()){
        models[0]->drag(worldPos);
    }
    else if (dragMode == DragMode::Scene){
        camera.drag(worldPos);
    }
}

void Scene2D::handleMouseRelease() {
    if(dragMode == DragMode::Model){
        models[0]->endDrag();
    }
    else if (dragMode == DragMode::Scene){
        camera.endDrag();
    }
    dragMode = DragMode::None;
}

void Scene2D::handleZoom(float factor, const glm::vec2& screenPos) {
    glm::vec2 worldPos = camera.screenToWorld(screenPos);
    camera.zoom(factor, worldPos);
}