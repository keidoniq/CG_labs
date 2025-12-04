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

void Scene3D::handleZoom(float factor, const glm::vec2& screenPos) {
    glm::vec2 projPos = camera.screenToProj(screenPos);
    camera.zoom(factor, projPos);
}