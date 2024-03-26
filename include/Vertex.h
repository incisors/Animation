#pragma once

#include "core.h"
#include <vector>
#include <utility>

class Vertex {
public:
    Vertex();
    ~Vertex();

    void setPos(glm::vec3 pos);
    void setNormal(glm::vec3 norm);
    void setSkinWeights(std::vector<int> indices, std::vector<float> weights);
    std::vector<int> getJointIndices();
    std::vector<float> getJointWeights();


    glm::vec3 position;
    glm::vec3 normal;
    std::vector<int> jointIndices;
    std::vector<float> jointWeights;
    // std::pair<std::vector<int>, std::vector<float>> skinWeight = std::make_pair(jointIndices, jointWeights);

private:
};