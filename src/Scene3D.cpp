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

int Scene3D::getiNextModel()
{
    if (models.empty()) return 0;
    return (iCurrModel + 1) % models.size();
}

void Scene3D::loadModel(std::string modelPath)
{
    ModelLoader loader;
    if (!loader.isLoad(modelPath))
        std::cout << "Failed to load model: " << modelPath << std::endl;
    
    Vertices modelAxis;
    for (const auto& vertex : loader.vcCoordinates)
        modelAxis.addVertex(vertex.x, vertex.y, vertex.z);

    Vertices modelVertices;
    for (const auto& vertex : loader.vCoordinates)
        modelVertices.addVertex(vertex.x, vertex.y, vertex.z);

    Faces modelFaces;
    for (const auto& faceIndices : loader.fIndicesTrn)
        modelFaces.addFace(faceIndices[0], faceIndices[3], faceIndices[6]);

    Edges modelEdges = modelFaces.getEdgesFromFaces();
    Model3D* newModel = new Model3D(modelVertices, modelFaces, modelEdges, modelAxis);
    addModel(*newModel);
}

void Scene3D::addModel(Model3D &model)
{
    float coefScale = getRandomScaleFactor();
    glm::vec3 offset = getRandomOffset();
    glm::vec3 offsetWorld = camera.normalizedToWorld(glm::vec4(offset.x,offset.y,offset.z,1));
    float angle = getRandomAngle();
    model.scale(coefScale, coefScale, coefScale);
    model.translate(offsetWorld.x, offsetWorld.y, offsetWorld.z);
    model.rotate(angle, Axis::X);

    models.push_back(&model);
}

void Scene3D::toNextModel()
{
    iCurrModel = getiNextModel();
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
    
    switch(currDrawingMode){
        case ONE_MODEL: {
            Model3D* currModel = models.at(iCurrModel);
            currModel->applyTransformation();
            renderer.addModelTriangles(*currModel, camera, glm::vec3(0.8f, 0.3f, 0.2f)); 
            renderer.addModelEdges(*currModel, camera);
            break;
        }
        case ALL_MODELS:{
            int idColor = 0;
            for(auto m: models){
                m->applyTransformation();
                renderer.addModelTriangles(*m, camera, MODEL_COLORS[idColor]);
                renderer.addModelEdges(*m, camera);
                idColor = (idColor + 1) % MODEL_COLORS.size();
            }
            break;
        }
        default: break;
    }

    renderer.drawElements();
    renderer.drawAxes(camera, 5.0f);
    renderer.renderToScreen();
}

void Scene3D::changeDrawingMode()
{
    switch (currDrawingMode) {
        case ONE_MODEL: currDrawingMode =  ALL_MODELS; break;
        case ALL_MODELS: currDrawingMode = ONE_MODEL; break;
    }
}

void Scene3D::handleZoom(float factor, const glm::vec2& screenPos) {
    glm::vec2 projPos = camera.screenToProj(screenPos);
    camera.zoom(factor, projPos);
}