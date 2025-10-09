#include <iostream>
#include <string>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "shaders/ShaderModule.h"
#include "Model2D.h"
#include "Scene2D.h"

std::string controlsInfo();
std::ostream& operator<<(std::ostream& os, const glm::mat4& mat); //todo -> to custom matrix wrapper class
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const std::vector<std::vector<float>> COLOURS_TO_PICK  = {
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

Scene2D* scene = nullptr;
Model2D* testModel = nullptr;
Vertices2D* originalVertices = nullptr;

int main()
{
    std::cout << controlsInfo();

    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab_01 - Model2D", NULL, NULL);
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

    // build and compile shader program
    ShaderModule triShader(VSHADER_PATH, FSHADER_PATH);

    //scene create
    scene = new Scene2D();
    scene->getCamera().setViewport(SCR_WIDTH, SCR_HEIGHT);
    scene->getCamera().updAxes();

    Vertices2D convexPolygon;
    for (int i = 0; i < POLYGON_N_SIDES; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / POLYGON_N_SIDES;
        float x = POLYGON_RADIUS * glm::cos(angle);
        float y = POLYGON_RADIUS * glm::sin(angle);
        convexPolygon.addVertex(x, y);
    }

    originalVertices = new Vertices2D(convexPolygon);

    Edges polygonEdges;
    for (int i = 0; i < POLYGON_N_SIDES - 1; ++i) {
        polygonEdges.addEdge(i, i + 1);
    }
    polygonEdges.addEdge(POLYGON_N_SIDES - 1, 0);

    //create model
    testModel = new Model2D(convexPolygon, polygonEdges);
    scene->addModel(*testModel);

    //setup buffers
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //gl wireframe rendering
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        triShader.bind();

        //Upd projection matrix for shader
        glm::mat4 projection = scene->getCamera().getProjectionMatrix();

        triShader.setMat4("projection", projection);
        triShader.setMat4("view", view);
        triShader.setMat4("model", model);

        // Get the transformed vertices for rendering
        Vertices2D currentVertices = testModel->getVertices();
        VerticesMatrix transformedVertices = currentVertices.getVertices();

        //vertex data for OpenGL
        int i = 0;
        std::vector<float> vertices;
        for (const auto& vertex : transformedVertices) {
            vertices.push_back(vertex.x());
            vertices.push_back(vertex.y());
            vertices.push_back(0.0f);
            
            // colors
            int id_colour = i % COLOURS_TO_PICK.size();
            vertices.insert(vertices.end(),COLOURS_TO_PICK[id_colour].begin(),COLOURS_TO_PICK[id_colour].end());
            ++i;
        }

        // edge indices
        std::vector<unsigned int> indices;
        for (unsigned int i = 0; i < transformedVertices.size(); ++i) {
            indices.push_back(i);
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
        glClearColor(0.69, 0.79, 0.85, 0.74f);
        glClear(GL_COLOR_BUFFER_BIT);

        scene->render();
        // render model
        glBindVertexArray(VAO);
        //glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
        glDrawElements(GL_TRIANGLE_FAN, indices.size(), GL_UNSIGNED_INT, 0);
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
    ss << std::string(17, '=') << "\nControls\n"
       << "W/A/S/D -> Translate up/down/right/left\n"
       << "Q/E -> Rotate\n"
       << "I/J -> Scale\n"
       << "X/Y -> Shear\n"
       << "R/F -> Reflect X/Y\n"
       << "1: Reflect around custom axis, X\n"
       << "2: Reflect around custom axis, Y\n"
       << "3: Reflect around custom axis, X and Y\n"
       << "4: Scale around custom axis, X\n"
       << "5: Scale around custom axis, Y\n"
       << "6: Scale around custom axis, X and Y\n"
       << "7: Shear along custom axis, X\n"
       << "8: Shear along custom axis, Y\n"
       << "9: Shear along custom axis, X and Y\n"
       << "T -> Reset to Init\n"
       << "ESC -> Exit.\n";
    return ss.str();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Vertices2D currentVertices = testModel->getVertices();
    VerticesMatrix verticesMatrix = currentVertices.getVertices();
    glm::vec2 customPoint1 = verticesMatrix.front().getCartesianCoordinates();
    glm::vec2 customPoint2 = verticesMatrix[3].getCartesianCoordinates();

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            // Translate
            case GLFW_KEY_W:
                testModel->translate(0.0f, TRANSFORM_COEF.at("TRANSLATE"));
                break;
            case GLFW_KEY_S:
                testModel->translate(0.0f, -TRANSFORM_COEF.at("TRANSLATE"));
                break;
            case GLFW_KEY_A:
                testModel->translate(-TRANSFORM_COEF.at("TRANSLATE"), 0.0f);
                break;
            case GLFW_KEY_D:
                testModel->translate(TRANSFORM_COEF.at("TRANSLATE"), 0.0f);
                break;
                
            // Rotate
            case GLFW_KEY_Q:
                testModel->rotate(glm::radians(TRANSFORM_COEF.at("ROTATE_ANGLE")));
                break;
            case GLFW_KEY_E:
                testModel->rotate(glm::radians(-TRANSFORM_COEF.at("ROTATE_ANGLE")));
                break;
                
            // Sheare
            case GLFW_KEY_X:
                testModel->shear(TRANSFORM_COEF.at("SHEAR"), 0.0f);
                break;
            case GLFW_KEY_Y:
                testModel->shear(0.0f, TRANSFORM_COEF.at("SHEAR"));
                break;

            //Scale
            case GLFW_KEY_I:
                testModel->scale(TRANSFORM_COEF.at("SCALE_IN"), 1.f);
                break;
            case GLFW_KEY_J:
                testModel->scale(TRANSFORM_COEF.at("SCALE_OUT"), 1.f);
                break;
                
            //Reflect
            case GLFW_KEY_R:
                testModel->reflect(true, false);
                break;
            case GLFW_KEY_F:
                testModel->reflect(false, true);
                break;

            //Reflect with axis
            case GLFW_KEY_1:
                testModel->reflectWithAxis(customPoint1, customPoint2, true, false);
                break;
            case GLFW_KEY_2:
                testModel->reflectWithAxis(customPoint1, customPoint2, false, true);
                break;
            case GLFW_KEY_3:
                testModel->reflectWithAxis(customPoint1, customPoint2, true, true);
                break;
            
            //Scale with axis
            case GLFW_KEY_4:
                testModel->scaleWithAxis(customPoint1, customPoint2, 1.1f, 1.1f);
                break;
            case GLFW_KEY_5:
                testModel->scaleWithAxis(customPoint1, customPoint2, 1.f, (1.f/1.1));
                break;
            case GLFW_KEY_6:
                testModel->scaleWithAxis(customPoint1, customPoint2, 1.f, 1.1f);
                break;
            
            //Shear with axis
            case GLFW_KEY_7:
                testModel->shearWithAxis(customPoint1, customPoint2, 
                    -TRANSFORM_COEF.at("SHEAR"), 0.f);
                break;
            case GLFW_KEY_8:
                testModel->shearWithAxis(customPoint1, customPoint2, 
                    0.f, TRANSFORM_COEF.at("SHEAR"));
                break;
            case GLFW_KEY_9:
                testModel->shearWithAxis(customPoint1, customPoint2, 
                    TRANSFORM_COEF.at("SHEAR"), 0.f);
                break;
                
            case GLFW_KEY_T:
                testModel->resetTransformation();
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