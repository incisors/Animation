#pragma once

#ifndef SKELETON_H
#define SKELETON_H

#include "core.h"
#include "Joint.h"
#include "Tokenizer.h"
#include <vector>
#include <string>
#include <cstring>

class Skeleton
{

public:
    Joint *root;
    glm::mat4 parentMatrix;
    std::vector<Joint *> joints;
    Skeleton();
    ~Skeleton();
    glm::mat4 getWorldMatrix(int joint)
    {
        return joints[joint]->getWorldMatrix();
    }
    bool load(const char *file);                        // Load the skeleton data, this might involve
    void update(glm::mat4 Matrix);                      // Traverse the tree and compute all joint matrices
    void draw(glm::mat4 viewProjMatrix, GLuint shader); // Implement the drawing logic for the entire skeleton
};

#endif // SKELETON_H
