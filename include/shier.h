#pragma once

#include "core.h"
#include <vector>


class Dodecahedron {
public:
    Dodecahedron(float radius, glm::vec3 position) : radius(radius), position(position) {
        generateVertices();
        generateIndices();
        setupBuffers();
    }

    ~Dodecahedron() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    void draw() {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    float radius;
    glm::vec3 position;
    std::vector<glm::vec3> vertices;
    std::vector<glm::uvec3> indices;
    GLuint vao, vbo, ebo;

    void generateVertices() {
        // 正十二面体的顶点
        float phi = (1.0f + std::sqrt(5.0f)) / 2.0f;
        float a = 1.0f;
        float b = 1.0f / phi;

        vertices = {
            { 0,  b,  a}, { b,  a,  0}, {-b,  a,  0},
            { 0,  b, -a}, { 0, -b, -a}, {-a,  0, -b},
            {-a,  0,  b}, { a,  0, -b}, { a,  0,  b},
            { b, -a,  0}, { 0, -b,  a}, {-b, -a,  0},
            { 0,  b,  a}, { a,  0,  b}, { b,  a,  0},
            { 0,  b, -a}, {-b,  a,  0}, {-a,  0, -b},
            { 0, -b, -a}, { b, -a,  0}, { a,  0, -b},
            { 0, -b,  a}, {-a,  0,  b}, {-b, -a,  0}
        };

        // Scale and translate vertices
        for (auto& v : vertices) {
            v = position + radius * glm::normalize(v);
        }
    }

    void generateIndices() {
        // 正十二面体的面
        indices = {
            {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}, {12, 13, 14},
            {15, 16, 17}, {18, 19, 20}, {21, 22, 23}, {0, 2, 6}, {0, 6, 8},
            {0, 8, 13}, {0, 13, 14}, {0, 14, 1}, {1, 14, 19}, {1, 19, 20},
            {1, 20, 16}, {1, 16, 2}, {2, 16, 17}, {2, 17, 22}, {2, 22, 6},
            {3, 4, 9}, {3, 9, 11}, {3, 11, 10}, {3, 10, 5}, {4, 5, 18},
            {4, 18, 20}, {4, 20, 19}, {4, 19, 9}, {5, 10, 23}, {5, 23, 22},
            {5, 22, 17}, {5, 17, 18}, {6, 7, 21}, {6, 21, 12}, {6, 12, 8},
            {7, 8, 13}, {7, 13, 12}, {7, 21, 23}, {7, 23, 10}, {7, 10, 9},
            {7, 9, 11}, {7, 11, 3}, {7, 3, 15}, {7, 15, 21}, {12, 13, 8},
            {14, 19, 20}, {15, 16, 17}, {18, 22, 23}, {15, 3, 5}, {15, 5, 17},
            {15, 17, 16}, {18, 5, 10}, {18, 10, 23}, {18, 23, 22}, {21, 15, 12},
            {21, 12, 13}, {21, 13, 7}, {11, 9, 3}
        };
    }

    void setupBuffers() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(glm::uvec3), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }
};
