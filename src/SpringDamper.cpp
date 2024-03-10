#include "springdamper.h"


//float SpringDamper::SpringConst = 1.0f; 
//float SpringDamper::DampingConst = 1.0f;
//float SpringDamper::RestLength = 0.01f;

SpringDamper::SpringDamper(Particle* _P1, Particle* _P2, float* _SpringConst, float* _DampingConst, float* _RestLength)
{
	P1 = _P1;
	P2 = _P2;

	SpringConst = _SpringConst;
	DampingConst = _DampingConst;
	RestLength = _RestLength;

	prevDir = P2->GetPosition() - P1->GetPosition();
}

//void SpringDamper::SetSpringConst(float k)
//{
//	SpringConst = k;
//}
//
//void SpringDamper::SetDampingConst(float k)
//{
//	DampingConst = k;
//}

void SpringDamper::ComputeForce()
{
	glm::vec3 dir = P2->GetPosition() - P1->GetPosition();
	float currLength = glm::length(dir);
	if (currLength > EPSILON)
	{
		dir = glm::normalize(dir);
		prevDir = dir;
	}
	else
	{
		//dir = prevDir;
		//currLength = 0.0f;
		dir = glm::vec3(0.0f);
		currLength = 0.0f;
	}

	glm::vec3 f_spring = -(*SpringConst) * ((*RestLength) - currLength) * dir;
	if (f_spring.length() < EPSILON) f_spring = glm::vec3(0.0f);

	/*if (std::isnan(f_spring.x))
	{
		std::cout << "SpringDamper: spring nan force" << std::endl;
	}*/

	float v = glm::dot(
		P1->GetVelocity() - P2->GetVelocity(),
		dir);

	glm::vec3 f_damper = -(*DampingConst) * v * dir;
	if (f_damper.length() < EPSILON) f_damper = glm::vec3(0.0f);

	/*if (std::isnan(f_damper.x))
	{
		std::cout << "SpringDamper: damper nan force" << std::endl;
	}*/

	P1->ApplyForce(f_spring + f_damper);
	P2->ApplyForce(-f_spring - f_damper);
}