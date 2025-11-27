#include "Scene3D.h"

void Scene3D::addModel(Model3D& model) {
    models.push_back(&model);
}

void Scene3D::toNextModel()
{
    iCurrModel = (iCurrModel + 1) % models.size();
}

void Scene3D::updModels() const
{
    for(auto m: models){
        m->applyTransformation();
    }
}

void Scene3D::clearModels() {
    models.clear();
}

void Scene3D::render() const {
    camera.clear();
    camera.drawAxes();

    for(auto m: models){
        m->applyTransformation();
    }
}

void Scene3D::handleMouseClick(const glm::vec2& screenPos, DragMode newDragMode) {
    dragMode = newDragMode;
    glm::vec3 worldPos = camera.screenToWorld_GLM(screenPos);
    std::cout << "Mouse click - Screen: (" << screenPos.x << ", " << screenPos.y 
              << ") World: (" << worldPos.x << ", " << worldPos.y << ", " << worldPos.z << ")" << std::endl;
              
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

void Scene3D::handleMouseDrag(const glm::vec2& screenPos) {
    glm::vec3 worldPos = camera.screenToWorld_GLM(screenPos);

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

void Scene3D::handleMouseRelease() {
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

void Scene3D::handleZoom(float factor, const glm::vec2& screenPos) {
    glm::vec3 worldPos = camera.screenToWorld_GLM(screenPos);
    camera.zoom(factor, worldPos);
}