#pragma once

#include "Particle.h"

class SpringDamper
{
private:
	/*static float SpringConst, DampingConst;
	static float RestLength;*/
	const float *SpringConst, *DampingConst;
	const float *RestLength;
	Particle *P1, *P2;

	glm::vec3 prevDir;

public:
	SpringDamper(Particle* _P1, Particle* _P2, float* SpringConst, float* DampingConst, float* RestLength);

	/*static void SetSpringConst(float k);
	static void SetDampingConst(float k);*/

	void ComputeForce();

};