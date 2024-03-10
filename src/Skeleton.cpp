#include "Skeleton.h"
#include <iostream>
Skeleton::Skeleton()
{
}

Skeleton::~Skeleton()
{
    delete root;
}

bool Skeleton::load(const char *file)
{
    Tokenizer token;
    token.Open(file);
    token.FindToken("balljoint");
    // Parse tree
    root = new Joint;
    root->load(token, joints);
    // Finish
    token.Close();
    return true;
}

void Skeleton::update(glm::mat4 Matrix)
{
    parentMatrix = Matrix;
    // // print mat4
    // std::cout << "Matrix: " << parentMatrix[0][0] << " " << parentMatrix[0][1] << " " << parentMatrix[0][2] << " " << parentMatrix[0][3] << std::endl;
    // std::cout << "Matrix: " << parentMatrix[1][0] << " " << parentMatrix[1][1] << " " << parentMatrix[1][2] << " " << parentMatrix[1][3] << std::endl;
    // std::cout << "Matrix: " << parentMatrix[2][0] << " " << parentMatrix[2][1] << " " << parentMatrix[2][2] << " " << parentMatrix[2][3] << std::endl;
    // std::cout << "Matrix: " << parentMatrix[3][0] << " " << parentMatrix[3][1] << " " << parentMatrix[3][2] << " " << parentMatrix[3][3] << std::endl;
    root->update(parentMatrix);
}

void Skeleton::draw(glm::mat4 viewProjMatrix, GLuint shader)
{

    root->draw(viewProjMatrix, shader);
}
