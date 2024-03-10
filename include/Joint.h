#pragma once

#ifndef JOINT_H
#define JOINT_H

#include <vector>
#include <string>
#include "core.h"
#include "DOF.h"
#include "Cube.h" // Make sure you include the header for Cube if it's used
#include "Tokenizer.h"
class Joint {
public:
    std::string name;
    glm::mat4 localMatrix;
    glm::mat4 worldMatrix = glm::mat4(1.0f);

    DOF* dofx;
    DOF* dofy;
    DOF* dofz;

    std::vector<Joint*> children;
    Joint* parent;
    
    glm::vec3 offset;
    glm::vec3 boxmin;
    glm::vec3 boxmax;

    Cube* cube; // Ensure that Cube is defined somewhere, otherwise, this will cause an error
    
    Joint();
    ~Joint();

    glm::mat4 getWorldMatrix() { 
        if (worldMatrix != glm::mat4(1.0f)){
            return worldMatrix;
        }
        return glm::mat4(1.0f); 
    }
    void update(const glm::mat4& parentMatrix); // Include parameter type
    bool load(Tokenizer &t, std::vector<Joint*> &joints); // Load joint data, this might involve loading DOFs, etc.
    void addChild(Joint* child);
    void setParent(Joint* parent);
    void setRotation(glm::vec3 rot);
    void draw(const glm::mat4& viewProjMatrix, GLuint shader); // Include parameter type

    void incX() { dofx->inc(); }
    void decX() { dofx->dec(); }
    void incY() { dofy->inc(); }
    void decY() { dofy->dec(); }
    void incZ() { dofz->inc(); }
    void decZ() { dofz->dec(); }

};

#endif // JOINT_H
