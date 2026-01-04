cmake_minimum_required(VERSION 3.21)
project(graphics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(/usr/include/glad)
include_directories(/usr/include/stb)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

#find_package(SFML REQUIRED COMPONENTS system window graphics)

add_executable(ex1 lab1.cpp /usr/src/glad.c)
add_executable(ex2 lab2.cpp /usr/src/glad.c)
add_executable(ex3 lab3.cpp /usr/src/glad.c)
add_executable(ex4 lab4.cpp /usr/src/glad.c)
add_executable(ex5 lab5.cpp /usr/src/glad.c)

target_link_libraries(ex1 PRIVATE
        OpenGL::GL
        glfw
        glm::glm

        #sfml-system
        #sfml-window
        #sfml-graphics
)

target_link_libraries(ex2 PRIVATE
        OpenGL::GL
        glfw
        glm::glm
)

target_sources(ex3 PRIVATE /usr/include/stb/stb_image.h)

target_link_libraries(ex3 PRIVATE
        OpenGL::GL
        glfw
        glm::glm
)

target_link_libraries(ex4 PRIVATE
        OpenGL::GL
        glfw
        glm::glm
)

target_sources(ex5 PRIVATE /usr/include/stb/stb_image_write.h)

target_link_libraries(ex5 PRIVATE
        OpenGL::GL
        glfw
        glm::glm
)