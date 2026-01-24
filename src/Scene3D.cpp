#include "Scene3D.h"

float Scene3D::getRandomAngle(int limitUp, float range){
    return glm::radians(rand() % limitUp - range);
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

int Scene3D::getiNextCamera()
{
    if (cameras.empty()) return 0;
    return (iCurCamera + 1) % cameras.size();
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

void Scene3D::addModel(Model3D &model, bool isRandom)
{
    float coefScale = 1.f;
    glm::vec3 offset;
    float angle;

    if (isRandom){
        coefScale = getRandomScaleFactor();
        offset = getRandomOffset();
        offset = cameras[iCurCamera]->normalizedToWorld(glm::vec4(offset.x,offset.y,offset.z,1));
        angle = getRandomAngle();
    } else {
        coefScale = 0.4f;
        offset = getGridPosition(models.size());
        angle = getRandomAngle(20, 10.f);
    }
    
    model.scale(coefScale, coefScale, coefScale);
    model.translate(offset.x, offset.y, offset.z);
    model.rotate(angle, Axis::X);

    models.push_back(&model);
}

void Scene3D::toNextModel()
{
    iCurrModel = getiNextModel();
}

void Scene3D::addCamera(int width, int height, const glm::vec3 &O_vector, const glm::vec3 &T_vector, const glm::vec3 &N_vector, float F, float D, float L, float R, float B, float T)
{
    Camera3D* newCamera = new Camera3D(
        width, height, O_vector, T_vector, N_vector, 
        D, F, L, R, B, T);
    addCamera(*newCamera);
}

void Scene3D::addCamera(int width, int height, const glm::vec3 &O_vector, Quaternion orientation, float F, float D, float L, float R, float B, float T)
{
    Camera3D* newCamera = new Camera3D(
        width, height, O_vector, orientation, 
        D, F, L, R, B, T);
    addCamera(*newCamera);
}

void Scene3D::addCamera(Camera3D &newCamera)
{
    cameras.push_back(&newCamera);
}

void Scene3D::toNextCamera()
{
    iCurCamera = getiNextCamera();
}

void Scene3D::updModels() const
{
    for(auto m: models){
        m->applyTransformation();
    }
}

void Scene3D::resize(int width, int height){
    for(const auto& c: cameras){
        c->setViewport(width, height);
    }
    renderer.resize(width, height); 
}

void Scene3D::clearModels() {
    models.clear();
}

void Scene3D::clearCameras() {
    cameras.clear();
}

void Scene3D::render() {
    renderer.clear();
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.89, 0.93, 0.98, 1.f);
    glClear(GL_COLOR_BUFFER_BIT); 

    Camera3D* currCamera = cameras.at(iCurCamera);
    
    switch(currDrawingMode){
        case ONE_MODEL: {
            Model3D* currModel = models.at(iCurrModel);
            currModel->applyTransformation();
            renderer.addModelTriangles(*currModel, *currCamera, glm::vec3(0.8f, 0.3f, 0.2f)); 
            renderer.addModelEdges(*currModel, *currCamera);
            break;
        }
        case ALL_MODELS:{
            int idColor = 0;
            for(auto m: models){
                m->applyTransformation();
                renderer.addModelTriangles(*m, *currCamera, MODEL_COLORS[idColor]);
                renderer.addModelEdges(*m, *currCamera);
                idColor = (idColor + 1) % MODEL_COLORS.size();
            }
            break;
        }
        default: break;
    }

    renderer.drawElements();
    renderer.drawAxes(*currCamera, 5.0f);
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
    glm::vec2 projPos = cameras[iCurCamera]->screenToProj(screenPos);
    cameras[iCurCamera]->zoom(factor, projPos);
}

glm::vec3 Scene3D::getGridPosition(int index){
    int nSpaces = index / 6 + 1;
    int gridPosition = index % 6;
    switch (gridPosition){
        case 0: return glm::vec3(nSpaces*MODEL_SPACING, 0.0f, 0.0f);
        case 1: return glm::vec3(-nSpaces*MODEL_SPACING, 0.0f, 0.0f);   
        case 2: return glm::vec3(0.0f, nSpaces*MODEL_SPACING, 0.0f);
        case 3: return glm::vec3(0.0f, -nSpaces*MODEL_SPACING, 0.0f);
        case 4: return glm::vec3(0.0f, 0.0f, nSpaces*MODEL_SPACING);
        case 5: return glm::vec3(0.0f, 0.0f, -nSpaces*MODEL_SPACING);
    }
    return glm::vec3(0.f);
}
