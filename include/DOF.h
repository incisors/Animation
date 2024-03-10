#pragma once
#ifndef DOF_H
#define DOF_H

#include "core.h"
#include <iostream>
class DOF {
private:
    float value;
    float minLimit;
    float maxLimit;

public:
    DOF();
    DOF(float value, float minLimit, float maxLimit);
    void setValue(float val);
    float getValue() const;
    void setMin(float min);
    void setMax(float max);
    void inc() { value += glm::radians(3.0f);} 
    void dec() { value -= glm::radians(3.0f);}
};

#endif // DOF_H
