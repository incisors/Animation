#include "Vertex.h"

Vertex::Vertex() {
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    normal = glm::vec3(0.0f, 0.0f, 0.0f);
    jointIndices = std::vector<int>();
    jointWeights = std::vector<float>();
    // skinWeight = 0;
}

Vertex::~Vertex() {
    for(int index : jointIndices){
        delete &index;
    }
    for(float weight : jointWeights){
        delete &weight;
    }
    delete &position;
    delete &normal;
    delete &jointIndices;
    delete &jointWeights;
    // delete &skinWeight;
}

void Vertex::setPos(glm::vec3 pos){
    position = pos;
}

void Vertex::setNormal(glm::vec3 norm){
    normal = norm;
}

void Vertex::setSkinWeights(std::vector<int> indices, std::vector<float> weights){
    jointIndices = indices;
    jointWeights = weights;
}

std::vector<int> Vertex::getJointIndices(){
    return jointIndices;
}

std::vector<float> Vertex::getJointWeights(){
    return jointWeights;
}