#include "DOF.h"
#include <algorithm>
DOF::DOF()
{
    minLimit = -3.4E+38;
    maxLimit = 3.4E+37;
}

DOF::DOF(float value, float minLimit, float maxLimit)
{
    this->value = value;
    this->minLimit = minLimit;
    this->maxLimit = maxLimit;
}

void DOF::setValue(float val)
{
    value = val;
}

float DOF::getValue() const
{
    if (value < minLimit)
    {
        return minLimit;
    }
    else if (value > maxLimit)
    {
        return maxLimit;
    }
    else
    {
        return value;
    }
}

void DOF::setMin(float min)
{
    minLimit = min;
}

void DOF::setMax(float max)
{
    maxLimit = max;
}
