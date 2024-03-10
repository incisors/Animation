#include "Joint.h"
#include <iostream>

Joint::Joint()
{
    localMatrix = glm::mat4(1.0f); // Initialize matrices to identity
    worldMatrix = glm::mat4(1.0f);
    parent = nullptr;

    dofx = new DOF();
    dofy = new DOF();
    dofz = new DOF();

    // Initialize cube if needed, e.g., cube = new Cube();
}

Joint::~Joint()
{
    delete dofx;
    delete dofy;
    delete dofz;
    // Correctly delete the cube if it's dynamically allocated
    for (Joint *child : children)
    {
        delete child; // Don't call the destructor explicitly, delete will do it
    }
}

bool Joint::load(Tokenizer &token, std::vector<Joint *> &joints)
{
    joints.push_back(this);
    char temp[256];
    token.GetToken(temp);
    name = temp;

    token.FindToken("{");
    while (1)
    {
        token.GetToken(temp);

        if (strcmp(temp, "offset") == 0)
        {
            offset.x = token.GetFloat();
            offset.y = token.GetFloat();
            offset.z = token.GetFloat();
        }
        else if (strcmp(temp, "boxmin") == 0)
        {
            boxmin.x = token.GetFloat();
            boxmin.y = token.GetFloat();
            boxmin.z = token.GetFloat();
        }
        else if (strcmp(temp, "boxmax") == 0)
        {
            boxmax.x = token.GetFloat();
            boxmax.y = token.GetFloat();
            boxmax.z = token.GetFloat();
        }
        else if (strcmp(temp, "rotxlimit") == 0)
        {
            dofx->setMin(token.GetFloat());
            dofx->setMax(token.GetFloat());
        }
        else if (strcmp(temp, "rotylimit") == 0)
        {
            dofy->setMin(token.GetFloat());
            dofy->setMax(token.GetFloat());
        }
        else if (strcmp(temp, "rotzlimit") == 0)
        {
            dofz->setMin(token.GetFloat());
            dofz->setMax(token.GetFloat());
        }
        else if (strcmp(temp, "pose") == 0)
        {
            dofx->setValue(token.GetFloat());
            dofy->setValue(token.GetFloat());
            dofz->setValue(token.GetFloat());
        }
        else if (strcmp(temp, "balljoint") == 0)
        {
            Joint *jnt = new Joint();
            jnt->load(token, joints);
            addChild(jnt);
        }
        else if (strcmp(temp, "}") == 0)
            break;
        else
            token.SkipLine();
    }
    cube = new Cube(boxmin, boxmax);

    return true;
}
#define euler_hdr "glm/gtx/euler_angles.hpp"
#include euler_hdr
void Joint::update(const glm::mat4 &parentMatrix)
{
    auto angleX = glm::eulerAngleX(dofx->getValue());
    auto angleY = glm::eulerAngleY(dofy->getValue());
    auto angleZ = glm::eulerAngleZ(dofz->getValue());

    localMatrix = glm::translate(offset) * angleZ * angleY * angleX;

    worldMatrix = parentMatrix * localMatrix;

    for (Joint *child : children)
    {
        child->update(worldMatrix);
    }
}

void Joint::draw(const glm::mat4 &viewProjMatrix, GLuint shader)
{
    assert(cube != nullptr);
    cube->draw(viewProjMatrix * worldMatrix, shader);

    for (Joint *child : children)
    {
        child->draw(viewProjMatrix, shader);
    }
}

void Joint::addChild(Joint *child)
{
    if (child)
    {
        children.push_back(child);
        child->setParent(this);
    }
    else
    {
        assert(false);
    }
}

void Joint::setParent(Joint *parent)
{
    this->parent = parent;
}

void Joint::setRotation(glm::vec3 rot)
{
    dofx->setValue(rot.x);
    dofy->setValue(rot.y);
    dofz->setValue(rot.z);
}