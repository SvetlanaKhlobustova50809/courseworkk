#include "ShaderProgram.h"
#include "Camera.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int useWireframe = 0;
int displayGrayscale = 0;
static bool keys[1024];

Camera camera(glm::vec3(67.0f, 627.5f, 169.9f), glm::vec3(0.0f, 1.0f, 0.0f), -128.1f, -42.4f);
GLfloat lastX = SCR_WIDTH / 2.0f;
GLfloat lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
const char* way;
char* speed;
int width, height, nrChannels;
unsigned int terrainVAO, terrainVBO, terrainIBO;
std::vector<GLfloat> vertices;
std::vector<unsigned> indices;

/*!
    @defgroup generate_mazes Заполнение сетки
    @ingroup maze
    @brief Данный модуль, предназначен для заполнения сетки.
*/
///@{
    /*! @brief Функция инициализации массива vertices
        @param width, height количество клеток в высоту и ширину сетки
        @param data массив с пиксельными данными карты высот
        @param bytePerPixel байт пикселя в перспективе
    */
void vertexGridPlaceholder(int width, int height, unsigned char* data, unsigned bytePerPixel) {

    GLfloat yScale = 64.0f / 256.0f, yShift = 16.0f;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            unsigned char* pixelOffset = data + (j + width * i) * bytePerPixel;
            unsigned char y = pixelOffset[0];

            // vertex
            vertices.push_back(-height / 2.0f + height * i / (GLfloat)height);   // vx
            vertices.push_back((int)y * yScale - yShift);   // vy
            vertices.push_back(-width / 2.0f + width * j / (GLfloat)width);   // vz
        }
    }
}

    /*! @brief Функция инициализации массива indices
        @param width, height количество клеток в высоту и ширину сетки
        @param rez шаг, с которым нужно заполнять массив indices
    */
void indicesGridPlaceholder(int width, int height, int rez) {
    for (unsigned i = 0; i < height - 1; i += rez)
    {
        for (unsigned j = 0; j < width; j += rez)
        {
            for (unsigned k = 0; k < 2; k++)
            {
                indices.push_back(j + width * (i + k * rez));
            }
        }
    }
}

    /*! @brief Функция настройки Объекта массива вершин (VAO) на графическом процессоре.
    */
void VAO_Setup() {
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &terrainIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);
}

    /*! @brief Функция визуализации сетки полосу за полосой.
        @param numStrips номер полосы
        @param numTrisPerStrip количество треугольников, из которых будет состоять полоса
    */
void visualisationGrip(const int numStrips, const int numTrisPerStrip) {
    glBindVertexArray(terrainVAO);
    for (unsigned strip = 0; strip < numStrips; strip++)
    {
        glDrawElements(GL_TRIANGLE_STRIP, numTrisPerStrip + 2, GL_UNSIGNED_INT, (void*)(sizeof(unsigned) * (numTrisPerStrip + 2) * strip));
    }
}
///@}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");
    
    way = "../textures/2.png";
    if (argc == 2) {
        way = argv[1];
    }
    unsigned char* data = stbi_load(way, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cout << "Введите путь до картинки в формате ../textures/2.png";
        return 0;
    }

    // glfw: инициализация и настройка
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw создание окна
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Landscape project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // говорит GLFW о том, чтобы захватить мышь
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: загружает все OpenGL функции указателей
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // строит и компилирует шейдерную програму
    Shader heightMapShader("../shaders/8.3.cpuheight.vs", "../shaders/8.3.cpuheight.fs");  
    
    if (data)
    {
        std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    int rez = 1;
    unsigned bytePerPixel = nrChannels;
    vertexGridPlaceholder(width,height,data,bytePerPixel);

    std::cout << "Loaded " << vertices.size() / 3 << " vertices" << std::endl;
    stbi_image_free(data);

    indicesGridPlaceholder(width, height, rez);
    std::cout << "Loaded " << indices.size() << " indices" << std::endl;

    const int numStrips = (height - 1) / rez;
    const int numTrisPerStrip = (width / rez) * 2 - 2;
    std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
    std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;

    VAO_Setup();

    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        
        deltaTime = (currentFrame - lastFrame)*40;
        lastFrame = currentFrame;
        glfwPollEvents();

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        heightMapShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        heightMapShader.setMat4("projection", projection);
        heightMapShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        heightMapShader.setMat4("model", model);

        visualisationGrip(numStrips, numTrisPerStrip);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &terrainVAO);
    glDeleteBuffers(1, &terrainVBO);
    glDeleteBuffers(1, &terrainIBO);

    glfwTerminate();
    return 0;
}

// обработка нажатий соответствующих клавиш в кадре
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Обратный вызов key
void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        default:
            break;
        }
    }
}

// Обработка движений мыши
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// Обработка вращения колесика мыши
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}