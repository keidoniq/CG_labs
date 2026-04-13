#include "Scene2D.h"

void Scene2D::addModel(Model2D& model) {
    models.push_back(&model);
}

int Scene2D::getiNextModel() const
{
    if (models.empty()) return 0;
    return (iCurrModel + 1) % models.size();
}

void Scene2D::toNextModel()
{
    iCurrModel = getiNextModel();
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
    switch(dragMode){
        case DragMode::Model:
            if(!models.empty())
                models[iCurrModel]->startDrag(worldPos);
            break;
        case DragMode::Scene:
            camera.startDrag(worldPos);
            break;
        case DragMode::None:
        default:
            break;
    }
}

void Scene2D::handleMouseDrag(const glm::vec2& screenPos) {
    glm::vec2 worldPos = camera.screenToWorld(screenPos);

    switch(dragMode){
        case DragMode::Model:
            if(!models.empty())
                models[iCurrModel]->drag(worldPos);
            break;
        case DragMode::Scene:
            camera.drag(worldPos);
            break;
        case DragMode::None:
        default:
            break;
    }
}

void Scene2D::handleMouseRelease() {
    switch(dragMode){
        case DragMode::Model:
            models[iCurrModel]->endDrag();
            break;
        case DragMode::Scene:
            camera.endDrag();
            break;
        case DragMode::None:default:
        break;
    }
    dragMode = DragMode::None;
}

void Scene2D::handleZoom(float factor, const glm::vec2& screenPos) {
    glm::vec2 worldPos = camera.screenToWorld(screenPos);
    camera.zoom(factor, worldPos);
}