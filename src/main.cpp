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
void createModels();
const unsigned int SCR_WIDTH = 900;
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
const unsigned int POLYGON_N_SIDES = 5;

const std::string VSHADER_PATH = "src/shaders/vshader.glsl";
const std::string FSHADER_PATH = "src/shaders/fshader.glsl";

bool wireframe = true;
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Лабораторная работа 1.2: составные аффинные преобразования каркасной 2D модели", NULL, NULL);
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

    std::vector<Model2D*> models;
    createModels();

    //setup buffers
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //gl wireframe rendering
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else 
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
            if (wireframe)
                vertices.insert(vertices.end(), {0, 0, 0});
            else{
                int id_colour = i % COLOURS_TO_PICK.size();
                vertices.insert(vertices.end(),COLOURS_TO_PICK[id_colour].begin(),COLOURS_TO_PICK[id_colour].end());
                ++i;
            }
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
        glClearColor(0.89, 0.93, 0.98, 1.f);//0.69, 0.79, 0.85, 0.74f);
        glClear(GL_COLOR_BUFFER_BIT);

        scene->render();
        // render model
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLE_FAN, indices.size(), GL_UNSIGNED_INT, 0);
        // if (wireframe)
        //     glDrawElements(GL_TRIANGLE_FAN, indices.size(), GL_UNSIGNED_INT, 0);
        // else 
        //     glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);        
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
       << "Z/C -> Rotate around point\n"
       << "V/B -> Scale around point\n"
       << "L/M -> Shear around point\n"
       << "U/O -> Rotate around center\n"
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
       << "P -> Change draw mode\n"
       << "N -> Next Model\n"
       << "ESC -> Exit.\n";
    return ss.str();
}

void createModels()
{
    // {// ===== convexPolygon 9 =====
    //     Vertices2D convexPolygon;
    //     for (int i = 0; i < POLYGON_N_SIDES; ++i) {
    //         float angle = 2.0f * glm::pi<float>() * i / POLYGON_N_SIDES;
    //         float x = POLYGON_RADIUS * glm::cos(angle);
    //         float y = POLYGON_RADIUS * glm::sin(angle);
    //         convexPolygon.addVertex(x, y);
    //     }
    //     originalVertices = new Vertices2D(convexPolygon);
    //     Edges polygonEdges;
    //     for (int i = 0; i < POLYGON_N_SIDES - 1; ++i) {
    //         polygonEdges.addEdge(i, i + 1);
    //     }
    //     polygonEdges.addEdge(POLYGON_N_SIDES - 1, 0);
    //     testModel = new Model2D(convexPolygon, polygonEdges);
    //     scene->addModel(*testModel);
    // }
    {// ===== TRIANGLE =====
        Vertices2D triangle;
        triangle.addVertex(0.0f, 0.3f);
        triangle.addVertex(-0.3f, -0.3f);
        triangle.addVertex(0.3f, -0.3f);

        Edges triangleEdges;
        triangleEdges.addEdge(0,1);
        triangleEdges.addEdge(1,2);
        triangleEdges.addEdge(2,0);

        testModel = new Model2D(triangle, triangleEdges);
        scene->addModel(*testModel);
    }

    {// ===== SQUARE =====
        Vertices2D square;
        square.addVertex(-0.3f, 0.3f);
        square.addVertex(0.3f, 0.3f);
        square.addVertex(0.3f, -0.3f);
        square.addVertex(-0.3f, -0.3f);

        Edges squareEdges;
        squareEdges.addEdge(0,1);
        squareEdges.addEdge(1,2);
        squareEdges.addEdge(2,3);
        squareEdges.addEdge(3,0);

        Model2D* model3 = new Model2D(square, squareEdges);
        scene->addModel(*model3);
    }
    
    {// ===== HEXAGON =====
        Vertices2D convexPolygon;
        int N_SIDES = 6;
        for (int i = 0; i < N_SIDES; ++i) {
            float angle = 2.0f * glm::pi<float>() * i / N_SIDES;
            float x = POLYGON_RADIUS * glm::cos(angle);
            float y = POLYGON_RADIUS * glm::sin(angle);
            convexPolygon.addVertex(x, y);
        }
        originalVertices = new Vertices2D(convexPolygon);
        Edges polygonEdges;
        for (int i = 0; i < N_SIDES - 1; ++i) {
            polygonEdges.addEdge(i, i + 1);
        }
        polygonEdges.addEdge(N_SIDES - 1, 0);
        Model2D* poly6Model = new Model2D(convexPolygon, polygonEdges);
        scene->addModel(*poly6Model);
    }

    {// ===== STAR (FAN) =====
        Vertices2D star;
        star.addVertex(0.0f, 0.0f);
        const int N = 10;
        float R_outer = 0.4f;
        float R_inner = 0.18f;
        for (int i = 0; i <= N; ++i) {
            float angle = i * 2.0f * glm::pi<float>() / N;

            float r = (i % 2 == 0) ? R_outer : R_inner;

            float x = r * glm::cos(angle);
            float y = r * glm::sin(angle);
            star.addVertex(x, y);
        }

        Edges starEdges;
        for (int i = 1; i <= N; ++i)
            starEdges.addEdge(i, i + 1);

        Model2D* starModel = new Model2D(star, starEdges);
        scene->addModel(*starModel);
    }

    {// ===== HOUSE (TRIANGLE FAN READY) =====
        Vertices2D house;
        house.addVertex(0.0f, 0.0f);
        house.addVertex(-0.3f, -0.3f); // 1
        house.addVertex( 0.3f, -0.3f); // 2
        house.addVertex( 0.3f,  0.1f); // 3
        house.addVertex( 0.0f,  0.5f); // 4 roof peak
        house.addVertex(-0.3f,  0.1f); // 5
        house.addVertex(-0.3f, -0.3f); // back to base (optional not required)

        Edges houseEdges;
        for (int i = 1; i < 5; ++i)
            houseEdges.addEdge(i, i + 1);
        houseEdges.addEdge(5, 1);

        Model2D* modelHouse = new Model2D(house, houseEdges);
        scene->addModel(*modelHouse);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Vertices2D currentVertices = testModel->getVertices();
    VerticesMatrix verticesMatrix = currentVertices.getVertices();
    glm::vec2 customPoint1 = verticesMatrix.front().getCartesianCoordinates();
    glm::vec2 customPoint2 = verticesMatrix[3].getCartesianCoordinates();

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            //rotate around centroid
            case GLFW_KEY_U:
                testModel->rotateAroundCenter(
                    glm::radians(TRANSFORM_COEF.at("ROTATE_ANGLE"))
                );
                break;
            case GLFW_KEY_O:
                testModel->rotateAroundCenter(
                    glm::radians(-TRANSFORM_COEF.at("ROTATE_ANGLE"))
                );
                break;

            //shear around
            case GLFW_KEY_L:
                testModel->shearAroundPoint(
                    customPoint1,
                    TRANSFORM_COEF.at("SHEAR"),
                    0.0f
                );
                break;
            case GLFW_KEY_M:
                testModel->shearAroundPoint(
                    customPoint1,
                    0.0f,
                    TRANSFORM_COEF.at("SHEAR")
                );
                break;
            
            //scale around point
            case GLFW_KEY_V:
                testModel->scaleAroundPoint(
                    customPoint1,
                    TRANSFORM_COEF.at("SCALE_IN"),
                    TRANSFORM_COEF.at("SCALE_IN")
                );
                break;
            case GLFW_KEY_B:
                testModel->scaleAroundPoint(
                    customPoint1,
                    TRANSFORM_COEF.at("SCALE_OUT"),
                    TRANSFORM_COEF.at("SCALE_OUT")
                );
                break;

            //rotate around point
            case GLFW_KEY_Z:
                testModel->rotateAroundPoint(
                    customPoint1,
                    glm::radians(TRANSFORM_COEF.at("ROTATE_ANGLE"))
                );
                break;
            case GLFW_KEY_C:
                testModel->rotateAroundPoint(
                    customPoint1,
                    glm::radians(-TRANSFORM_COEF.at("ROTATE_ANGLE"))
                );
                break;

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
                testModel->scaleWithAxis(customPoint1, customPoint2, 1.1f, 1.f);
                break;
            case GLFW_KEY_5:
                testModel->scaleWithAxis(customPoint1, customPoint2, 1.f, (1.1f));
                break;
            case GLFW_KEY_6:
                testModel->scaleWithAxis(customPoint1, customPoint2, 1.1f, 1.1f);
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
                    TRANSFORM_COEF.at("SHEAR"),TRANSFORM_COEF.at("SHEAR"));
                break;
                
            case GLFW_KEY_T:
                testModel->resetTransformation();
                break;
            case GLFW_KEY_P:
                wireframe = !wireframe;
                if (wireframe)
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                break;
            case GLFW_KEY_N:
                scene->toNextModel();
                testModel = scene->getCurrModel();
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