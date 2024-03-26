#pragma once

#include <vector>

#include "core.h"

class Cube {
public:
    GLuint VAO;
    GLuint VBO_positions, VBO_normals, EBO;

    glm::mat4 model;
    glm::vec3 color;

    // Cube Information
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;


    Cube(glm::vec3 cubeMin = glm::vec3(-1, -1, -1), glm::vec3 cubeMax = glm::vec3(1, 1, 1));
    ~Cube();

    void draw(const glm::mat4& viewProjMtx, GLuint shader);
    void update();

    void spin(float deg);
};