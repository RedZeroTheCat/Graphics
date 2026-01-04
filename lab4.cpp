#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>

const char* vertexShaderFlat = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightDir;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vec3 norm = normalize(aNormal);
    float intensity = max(dot(norm, -lightDir), 0.0);
    FragColor = vec3(intensity);
}
)";

const char* vertexShaderGouraud = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightDir;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vec3 norm = normalize(aNormal);
    float intensity = max(dot(norm, -lightDir), 0.0);
    FragColor = vec3(intensity);
}
)";

const char* fragmentShader = R"(
#version 330 core
in vec3 FragColor;
out vec4 FragOutput;

void main()
{
    FragOutput = vec4(FragColor, 0.9);
}
)";

/*
std::vector<float> vertices = {
        // Координаты         // Нормали
        // Задняя грань
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  // 0
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  // 1
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  // 2
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  // 3

        // Передняя грань
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  // 4
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  // 5
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  // 6
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  // 7

        // Левая грань
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  // 8
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  // 9
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  // 10
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  // 11

        // Правая грань
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  // 12
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  // 13
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  // 14
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  // 15

        // Верхняя грань
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  // 16
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  // 17
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  // 18
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  // 19

        // Нижняя грань
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  // 20
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  // 21
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  // 22
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f   // 23
};


std::vector<unsigned int> indices = {
        // Задняя грань
        0, 1, 2, 2, 3, 0,
        // Передняя грань
        4, 5, 6, 6, 7, 4,
        // Левая грань
        0, 3, 7, 7, 4, 0,
        // Правая грань
        1, 2, 6, 6, 5, 1,
        // Верхняя грань
        3, 2, 6, 6, 7, 3,
        // Нижняя грань
        0, 1, 5, 5, 4, 0
}; */

std::vector<float> vertices;
std::vector<unsigned int> indices;

const unsigned int LONGITUDE_SEGMENTS = 36;  // Количество сегментов по долготе
const unsigned int LATITUDE_SEGMENTS = 18;   // Количество сегментов по широте
const float RADIUS = 1.0f;

void generateSphereMesh() {
    // Генерация вершин
    for (unsigned int lat = 0; lat <= LATITUDE_SEGMENTS; ++lat) {
        float theta = lat * M_PI / LATITUDE_SEGMENTS; // Угол в радианах по широте (от 0 до PI)
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (unsigned int lon = 0; lon <= LONGITUDE_SEGMENTS; ++lon) {
            float phi = lon * 2.0f * M_PI / LONGITUDE_SEGMENTS; // Угол по долготе (от 0 до 2PI)
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            // Координаты вершины
            float x = RADIUS * sinTheta * cosPhi;
            float y = RADIUS * cosTheta;
            float z = RADIUS * sinTheta * sinPhi;

            // Нормаль совпадает с позицией для единичной сферы
            float nx = x / RADIUS;
            float ny = y / RADIUS;
            float nz = z / RADIUS;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
    }

    // Генерация индексов
    for (unsigned int lat = 0; lat < LATITUDE_SEGMENTS; ++lat) {
        for (unsigned int lon = 0; lon < LONGITUDE_SEGMENTS; ++lon) {
            unsigned int current = lat * (LONGITUDE_SEGMENTS + 1) + lon;
            unsigned int next = current + LONGITUDE_SEGMENTS + 1;

            // Первый треугольник
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            // Второй треугольник
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }
}

float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rotationX += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rotationX -= 1.0f;

    if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        rotationY += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
        rotationY -= 1.0f;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rotationZ += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rotationZ -= 1.0f;
}

int main() {
    bool useFlatShading;
    char flag;
    std::cout << "Использовать гладкое гладкое затенение? Y/n" << std::endl;
    std::cin >> flag;
    if (flag == 'Y') {
        useFlatShading = true;
    }
    else {
        useFlatShading = false;
    }

    generateSphereMesh();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Showcase", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // Компиляция шейдеров
    unsigned int vertexShader, fragmentShaderProgram, shaderProgram;
    fragmentShaderProgram = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderProgram, 1, &fragmentShader, NULL);
    glCompileShader(fragmentShaderProgram);

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, !useFlatShading ? &vertexShaderFlat : &vertexShaderGouraud, NULL);
    glCompileShader(vertexShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShaderProgram);
    glLinkProgram(shaderProgram);

    // Буферы
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(glGetUniformLocation(shaderProgram, "lightDir"), 0.0f, 0.0f, -1.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}