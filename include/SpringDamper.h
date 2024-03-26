#pragma once

#include "core.h"
#include "Particle.h"
#include <iostream>

class SpringDamper {
public:
    // SpringDamper() : SpringConstant(1.0f), DampingConstant(0.1f), RestLength(1.0f), P1(nullptr), P2(nullptr) {}

    void Initialize(Particle* p1, Particle* p2) {
        P1 = p1;
        P2 = p2;
        RestLength = glm::length(P1->Position - P2->Position); // Correctly calculate RestLength here
    }

    void ComputeForce(){
        if (!P1 || !P2) return; // Safety check to ensure pointers are valid.

        glm::vec3 e_star = P2->Position - P1->Position;
        
        float l = glm::length(e_star);
        glm::vec3 e = e_star / l;
        
        // Calculate the closing velocity (component of the relative velocity along e)
        glm::vec3 V1V2 = P1->Velocity - P2->Velocity;  // Relative velocity (P1 to P2)
        float v_close = glm::dot(V1V2, e);

        // Calculate the forces
        float f = -SpringConstant * (RestLength - l) - DampingConstant * v_close;
        glm::vec3 f_total = f * e;  // Force on P1

        // Apply equal and opposite forces to the two particles
        P1->ApplyForce(f_total);
		glm::vec3 f_total_neg = -f_total;
        P2->ApplyForce(f_total_neg);
    }

    
    // float restLength = glm::length(P1->Position - P2->Position);
    float SpringConstant = 1.0f;
    float DampingConstant = 0.1f;
    float RestLength = 1.0f;
    Particle* P1;
    Particle* P2;
};
