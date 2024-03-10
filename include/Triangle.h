#pragma once

#include "Particle.h"

class Triangle
{
public:
	Particle* P1, * P2, * P3;

	const float* FluidDensity;
	const float* C_d;
	const glm::vec3* WindVelocity;

	Triangle(Particle* _P1, Particle* _P2, Particle* _P3, float* _FluidDensity, float* _C, glm::vec3* _WindVelocity);

	/*static void SetFluidDensity(float rho);
	static void SetDragConst(float C);
	static void SetWindVelocity(glm::vec3 v);*/

	void ComputeAerodynamicForce();
	void ComputeNormal();

};