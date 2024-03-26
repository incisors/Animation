#pragma once

#include "core.h"
#include "Vertex.h"
#include "Particle.h"

class Triangle {
public:
    
    Particle* P1;
    Particle* P2;
    Particle* P3;
    glm::vec3 Normal;

private:
};