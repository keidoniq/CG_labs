#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "shaders/ShaderModule.h"
#include "Model3D.h"
#include "Scene3D.h"
#include "ModelLoader.h"

std::string controlsInfo();
std::ostream& operator<<(std::ostream& os, const glm::mat4 &mat);
std::ostream& operator<<(std::ostream &os, const glm::vec4 &vec);
std::ostream& operator<<(std::ostream &os, const glm::vec3 &vec);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const std::string VSHADER_PATH = "src/shaders/vshader.glsl";
const std::string FSHADER_PATH = "src/shaders/fshader.glsl";
const std::vector<std::string> modelPaths = {
    "resourses/gear.obj",
    "resourses/star.obj", 
    "resourses/cube.obj",
    "resourses/torusknot.obj",
    "resourses/gem.obj",
    "resourses/icosphere.obj",
    "resourses/cylinder.obj",
    "resourses/cone.obj",
    "resourses/teapot.obj",
};

Scene3D* scene = nullptr;
Model3D* currModel = nullptr;
Vertices* originalVertices = nullptr;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Model3D", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    std::cout << controlsInfo();
    ShaderModule screenShader(VSHADER_PATH, FSHADER_PATH);

    scene = new Scene3D(SCR_WIDTH, SCR_HEIGHT, &screenShader);
    scene->getCamera().setViewport(SCR_WIDTH, SCR_HEIGHT);

    for (auto modelPath: modelPaths){
        ModelLoader loader;
        if (!loader.isLoad(modelPath)) {
            std::cout << "Failed to load model: " << modelPath << std::endl;
            return -1;
        }
        
        Vertices modelAxis;
        for (const auto& vertex : loader.vcCoordinates) {
            modelAxis.addVertex(vertex.x, vertex.y, vertex.z);
        }

        Vertices modelVertices;
        for (const auto& vertex : loader.vCoordinates) {
            modelVertices.addVertex(vertex.x, vertex.y, vertex.z);
        }

        Faces modelFaces;
        for (const auto& faceIndices : loader.fIndicesTrn) {
            modelFaces.addFace(faceIndices[0], faceIndices[3], faceIndices[6]);
        }

        Edges modelEdges = modelFaces.getEdgesFromFaces();

        Model3D* newModel = new Model3D(modelVertices, modelFaces, modelEdges, modelAxis);
        scene->addModel(*newModel);
    }

    currModel = scene->getCurrModel();

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);        
        scene->render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //clear
    delete scene;
    delete currModel;
    delete originalVertices;

    glfwTerminate();
    return 0;
}

std::string controlsInfo() {
    std::stringstream ss;
    ss << std::string(17, '=') << "\n3D Model Controls\n" << std::string(25, '=') << "\n"
       << "TRANSLATION:\n"
       << "  W - Move Up (Y+)\n"
       << "  S - Move Down (Y-)\n" 
       << "  A - Move Left (X-)\n"
       << "  D - Move Right (X+)\n"
       << "  Q - Move Back (Z-)\n"
       << "  E - Move Forward (Z+)\n\n"
       
       << "ROTATION:\n"
       << "  Y - Rotate around Y axis\n"
       << "  X - Rotate around X axis\n"
       << "  Z - Rotate around Z axis\n\n"
       
       << "SCALING:\n"
       << "  I - Scale IN (2x)\n"
       << "  J - Scale OUT (0.5x)\n\n"

       << "REFLECTION (Mirror):\n"
       << "  1 - Reflect X axis\n"
       << "  2 - Reflect Y axis\n"
       << "  3 - Reflect Z axis\n"
       << "  4 - Reflect XY plane\n"
       << "  5 - Reflect XZ plane\n"
       << "  6 - Reflect YZ plane\n\n"

       << "CAMERA MOVEMENT:\n"
       << "  O - Zoom In (focus distance *1.25)\n"
       << "  K - Zoom Out (focus distance *0.25)\n"
       << "  Arrow Up - Move camera forward\n"
       << "  Arrow Down - Move camera backward\n" 
       << "  Arrow Left - Move camera left\n"
       << "  Arrow Right - Move camera right\n\n"
       
       << "SYSTEM:\n"
       << "  N - Go to the next model\n"
       << "  T - Reset all model transformations\n"
       << "  P - Reset camera to default position\n"
       << "  M - Change drawing mode\n"
       << "  0 - Print camera debug info\n"
       << "  Mouse Wheel - Zoom in/out\n"
       << "  ESC -> Exit.\n";
    return ss.str();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    VerticesMatrix centerAxis = currModel->getAxis();
    glm::vec3 p0 = centerAxis[0].getCartesianCoordinates();
    glm::vec3 p1 = centerAxis[1].getCartesianCoordinates();
    glm::vec3 p2 = centerAxis[2].getCartesianCoordinates();
    glm::vec3 p3 = centerAxis[3].getCartesianCoordinates();

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        Camera3D& camera = scene->getCamera();
        float moveSpeed = 0.2f;
        float currF = scene->getCamera().getFocusDistance();
        switch (key) {
            // CAMERA MOVEMENT CONTROLS
            case GLFW_KEY_O:
                scene->getCamera().zoomByFocusDistance(1.25);
                std::cout << "\nO - currF: "<< currF << " new: " << scene->getCamera().getFocusDistance();
                break;
            case GLFW_KEY_K:
                scene->getCamera().zoomByFocusDistance(0.8); //done
                std::cout << "\nK - currF: "<< currF << " new: " << scene->getCamera().getFocusDistance();
                break;
            case GLFW_KEY_UP:
                camera.moveForward(moveSpeed);
                break;
            case GLFW_KEY_DOWN:
                camera.moveBackward(moveSpeed);
                break;
            case GLFW_KEY_LEFT:
                camera.moveLeft(moveSpeed);
                break;
            case GLFW_KEY_RIGHT:
                camera.moveRight(moveSpeed);
                break;
            // Translate
            case GLFW_KEY_W:
                currModel->translate(0.0f, 0.5f, 0.0f);
                break;
            case GLFW_KEY_S:
                currModel->translate(0.0f, -0.5f, 0.0f);
                break;
            case GLFW_KEY_A:
                currModel->translate(-0.5f, 0.0f, 0.0f);
                break;
            case GLFW_KEY_D:
                currModel->translate(0.5f, 0.0f, 0.0f);
                break;
            case GLFW_KEY_Q:
                currModel->translate(0.0f, 0.0f, -0.5f);
                break;
            case GLFW_KEY_E:
                currModel->translate(0.0f, 0.0f, 0.5f);
                break;
                
            // Rotate
            case GLFW_KEY_Y:
                currModel->rotate(glm::radians(15.0), Axis::Y);
                break;
            case GLFW_KEY_X:
                currModel->rotate(glm::radians(15.0), Axis::X);
                break;
            case GLFW_KEY_Z:
                currModel->rotate(glm::radians(15.0), Axis::Z);
                break;
            case GLFW_KEY_7:
                currModel->rotateWithAxis(p0, p1, glm::radians(5.0)); 
                break;
            case GLFW_KEY_8:
                currModel->rotateWithAxis(p0, p2, glm::radians(5.0)); 
                break;
            case GLFW_KEY_9:
                currModel->rotateWithAxis(p0, p3, glm::radians(5.0)); 
                break;

            //Scale
            case GLFW_KEY_I:
                currModel->scale(2.f, 2.f, 2.f);
                break;
            case GLFW_KEY_J:
                currModel->scale(0.5f, 0.5f, 0.5f);
                break;
                
            //Reflect
            case GLFW_KEY_1:
                currModel->reflect(true, false, false); // Reflect X
                break;
            case GLFW_KEY_2:
                currModel->reflect(false, true, false); // Reflect Y
                break;
            case GLFW_KEY_3:
                currModel->reflect(false, false, true); // Reflect Z
                break;
            case GLFW_KEY_4:
                currModel->reflect(true, true, false); // Reflect XY
                break;
            case GLFW_KEY_5:
                currModel->reflect(true, false, true); // Reflect XZ
                break;
            case GLFW_KEY_6:
                currModel->reflect(false, true, true); // Reflect YZ
                break;
                
            case GLFW_KEY_T:
                currModel->resetTransformation();
                break;
            case GLFW_KEY_P:
                scene->getCamera().resetCamera();
                break;
            case GLFW_KEY_N:
                std::cout << "Switched to model index: " << scene->getiCurrModel()+1
                    << " out of " << scene->getNModels() 
                    << " models. Model address: " << currModel << std::endl;
                scene->toNextModel();
                currModel = scene->getCurrModel();
                break;
            case GLFW_KEY_0:
                std::cout << "CAMERA INFO:\t" <<
                "F=" << scene->getCamera().getFocusDistance() <<
                " D=" << scene->getCamera().getDistancce() <<
                "\nO_vector: "<< scene->getCamera().getO() <<
                "N_vector: "<< scene->getCamera().getN() <<
                "T_vector: "<< scene->getCamera().getT() << '\n';
                break;
        }
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    glm::vec2 screenPos(xpos, ypos);

    float coef = yoffset > 0 ? 1.1:0.9;
    scene->handleZoom(coef, screenPos);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    if (scene) {
        scene->resize(width, height);
    }
}

std::ostream &operator<<(std::ostream &os, const glm::mat4 &mat)
{
    os << std::fixed;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            os << mat[i][j] << " ";
        }
        os << '\n';
    }
    return os;
}
std::ostream &operator<<(std::ostream &os, const glm::vec4 &vec)
{
    os << vec.x << ' ' << vec.y << ' ' << vec.z << ' ' << vec.w << '\n';
    return os;
}
std::ostream &operator<<(std::ostream &os, const glm::vec3 &vec)
{
    os << vec.x << ' ' << vec.y << ' ' << vec.z << '\n';
    return os;
}