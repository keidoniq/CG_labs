#include <iostream>
#include <string>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include "shaders/ShaderModule.h"
#include "Model2D.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const std::vector<std::vector<float>> COLOURS_TO_PICK  = {
    {0, 0, 1},
    {0, 1, 0},
    {1, 0, 0},
};

const std::map<std::string, float> TRANSFORM_COEF{
    {"TRANSLATE", 0.05f}, {"SCALE", 0.1f},
    {"ROTATE_ANGLE", 10}, {"SHEAR", 0.1f}
};

const float POLYGON_RADIUS = 0.4f;
const unsigned int POLYGON_N_SIDES = 6;

const std::string VSHADER_PATH = "src/shaders/vshader.glsl";
const std::string FSHADER_PATH = "src/shaders/fshader.glsl";

Model2D* testModel = nullptr;
Vertices2D* originalVertices = nullptr;

int main()
{
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

    // glad: loading OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // build and compile shader program
    ShaderModule triShader(VSHADER_PATH, FSHADER_PATH); 

    //model for testing
    Vertices2D squareVertices;
    squareVertices.addVertex(-0.3f, 0.f);  // 0
    squareVertices.addVertex(-0.3f, 0.3f);   // 1
    squareVertices.addVertex(0.3f, 0.3f);    // 2
    squareVertices.addVertex(0.6f, 0.f);   // 3
    squareVertices.addVertex(0.6f, -0.3f);   // 4
    squareVertices.addVertex(0.6f, -0.6f);   // 5
    squareVertices.addVertex(0.3f, -0.9f);   // 6
    squareVertices.addVertex(0.f, -0.6f);   // 7

    Vertices2D convexPolygon;
    for (int i = 0; i < POLYGON_N_SIDES; ++i) {
        float angle = 2.0f * glm::pi<float>() * i / POLYGON_N_SIDES;
        float x = POLYGON_RADIUS * glm::cos(angle);
        float y = POLYGON_RADIUS * glm::sin(angle);
        convexPolygon.addVertex(x, y);
    }

    //originalVertices = new Vertices2D(squareVertices);
    originalVertices = new Vertices2D(convexPolygon);

    Edges2D squareEdges;
    squareEdges.addEdge(0, 1);  
    squareEdges.addEdge(1, 2);
    squareEdges.addEdge(2, 3);
    squareEdges.addEdge(3, 4);
    squareEdges.addEdge(4, 5);
    squareEdges.addEdge(5, 6);
    squareEdges.addEdge(6, 7);
    squareEdges.addEdge(7, 0);
    squareEdges.addEdge(5, 7);

    Edges2D polygonEdges;
    for (int i = 0; i < POLYGON_N_SIDES - 1; ++i) {
        polygonEdges.addEdge(i, i + 1);
    }
    polygonEdges.addEdge(POLYGON_N_SIDES - 1, 0);

    //create model
    //testModel = new Model2D(squareVertices, squareEdges);
    testModel = new Model2D(convexPolygon, polygonEdges);

    //setup buffers
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //gl wireframe rendering
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    std::cout << std::string(17, '=') << "\nControls\n"
        << "W/A/S/D -> Translate up/down/right/left\n"
        << "Q/E -> Rotate\n"
        << "Mouse scroll -> Scale\n"
        << "X/Y -> Shear\n"
        << "R/F -> Reflect X/Y\n"
        << "T -> Reset to Init\n"
        << "ESC -> Exit.\n";

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        triShader.bind();

        // Apply any pending transformations
        testModel->applyTransformation();

        // Get the transformed vertices for rendering
        Vertices2D currentVertices = testModel->getVertices();
        Matrix transformedVertices = currentVertices.getVertices();

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
        // const auto& edges = squareEdges.getEdges();
        // for (const auto& edge : edges) {
        //     indices.push_back(edge.first);
        //     indices.push_back(edge.second);
        // }
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
    delete testModel;
    delete originalVertices;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            // Translate
            case GLFW_KEY_W:
                testModel->translate(0.0f, TRANSFORM_COEF.at("TRANSLATE"));
                std::cout << "Translate: Up\n";
                break;
            case GLFW_KEY_S:
                testModel->translate(0.0f, -TRANSFORM_COEF.at("TRANSLATE"));
                std::cout << "Translate: Down\n";
                break;
            case GLFW_KEY_A:
                testModel->translate(-TRANSFORM_COEF.at("TRANSLATE"), 0.0f);
                std::cout << "Translate: Left\n";
                break;
            case GLFW_KEY_D:
                testModel->translate(TRANSFORM_COEF.at("TRANSLATE"), 0.0f);
                std::cout << "Translate: Right\n";
                break;
                
            // Rotate
            case GLFW_KEY_Q:
                testModel->rotate(glm::radians(TRANSFORM_COEF.at("ROTATE_ANGLE")));
                std::cout << "Rotate: clockwise\n";
                break;
            case GLFW_KEY_E:
                testModel->rotate(glm::radians(-TRANSFORM_COEF.at("ROTATE_ANGLE")));
                std::cout << "Rotate: otherwise\n";
                break;
                
            // Sheare
            case GLFW_KEY_X:
                testModel->shear(TRANSFORM_COEF.at("SHEAR"), 0.0f);
                std::cout << "Shear: X-axis\n";
                break;
            case GLFW_KEY_Y:
                testModel->shear(0.0f, TRANSFORM_COEF.at("SHEAR"));
                std::cout << "Shear: Y-axis\n";
                break;
                
            // Reflect
            case GLFW_KEY_R:
                testModel->reflect(true, false);
                std::cout << "Reflect: X-axis\n";
                break;
            case GLFW_KEY_F:
                testModel->reflect(false, true);
                std::cout << "Reflect: Y-axis\n";
                break;
                
            // Reset
            case GLFW_KEY_T:
                testModel->resetTransformation();
                std::cout << "Reset all transformations\n";
                break;
        }
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    printf("Scroll: Y=%.2f\n", yoffset);
    float coef = yoffset > 0 ? 1.1:0.9;
    testModel->scale(coef, coef);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}