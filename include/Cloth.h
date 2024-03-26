#pragma once

#include "core.h"
#include "Particle.h"
#include "SpringDamper.h"
#include "Triangle.h"
#include <iostream>
#include <vector>
#include "Ground.h"

class Cloth {
public:

    Cloth(int width, int height, GLuint programID);
    ~Cloth();

    void Update(float deltaTime);
    void Draw(const glm::mat4& viewProjMtx, GLuint shader);

    std::vector<Particle*> Particles;
    std::vector<SpringDamper*> Springs;
    std::vector<Triangle*> Triangles;

    int Width, Height;
    float ParticleMass;
    float TotalMass; // n*ParticleMass
    float forwardSpeed = 0.1f;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    // OpenGL stuff from Skin
    GLuint meshVAO;
    GLuint positionVBO;
    GLuint normalVBO;
    GLuint ebo;
    glm::vec3 color;
    glm::mat4 model;
	Ground* ground;
};
