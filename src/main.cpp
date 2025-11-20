#include <iostream>
#include <string>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "shaders/ShaderModule.h"
#include "Model3D.h"
#include "Scene3D.h"

std::string controlsInfo();
std::ostream& operator<<(std::ostream& os, const glm::mat4& mat); 
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const std::vector<std::vector<float>> COLOURS_TO_PICK  = {
    {0, 0, 0},
    {1, 1, 1},
    {0, 0, 1},
    {0, 1, 0},
    {1, 0, 0},
};

const std::map<std::string, float> TRANSFORM_COEF{
    {"TRANSLATE", 0.05f}, {"SCALE", 0.1f},
    {"ROTATE_ANGLE", 10}, {"SHEAR", 0.1f},
    {"SCALE_IN", 2.f}, {"SCALE_OUT", 0.5f}, 
};

const float POLYGON_RADIUS = 0.4f;
const unsigned int POLYGON_N_SIDES = 6;

const std::string VSHADER_PATH = "src/shaders/vshader.glsl";
const std::string FSHADER_PATH = "src/shaders/fshader.glsl";

Scene3D* scene = nullptr;
Model3D* testModel = nullptr;
Vertices* originalVertices = nullptr;

int main()
{
    std::cout << controlsInfo();

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab_01 - Model3D", NULL, NULL);
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // glad: loading OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    // build and compile shader program
    ShaderModule triShader(VSHADER_PATH, FSHADER_PATH);

    //scene create
    scene = new Scene3D();
    scene->getCamera().setViewport(SCR_WIDTH, SCR_HEIGHT);
    scene->getCamera().updAxes();

    Vertices cubeVertices;
    cubeVertices.addVertex(-0.5f, -0.5f, -0.5f); // 0
    cubeVertices.addVertex( 0.5f, -0.5f, -0.5f); // 1
    cubeVertices.addVertex( 0.5f,  0.5f, -0.5f); // 2
    cubeVertices.addVertex(-0.5f,  0.5f, -0.5f); // 3
    cubeVertices.addVertex(-0.5f, -0.5f,  0.5f); // 4
    cubeVertices.addVertex( 0.5f, -0.5f,  0.5f); // 5
    cubeVertices.addVertex( 0.5f,  0.5f,  0.5f); // 6
    cubeVertices.addVertex(-0.5f,  0.5f,  0.5f); // 7
    originalVertices = new Vertices(cubeVertices);

    Faces cubeFaces;
    cubeFaces.addFace(0, 1, 2); cubeFaces.addFace(0, 2, 3); // передняя
    cubeFaces.addFace(4, 5, 6); cubeFaces.addFace(4, 6, 7); // задняя
    cubeFaces.addFace(0, 1, 5); cubeFaces.addFace(0, 5, 4); // нижняя
    cubeFaces.addFace(2, 3, 7); cubeFaces.addFace(2, 7, 6); // верхняя
    cubeFaces.addFace(0, 3, 7); cubeFaces.addFace(0, 7, 4); // левая
    cubeFaces.addFace(1, 2, 6); cubeFaces.addFace(1, 6, 5); // правая

    Edges cubeEdges = cubeFaces.getEdgesFromFaces();
    std::vector<unsigned int> indices;
    for (const auto& edge : cubeEdges.getEdges()) {
        indices.push_back(edge.getFirst());
        indices.push_back(edge.getSecond());
    }
    //create model
    testModel = new Model3D(cubeVertices, cubeFaces, cubeEdges);
    scene->addModel(*testModel);

    //setup buffers
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //gl wireframe rendering
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        //testModel->applyTransformation();
        triShader.bind();

        //Upd for shader
        projection = scene->getCamera().getProjectionMatrix();
        view = scene->getCamera().getViewMatrix();

        triShader.setMat4("projection", projection);
        triShader.setMat4("view", view);
        triShader.setMat4("model", model);

        // Get the transformed vertices for rendering
        Vertices currentVertices = testModel->getVertices();
        VerticesMatrix transformedVertices = currentVertices.getVertices();

        //vertex data for OpenGL
        int i = 0;
        std::vector<float> vertices;
        for (const auto& vertex : transformedVertices) {
            vertices.push_back(vertex.x());
            vertices.push_back(vertex.y());
            vertices.push_back(vertex.z());
            
            //colors
            // int id_colour = i % COLOURS_TO_PICK.size();
            // vertices.insert(vertices.end(),COLOURS_TO_PICK[id_colour].begin(),COLOURS_TO_PICK[id_colour].end());
            // ++i;
            vertices.insert(vertices.end(), COLOURS_TO_PICK[0].begin(), COLOURS_TO_PICK[0].end());
        }

        // updating buffers
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // render
        glClearColor(0.91, 0.92, 0.95, 0.94f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene->render();
        // render model
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        triShader.release();

        // glfw: swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //clear
    delete scene;
    delete testModel;
    delete originalVertices;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    glm::vec2 screenPos(xpos, ypos);

    switch (action) {
        case GLFW_PRESS:
            switch (button) {
                case GLFW_MOUSE_BUTTON_LEFT:
                    scene->handleMouseClick(screenPos, DragMode::Scene); // LMB → move scene
                    break;
                case GLFW_MOUSE_BUTTON_RIGHT:
                    scene->handleMouseClick(screenPos, DragMode::Model); // RMB → move model
                    break;
            }
            break;
        case GLFW_RELEASE:
            scene->handleMouseRelease();
            break;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ||
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        glm::vec2 screenPos(xpos, ypos);
        scene->handleMouseDrag(screenPos);
    }
}

std::string controlsInfo() {
    std::stringstream ss;
    ss << std::string(17, '=') << "\n3D Model Controls\n" << std::string(25, '=') << "\n"
       << "TRANSLATION:\n"
       << "  W - Move Up (Y+)\n"
       << "  S - Move Down (Y-)\n" 
       << "  A - Move Left (X-)\n"
       << "  D - Move Right (X+)\n"
       << "  Z - Move Back (Z-)\n"
       << "  C - Move Forward (Z+)\n\n"
       
       << "ROTATION:\n"
       << "  Q - Rotate around Y axis\n"
       << "  E - Rotate around X axis\n"
       << "  R - Rotate around Z axis\n\n"
       
       << "SHEAR (Skew):\n"
       << "  X - Shear X axis\n"
       << "  Y - Shear Y axis\n"
       << "  V - Shear Z axis\n\n"
       
       << "SCALING:\n"
       << "  I - Scale Uniform IN (2x)\n"
       << "  J - Scale Uniform OUT (0.5x)\n"
       << "  K - Scale Z axis OUT (0.5x)\n"
       << "  L - Scale Z axis IN (2x)\n\n"

       << "REFLECTION (Mirror):\n"
       << "  1 - Reflect X axis\n"
       << "  2 - Reflect Y axis\n"
       << "  3 - Reflect Z axis\n"
       << "  4 - Reflect XY plane\n"
       << "  5 - Reflect XZ plane\n"
       << "  6 - Reflect YZ plane\n"
       << "  7 - Reflect all axes (XYZ)\n\n"
       
       << "SYSTEM:\n"
       << "  T - Reset all model transformations\n"
       << "  P - Reset camera to default position\n"
       << "  Mouse Wheel - Zoom in/out\n"
       << "  LMB + Drag - Move scene\n"
       << "  RMB + Drag - Move model\n"
       << "  ESC -> Exit.\n";
    return ss.str();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Vertices currentVertices = testModel->getVertices();
    // VerticesMatrix verticesMatrix = currentVertices.getVertices();
    // glm::vec2 customPoint1 = verticesMatrix.front().getCartesianCoordinates();
    // glm::vec2 customPoint2 = verticesMatrix[3].getCartesianCoordinates();

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            // Translate
            case GLFW_KEY_W:
                testModel->translate(0.0f, 0.5f, 0.0f);
                break;
            case GLFW_KEY_S:
                testModel->translate(0.0f, -0.5f, 0.0f);
                break;
            case GLFW_KEY_A:
                testModel->translate(-0.5f, 0.0f, 0.0f);
                break;
            case GLFW_KEY_D:
                testModel->translate(0.5f, 0.0f, 0.0f);
                break;
            case GLFW_KEY_Z:
                testModel->translate(0.0f, 0.0f, -0.5f);
                break;
            case GLFW_KEY_C:
                testModel->translate(0.0f, 0.0f, 0.5f);
                break;
                
            // Rotate
            case GLFW_KEY_Q:
                testModel->rotate(glm::radians(TRANSFORM_COEF.at("ROTATE_ANGLE")), Axis::Y);
                break;
            case GLFW_KEY_E:
                testModel->rotate(glm::radians(TRANSFORM_COEF.at("ROTATE_ANGLE")), Axis::X);
                break;
            case GLFW_KEY_R:
                testModel->rotate(glm::radians(TRANSFORM_COEF.at("ROTATE_ANGLE")), Axis::Z);
                break;
                
            // Shear
            case GLFW_KEY_X:
                testModel->shear(0.1f, 0.0f, 0.0f);
                break;
            case GLFW_KEY_Y:
                testModel->shear(0.0f, 0.1f, 0.f);
                break;
            case GLFW_KEY_V:
                testModel->shear(0.0f, 0.f, 0.1f);
                break;

            //Scale
            case GLFW_KEY_I:
                testModel->scale(2.f, 2.f, 2.f);
                break;
            case GLFW_KEY_J:
                testModel->scale(0.5f, 0.5f, 0.5f);
                break;
            case GLFW_KEY_K:
                testModel->scale(1.0f, 1.0f, 0.5f);
                break;
            case GLFW_KEY_L:
                testModel->scale(1.0f, 1.0f, 2.f); // Scale только по Z
                break;
                
            //Reflect
            case GLFW_KEY_1:
                testModel->reflect(true, false, false); // Reflect X
                break;
            case GLFW_KEY_2:
                testModel->reflect(false, true, false); // Reflect Y
                break;
            case GLFW_KEY_3:
                testModel->reflect(false, false, true); // Reflect Z
                break;
            case GLFW_KEY_4:
                testModel->reflect(true, true, false); // Reflect XY
                break;
            case GLFW_KEY_5:
                testModel->reflect(true, false, true); // Reflect XZ
                break;
            case GLFW_KEY_6:
                testModel->reflect(false, true, true); // Reflect YZ
                break;
            case GLFW_KEY_7:
                testModel->reflect(true, true, true); // Reflect XYZ
                break;
                
            case GLFW_KEY_T:
                testModel->resetTransformation();
                break;
            case GLFW_KEY_P:
                scene->getCamera().resetCamera();
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
        scene->getCamera().setViewport(width, height);
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