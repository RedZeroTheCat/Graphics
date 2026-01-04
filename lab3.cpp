#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_truetype.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <sstream>

// Shader sources
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.8, 0.3, 0.3, 1.0);
}
)";

// Transformation matrices
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);

// Variables for transformations
float rotationX = 0.0f;
float rotationY = 0.0f;
float scale = 1.0f;
float translateX = 0.0f;
float translateY = 0.0f;
float translateZ = 0.0f;
int transformationOrder = 1;
// 1: Scale -> Rotate -> Translate,
// 2: Translate -> Rotate -> Scale,
// 3: Rotate -> Scale -> Translate

// Generate sphere vertices
std::vector<float> generateSphereVertices(float radius, int sectorCount, int stackCount) {
    std::vector<float> vertices;
    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stackCount;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * 2 * glm::pi<float>() / sectorCount;
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    return vertices;
}

// Render matrix as text
void renderMatrix(glm::mat4 matrix) {
    const float* ptr = glm::value_ptr(matrix);
    for (int row = 0; row < 4; ++row) {
        std::ostringstream ss;
        for (int col = 0; col < 4; ++col) {
            ss << ptr[col * 4 + row] << " ";
        }
        std::cout << ss.str().c_str() << std::endl;
    }
}


// Callback for window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Input handling
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        translateY += 0.05f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        translateY -= 0.05f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        translateX -= 0.05f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        translateX += 0.05f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
        translateZ -= 0.05f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        translateZ += 0.05f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        rotationX += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        rotationX -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        rotationY -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        rotationY += 1.0f;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        scale += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        scale -= 0.01f;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        transformationOrder = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        transformationOrder = 2;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        transformationOrder = 3;
}


int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Showcase", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load OpenGL function pointers using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Build and compile shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Generate sphere data
    std::vector<float> sphereVertices = generateSphereVertices(1.0f, 36, 18);

    // Set up vertex buffer and array objects
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Clear screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Apply transformations
        model = glm::mat4(1.0f);
        if (transformationOrder == 1) {
            model = glm::scale(model, glm::vec3(scale));
            model = glm::rotate(model, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, glm::vec3(translateX, translateY, translateZ));
        }
        else if (transformationOrder == 2) {
            model = glm::translate(model, glm::vec3(translateX, translateY, translateZ));
            model = glm::rotate(model, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(scale));
        }
        else {
            model = glm::rotate(model, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(scale));
            model = glm::translate(model, glm::vec3(translateX, translateY, translateZ));
        }

        // Render sphere
        glUseProgram(shaderProgram);
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sphereVertices.size() / 3);

        // Display transformation matrices
        renderMatrix(model);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

