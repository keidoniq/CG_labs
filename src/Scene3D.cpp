#include "Scene3D.h"

float Scene3D::getRandomAngle(){
    return glm::radians(rand() % 90 - 45.f);
}

glm::vec3 Scene3D::getRandomOffset(float coeff, int pow)
{
    int limit = int(glm::pow(10, pow));
    return glm::vec3(
        coeff * (rand() % limit - 0.5*limit),
        coeff * (rand() % limit - 0.5*limit),
        coeff * (rand() % limit - 0.5*limit)
    );
}

float Scene3D::getRandomScaleFactor(float coeff)
{
    return coeff * (rand() % 10 + 5);
}

void Scene3D::addModel(Model3D &model)
{
    // float coefScale = getRandomScaleFactor();
    // glm::vec3 offset = getRandomOffset();
    // glm::vec3 offsetWorld = camera.normalizedToWorld(glm::vec4(offset.x,offset.y,offset.z,1));
    float angle = getRandomAngle();
    // model.scale(coefScale, coefScale, coefScale);
    // model.translate(offsetWorld.x, offsetWorld.y, offsetWorld.z);
    model.rotate(angle, Axis::X);

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

void Scene3D::resize(int width, int height){
    camera.setViewport(width, height);
    renderer.resize(width, height); 
}

void Scene3D::clearModels() {
    models.clear();
}

void Scene3D::render() {
    renderer.clear();
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.89, 0.93, 0.98, 1.f);
    glClear(GL_COLOR_BUFFER_BIT); 

    if (!models.empty()) {
        Model3D* currModel = models[iCurrModel];
        currModel->applyTransformation();
        renderer.drawModel(*currModel, camera);
    }
    renderer.drawAxes(camera, 5.0f);
    renderer.renderToScreen();
}

void Scene3D::handleZoom(float factor, const glm::vec2& screenPos) {
    glm::vec2 projPos = camera.screenToProj(screenPos);
    camera.zoom(factor, projPos);
}