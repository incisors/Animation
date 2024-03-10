#include "Triangle.h"

//float Triangle::FluidDensity;
//float Triangle::C_d;
//glm::vec3 Triangle::WindVelocity;

Triangle::Triangle(Particle* _P1, Particle* _P2, Particle* _P3, float* _FluidDensity, float* _C, glm::vec3* _WindVelocity)
{
	P1 = _P1;
	P2 = _P2;
	P3 = _P3;

	/*P1->AddTriangleCount();
	P2->AddTriangleCount();
	P3->AddTriangleCount();*/

	FluidDensity = _FluidDensity;
	C_d = _C;
	WindVelocity = _WindVelocity;
}

//void Triangle::SetFluidDensity(float rho)
//{
//	FluidDensity = rho;
//}
//
//void Triangle::SetDragConst(float C)
//{
//	C_d = C;
//}
//
//void Triangle::SetWindVelocity(glm::vec3 v)
//{
//	WindVelocity = v;
//}

void Triangle::ComputeAerodynamicForce()
{
	glm::vec3 SurfaceVelocity = (
		P1->GetVelocity() 
		+ P2->GetVelocity() 
		+ P2->GetVelocity()) / 3.0f;

	glm::vec3 v_dir = SurfaceVelocity - *WindVelocity;
	float v_scale = glm::length(v_dir);
	if (v_scale < EPSILON) return;

	v_dir /= v_scale;

	glm::vec3 n = glm::cross(
		P2->GetPosition() - P1->GetPosition(), 
		P3->GetPosition() - P1->GetPosition());
	float area = glm::length(n);
	n /= area;
	area /= 2;

	float crossArea = area * glm::dot(v_dir, n);

	glm::vec3 f = -0.5f * (*FluidDensity) * v_scale * v_scale * (*C_d) * crossArea * n;

	f /= 3.0f;
	P1->ApplyForce(f);
	P2->ApplyForce(f);
	P3->ApplyForce(f);

	/*P1->AddNormal(n);
	P2->AddNormal(n);
	P3->AddNormal(n);*/
}

void Triangle::ComputeNormal()
{
	glm::vec3 n = glm::cross(
		P2->GetPosition() - P1->GetPosition(),
		P3->GetPosition() - P1->GetPosition());

	if (n.length() < EPSILON) return;

	n = glm::normalize(n);

	P1->AddNormal(n);
	P2->AddNormal(n);
	P3->AddNormal(n);
}