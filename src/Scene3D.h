
#pragma once
#include "Camera3D.h"
#include "Model3D.h"
#include "ModelLoader.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include "ZBufferRender.h"

/*
Класс Scene3D содержит в себе камеру и
модель (или набор моделей) и
метод Render() отрисовки всех моделей.
*/ 
enum Drawing_Mode {
    ALL_MODELS, ONE_MODEL
};

class Scene3D {
private:
    ZBufferRenderer renderer;
    std::vector<Camera3D*> cameras;
    std::vector<Model3D*> models;
    int iCurrModel = 0;
    int iCurCamera = 0;
    Drawing_Mode currDrawingMode = Drawing_Mode::ONE_MODEL;

    float getRandomAngle(int limitUp = 90, float range = 45.f);
    glm::vec3 getRandomOffset(float coeff = 1e-3, int pow = 3);
    float getRandomScaleFactor(float coeff = 1e-1);
    int getiNextModel();
    int getiNextCamera();
public:
    Scene3D(int w, int h, ShaderModule* shader): renderer(w, h, shader) { 
        srand(time(nullptr)); 
        Camera3D* defaultCamera = new Camera3D(w, h);
        addCamera(*defaultCamera);
    }
    
    void loadModel(std::string modelPath);
    void addModel(Model3D& model, bool isRandom = false);
    void toNextModel();
    void clearModels();
    void updModels() const;
    
    void addCamera(int width, int height,
        const glm::vec3& O_vector = glm::vec3(1.5f, 1.f, 1.f),
        const glm::vec3& T_vector = glm::vec3(0.f, 1.f, 0.f),
        const glm::vec3& N_vector = glm::vec3(0.f, 0.f, 3.f),
        float F = 7.f, float D = 10.f, 
        float L = -5.f, float R = 5.f, float B = -5.f, float T = 5.f);
    void addCamera(Camera3D& newCamera);
    void toNextCamera();
    void clearCameras();

    Model3D* getCurrModel() { return models[iCurrModel];}
    int getNModels() { return models.size(); }
    int getiCurrModel() { return iCurrModel; }
    Camera3D* getCurrCamera() { return cameras[iCurCamera]; }
    int getNCameras() { return cameras.size(); }
    int getiCurrCamera() { return iCurCamera; }
    
    void render();
    void changeDrawingMode();
    void resize(int width, int height);
    void handleZoom(float factor, const glm::vec2& screenPos);

private:    
    const std::vector<glm::vec3> MODEL_COLORS = {
        glm::vec3(0.8f, 0.2f, 0.2f),   // Красный
        glm::vec3(0.2f, 0.8f, 0.2f),   // Зеленый
        glm::vec3(0.2f, 0.2f, 0.8f),   // Синий
        
        glm::vec3(0.8f, 0.8f, 0.2f),   // Желтый
        glm::vec3(0.8f, 0.2f, 0.8f),   // Пурпурный
        glm::vec3(0.2f, 0.8f, 0.8f),   // Бирюзовый
        
        glm::vec3(1.0f, 0.5f, 0.0f),   // Оранжевый
        glm::vec3(0.5f, 0.0f, 1.0f),   // Фиолетовый
        glm::vec3(0.0f, 0.5f, 0.5f),   // Темно-бирюзовый

        glm::vec3(1.0f, 0.7f, 0.7f),   // Светло-красный
        glm::vec3(0.7f, 1.0f, 0.7f),   // Светло-зеленый
        glm::vec3(0.7f, 0.7f, 1.0f),   // Светло-синий
        
        glm::vec3(1.0f, 1.0f, 0.7f),   // Светло-желтый
        glm::vec3(1.0f, 0.7f, 1.0f),   // Светло-пурпурный
        glm::vec3(0.7f, 1.0f, 1.0f),   // Светло-бирюзовый
        
        glm::vec3(1.0f, 0.8f, 0.6f),   // Персиковый
        glm::vec3(0.8f, 0.7f, 1.0f),   // Лавандовый
        glm::vec3(0.7f, 0.9f, 0.8f),   // Мятный

        glm::vec3(1.0f, 0.0f, 0.0f),   
        glm::vec3(0.0f, 1.0f, 0.0f),   
        glm::vec3(0.0f, 0.0f, 1.0f),   
        
        glm::vec3(1.0f, 1.0f, 0.0f),   // Ярко-желтый
        glm::vec3(1.0f, 0.0f, 1.0f),   // Ярко-пурпурный
        glm::vec3(0.0f, 1.0f, 1.0f),   // Ярко-бирюзовый
        
        glm::vec3(1.0f, 0.5f, 0.0f),   // Оранжевый
        glm::vec3(0.6f, 0.2f, 1.0f),   // Фиолетовый
        glm::vec3(0.2f, 0.8f, 0.4f)    // Салатовый
    };    
    const float MODEL_SPACING = 1.f;
    glm::vec3 getGridPosition(int index);
};