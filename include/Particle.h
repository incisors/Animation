#pragma once

#include "core.h"
#include <iostream>

class Particle {
public:
    void ApplyForce(glm::vec3 &f) {
        Force+=f;
    }

    void Integrate(float deltaTime) {
        if (isFixed) return; // Do not integrate if the particle is fixed (pinned to the world

        glm::vec3 accel=(1/Mass) * Force; // Apply Newton’s Second Law (f=ma)
        Velocity += accel*deltaTime; // Forward Euler integration to get new velocity
        Position += Velocity*deltaTime; // Backward Euler integration to get new position
        Force=glm::vec3(0); // Zero force out so next frame will start fresh
    }

    void ApplyImpulse(const glm::vec3& imp) {
        Velocity+=imp/Mass;
    }

    glm::vec3 Position = glm::vec3(0, 0, 0);
    glm::vec3 Normal = glm::vec3(0, 1, 0);
    glm::vec3 Velocity = glm::vec3(0, 0, 0);
    glm::vec3 Force = glm::vec3(0, 0, 0);
    float Mass = 1.0f;
    bool isFixed = false;
};
