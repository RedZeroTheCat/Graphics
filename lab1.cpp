#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

const int NUM_SIDES = 6; // число сторон многоугольника
float angle = 0.0f;
float scale = 1.0f;
bool moveInCircle = false;
float circleRadius = 0.5f;
float angleOffset = 0.0f;

glm::vec2 position(0.0f, 0.0f);

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) position.y += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) position.y -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) position.x -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) position.x += 0.01f;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) scale += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) scale -= 0.01f;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) angle += 0.01f * M_PI;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) angle -= 0.01f * M_PI;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        moveInCircle = !moveInCircle;
    }
}

void drawPolygon(int num_sides) {
    glColor3f(1.0f, 0.0f, 0.0f);
    std::vector<glm::vec2> vertices;
    float inAngle = 2 * M_PI / num_sides;
    float radian = angle;
    for (int i = 0; i < num_sides; ++i) {
        vertices.emplace_back(cos(radian), sin(radian));
        radian += inAngle;
    }

    glBegin(GL_POLYGON);
    for (const auto& vertex : vertices) {
        glVertex2f(vertex.x * scale + position.x, vertex.y * scale + position.y);
    }
    glEnd();
}

void updatePosition() {
    if (moveInCircle) {
        angleOffset += 0.01f;
        position.x = circleRadius * cos(angleOffset);
        position.y = circleRadius * sin(angleOffset);
    }
}

int main() {
    int num_sides;
    std::cout << "Введите число граней:" << std::endl;
    std::cin >> num_sides;

    // Инициализация GLFW
    if (!glfwInit()) return -1;

    // Создание окна
    GLFWwindow *window = glfwCreateWindow(800, 800, "Showcase", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Инициализация GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 800);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        if (moveInCircle) {
            updatePosition();
        }

        glClear(GL_COLOR_BUFFER_BIT);
        drawPolygon(num_sides);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
