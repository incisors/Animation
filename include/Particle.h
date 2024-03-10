#pragma once

#include "core.h"

#define EPSILON 1e-6
#define MAX_FORCE 1e9

class Particle
{
private:
	glm::vec3* position;
	glm::vec3* normal;
	glm::vec3 velocity;
	glm::vec3 force;
	glm::vec3 prevForce;
	bool isFixed = false;
	/*int numOfTriangles = 0;*/

	const float* mass;
	const float* gravityAcce;
	const float* groundPos;

public:
	Particle(glm::vec3* _position, glm::vec3* normal, float* mass, float* gravityAcce, float* groundPos);

	/*static void SetMass(float _mass);
	static void SetGravityAcce(float _g);*/

	/*void AddTriangleCount();*/

	void ApplyForce(glm::vec3 f);
	void ApplyGravity();
	void Integrate(float deltaTime);

	void ResetForce();

	glm::vec3 GetVelocity();
	glm::vec3 GetPosition();

	void ResetNormal();
	void AddNormal(glm::vec3 n);

	void GroundCollision();

	void SetPosition(glm::vec3 pos) { *position = pos; }
	void SetFixed(bool _isFixed) { isFixed = _isFixed; }
};