#pragma once
#include "core.h"

class GeneralParticle
{
public:
	const float* radius;
	const float* mass;
	const float* g;
	const float* airDensity;
	const float* dragConst;
	const float* elasticity;
	const float* friction;
	const glm::vec3* groundTopleft;
	const glm::vec3* groundBottomRight;

	const glm::vec3* windSpeed;

	float lifeSpan;
	bool isActive = false;

	GLuint vao, vbo;

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 color;

    GeneralParticle(float* _radius, float* _mass, float* _g,
        float* _airDensity, float* _dragConst, glm::vec3* _windSpeed,
        float* _elasticity, float* _friction, 
        glm::vec3* _groundTopleft, glm::vec3* _groundBottomRight);
	//~GeneralParticle() {};

	void Update(float deltaT);
	void GroundCollision();
	void Render(const glm::mat4& viewProjMtx, GLuint shader);
	
	bool GetStatus() { return isActive; }
	void GetPosition(glm::vec3& pos) { pos = position; }
	void GetColor(glm::vec3& c) { c = color; }

	void SetPosition(glm::vec3 pos) { position = pos; }
	void SetVelocity(glm::vec3 v) { velocity = v; }
	void SetColor(glm::vec3 c) { color = c; }
	void SetLifeSpan(float t) { lifeSpan = t; }
	float GetLifeSpan() { return lifeSpan; }
	void SetActive() { isActive = true; }
	void SetInactive() { isActive = false; }

	void SetRadius(float r) {
		radius = &r;
	}
};