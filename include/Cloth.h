#pragma once
#include "core.h"
#include "Particle.h"
#include "SpringDamper.h"
#include "Triangle.h"
#include "Ground.h"

#include <ctime>

class Cloth
{
private:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::ivec3> indices;

	std::vector<Particle*> particles;
	std::vector<SpringDamper*> connections;
	std::vector<Triangle*> triangles;

	std::vector<int> fixedParticleIdx;

	glm::vec3 topLeftPos;
	glm::vec3 horiDir;
	glm::vec3 vertDir;

	float size, mass;
	int numOfParticles;

	int numOfOversamples;

	//float gridLength;

	//Ad force
	float FluidDensity;
	float C_d;
	glm::vec3 WindVelocity;

	//spring damper
	float springConst, dampingConst;
	float restLength;
	float restLengthDiag;

	//particle
	float particleMass;
    float gravityAcce;
	float groundPos;

	GLuint programID;
	GLuint vao, vbos[2], ebo;

	clock_t prevT;
	clock_t interval = 0;
	int FPSCount = 0;
	int FPS = 0;

	Ground* ground;

public:
	Cloth(float _size, float _mass, int N, glm::vec3 pos, glm::vec3 hori, glm::vec3 vert, GLuint ID);
	~Cloth();

	void Initialize();

	void Update();
	void Draw(const glm::mat4& viewProjMtx, GLuint ProgramID);

	void SetMass(float m) 
	{ 
		mass = m; 
		particleMass = mass / (numOfParticles * numOfParticles);
	}
	float GetMass() { return mass; }

	void SetFluidDensity(float rho) { FluidDensity = rho; }
	float GetFluidDensity() { return FluidDensity; }
	void SetDragConst(float C) { C_d = C; }
	float GetDragConst() { return C_d; }
	void SetWindVelocity(glm::vec3 v) { WindVelocity = v; }
	glm::vec3 GetWindVelocity() { return WindVelocity; }
	void SetSpringConst(float k) { springConst = k; }
	float GetSpringConst() { return springConst; }
	void SetDampingConst(float k) { dampingConst = k; }
	float GetDampingConst() { return dampingConst; }
	void SetRestLength(float l) 
	{ 
		restLength = l;
		restLengthDiag = l * 1.4142136f;
	}
	float GetRestLength() { return restLength; }
	void SetGravityAcce(float g) { gravityAcce = g; }
	float GetGravityAcce() { return gravityAcce; }
	void SetGroundPos(float h) 
	{ 
		groundPos = h;
		ground->SetGroundLevel(h);
	}
	float GetGroundPos() { return groundPos; }

	glm::vec3 GetFixedParticlePos(int i) { return positions[fixedParticleIdx[i]]; }
	void SetFixedParticlePos(int i, glm::vec3 pos) 
	{
		if (pos.y < groundPos) pos.y = 2 * groundPos - pos.y;
		positions[fixedParticleIdx[i]] = pos; 
	}
	int GetFixedParticleNum() { return fixedParticleIdx.size(); }

	int GetFPS() { return FPS; }

	void TranslateFixedParticles(int axis, float shift);
	void RotateFixedParticles(int axis, float shift);
};