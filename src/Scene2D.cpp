#include "Scene2D.h"

void Scene2D::addModel(Model2D& model) {
    models.push_back(&model);
}

void Scene2D::updModels()
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
    
}

void Scene2D::handleMouseClick(const glm::vec2& screenPos) {
    glm::vec2 worldPos = camera.screenToWorld(screenPos);
    camera.startDrag(worldPos);
}

void Scene2D::handleMouseDrag(const glm::vec2& screenPos) {
    glm::vec2 worldPos = camera.screenToWorld(screenPos);
    camera.drag(worldPos);
}

void Scene2D::handleMouseRelease() {
    camera.endDrag();
}

void Scene2D::handleZoom(float factor, const glm::vec2& screenPos) {
    glm::vec2 worldPos = camera.screenToWorld(screenPos);
    camera.zoom(factor, worldPos);
}