#pragma once
#include "GeneralParticle.h"
#include "Ground.h"

#include "shier.h"

#include <vector>
#include <ctime>
#include <random>

class ParticleSystem
{
public:
    std::vector<GeneralParticle*> particles;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> colors;

    GLuint programID;
    GLuint vao, vbos[2];

    static const int maxParticles = 10000;

    float radius;
    float mass;
    float g;
    float airDensity;
    float dragConst;
    glm::vec3 windSpeed;
    float elasticity;
    float friction;

    glm::vec3 groundTopLeft;
    glm::vec3 groundBottomRight;
    glm::vec3 groundCenter;
    float groundSize;
    Ground* ground;

    float creationRate;
    float creationInterval = 0.0f;

    glm::vec3 initialPos;
    glm::vec3 initialPosVar;
    glm::vec3 initialVelocity;
    glm::vec3 initialVelocityVar;
    float initialLifeSpan;
    float initialLifeSpanVar;

    clock_t prevT;

    int lastIdx = 0;
    int currParticleNum = 0;

    glm::mat4 model;

    std::vector<glm::vec3> sphereVertices;
    std::vector<unsigned int> sphereIndices;
    GLuint sphereVAO, sphereVBO, sphereEBO;

    std::default_random_engine randEng;
    std::normal_distribution<float> randNormal;
        GLuint spriteTexture; // 纹理ID
    int overSample = 2;


    ParticleSystem(GLuint ID, GLuint groundID);
    ~ParticleSystem();
    void Update();
    void Draw(const glm::mat4& viewProjMtx, GLuint shader);

    int FindIdleParticle();

	float GetCreationRate() { return creationRate; }
	void SetCreationRate(float c) { creationRate = c; }
	float GetRadius() { return radius; }
	void SetRadius(float r) 
	{ 
		radius = r;
		model = glm::scale(glm::vec3(r, r, r));
	}
	float GetMass() { return mass; }
	void SetMass(float m) { mass = m; }
	float GetGravity() { return g; }
	void SetGravity(float _g) { g = _g; }
	float GetAirDensity() { return airDensity; }
	void SetAirDensity(float rho) { airDensity = rho; }
	float GetDragConst() { return dragConst; }
	void SetDragConst(float c_d) { dragConst = c_d; }
	glm::vec3 GetWindSpeed() { return windSpeed; }
	void SetWindSpeed(glm::vec3 v) { windSpeed = v; }
	float GetElasticity() { return elasticity; }
	void SetElasticity(float e) { elasticity = e; }
	float GetFriction() { return friction; }
	void SetFriction(float f) { friction = f; }
	glm::vec3 GetGroundCenter() { return groundCenter; }
	void SetGroundCenter(glm::vec3 center) 
	{
		if (center.y > initialPos.y - radius)
		{
			center.y = initialPos.y - radius;
		}
		groundCenter = center;
		groundTopLeft = groundCenter - glm::vec3(groundSize / 2.0f, 0.0f, groundSize / 2.0f);
		groundBottomRight = groundCenter + glm::vec3(groundSize / 2.0f, 0.0f, groundSize / 2.0f);
		ground->SetGroundCenter(groundCenter);
	}
	float GetGroundSize() { return groundSize; }
	void SetGroundSize(float size)
	{
		groundSize = size;
		groundTopLeft = groundCenter - glm::vec3(groundSize / 2.0f, 0.0f, groundSize / 2.0f);
		groundBottomRight = groundCenter + glm::vec3(groundSize / 2.0f, 0.0f, groundSize / 2.0f);
		ground->SetGroundSize(size);
	}
	glm::vec3 GetInitialPos() { return initialPos; }
	void SetInitialPos(glm::vec3 pos) 
	{
		if (pos.y < groundCenter.y + radius)
		{
			pos.y = groundCenter.y + radius;
		}
		initialPos = pos; 
	}
	glm::vec3 GetInitialPosVar() { return initialPosVar; }
	void SetInitialPosVar(glm::vec3 var) { initialPosVar = var; }
	glm::vec3 GetInitialVelocity() { return initialVelocity; }
	void SetInitialVelocity(glm::vec3 v) { initialVelocity = v; }
	glm::vec3 GetInitialVelocityVar() { return initialVelocityVar; }
	void SetInitialVelocityVar(glm::vec3 var) { initialVelocityVar = var; }
	float GetInitialLifeSpan() { return initialLifeSpan; }
	void SetInitialLifeSpan(float t) { initialLifeSpan = t; }
	float GetInitialLifeSpanVar() { return initialLifeSpanVar; }
	void SetInitialLifeSpanVar(float var) { initialLifeSpanVar = var; }
    void LoadSphere();
    void LoadSpriteTexture( ); // 加载纹理的方法
        void RenderSphere();

};