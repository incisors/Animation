#include "Cloth.h"
#include <iostream>

#define euler_hdr "glm/gtx/euler_angles.hpp"
#include euler_hdr


Cloth::Cloth(float _size, float _mass, int N, glm::vec3 pos, glm::vec3 hori, glm::vec3 vert, GLuint ID)
{
	size = _size;
	mass = _mass;
	numOfParticles = N;
	topLeftPos = pos;
	horiDir = glm::normalize(hori);
	vertDir = glm::normalize(vert);

	programID = ID;

	Initialize();

	prevT = clock();
}

Cloth::~Cloth()
{
	for (auto it : particles)
	{
		delete it;
	}

	for (auto it : connections)
	{
		delete it;
	}

	for (auto it : triangles)
	{
		delete it;
	}

	// Delete the VBOs and the VAO.
	glDeleteBuffers(2, vbos);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void Cloth::Initialize()
{
	float gridLength = size / (numOfParticles - 1);

	positions.resize(numOfParticles * numOfParticles);
	normals.resize(numOfParticles * numOfParticles);
	//indices.resize((numOfParticles - 1) * (numOfParticles - 1) * 2);

	//const
	//Ad force
	FluidDensity = 1.225;
	C_d = 1.0f;
	WindVelocity = glm::vec3(0.5f, 0.5f, -2.5f);
	//spring damper
	springConst = 1000.0f;
	dampingConst = 3.5f; // 12.5f;
	restLength = gridLength;
	restLengthDiag = gridLength * 1.4142f;
	//particle
	particleMass = mass / (numOfParticles * numOfParticles);
	gravityAcce = 2.0f;
	groundPos = 0.0f;

	if (groundPos > topLeftPos.y) groundPos = topLeftPos.y - EPSILON;

	ground = new Ground(
		glm::vec3(-5.0f, groundPos, -5.0f), 
		10.0f, 
		programID);

	numOfOversamples = 20;

	glm::vec3 planeDir = glm::cross(horiDir, vertDir);
	planeDir.y = 0.0f;
	planeDir = glm::normalize(planeDir);

	//Particles
	glm::vec3 norm = glm::cross(vertDir, horiDir);
	norm = glm::normalize(norm);
	for (int y = 0; y < numOfParticles; y++)
	{
		for (int x = 0; x < numOfParticles; x++)
		{
			int i = x + y * numOfParticles;
			positions[i] = topLeftPos + x * gridLength * horiDir + y * gridLength * vertDir;
			if (positions[i].y < groundPos)
			{
				int j = i - numOfParticles;
				positions[i] = positions[j] + gridLength * planeDir;
				normals[i] = glm::vec3(0.0f, -1.0f, 0.0f);
			}
			else
			{
				normals[i] = norm;
			}
			Particle* P = new Particle(&positions[i], &normals[i], &particleMass, &gravityAcce, &groundPos);
			particles.push_back(P);
		}
	}

	//Triangles
	for (int y = 0; y < numOfParticles - 1; y++)
	{
		for (int x = 0; x < numOfParticles - 1; x++)
		{
			int topLeftIdx = x + y * numOfParticles;
			int topRightIdx = topLeftIdx + 1;
			int bottomLeftIdx = topLeftIdx + numOfParticles;
			int bottomRightIdx = bottomLeftIdx + 1;

			indices.push_back(glm::ivec3(topLeftIdx, bottomLeftIdx, bottomRightIdx));
			indices.push_back(glm::ivec3(topLeftIdx, bottomRightIdx, topRightIdx));

			Triangle* tri;
			tri = new Triangle(
				particles[topLeftIdx], particles[bottomLeftIdx], particles[bottomRightIdx], 
				&FluidDensity, &C_d, &WindVelocity);
			triangles.push_back(tri);
			tri = new Triangle(
				particles[topLeftIdx], particles[bottomRightIdx], particles[topRightIdx], 
				&FluidDensity, &C_d, &WindVelocity);
			triangles.push_back(tri);
		}
	}

	//Dampstring
	for (int y = 0; y < numOfParticles - 1; y++)
	{
		for (int x = 0; x < numOfParticles - 1; x++)
		{
			int topLeftIdx = x + y * numOfParticles;
			int topRightIdx = topLeftIdx + 1;
			int bottomLeftIdx = topLeftIdx + numOfParticles;
			int bottomRightIdx = bottomLeftIdx + 1;

			SpringDamper* sp;
			sp = new SpringDamper(
				particles[topLeftIdx], particles[topRightIdx], 
				&springConst, &dampingConst, &restLength);
			connections.push_back(sp);
			sp = new SpringDamper(
				particles[topLeftIdx], particles[bottomRightIdx],
				&springConst, &dampingConst, &restLengthDiag);
			connections.push_back(sp);
			sp = new SpringDamper(
				particles[topLeftIdx], particles[bottomLeftIdx],
				&springConst, &dampingConst, &restLength);
			connections.push_back(sp);
			sp = new SpringDamper(
				particles[bottomLeftIdx], particles[topRightIdx],
				&springConst, &dampingConst, &restLengthDiag);
			connections.push_back(sp);

			if (x == numOfParticles - 2)
			{
				sp = new SpringDamper(
					particles[topRightIdx], particles[bottomRightIdx],
					&springConst, &dampingConst, &restLength);
				connections.push_back(sp);
			}

			if (y == numOfParticles - 2)
			{
				sp = new SpringDamper(
					particles[bottomLeftIdx], particles[bottomRightIdx],
					&springConst, &dampingConst, &restLength);
				connections.push_back(sp);
			}
		}
	}

	//Fixed points
	for (int i = 0; i < numOfParticles; i++)
	{
		fixedParticleIdx.push_back(i);
		particles[i]->SetFixed(true);
	}
	

	// Generate a vertex array (VAO), vertex buffer objects (VBO) and EBO.
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbos);
	glGenBuffers(1, &ebo);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// 1st VBO, positions
	//a. Bind buffer to VAO
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	//b. Pass in the position data.
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(glm::vec3) * positions.size(),
		//currPositions.data(), 
		nullptr,
		GL_STREAM_DRAW);
	// c. Enable vertex attribute 0. Creat channel
	// We will be able to access vertices through it.
	glEnableVertexAttribArray(0);
	//d. How to read VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//2nd VBO, normals
	//a. Bind buffer to VAO
	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	//b. Pass in the data.
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(glm::vec3) * normals.size(),
		//currNormals.data(), 
		nullptr,
		GL_STREAM_DRAW);
	// c. Enable vertex attribute 0. Creat channel
	// We will be able to access vertices through it.
	glEnableVertexAttribArray(1);
	//d. How to read VBO
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//vertex index
	//a. Bind to the EBO. We will use it to store the indices.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//b. Pass in the data.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(glm::ivec3) * indices.size(),
		indices.data(),
		GL_STATIC_DRAW);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
	// 在 Cloth::Initialize 函数的末尾添加：
	std::cout << "First few triangle indices:" << std::endl;
	std::cout << "number of vertices: " << positions.size() << std::endl;
	std::cout << "Number of triangles: " << indices.size() << std::endl;
	for (int i = 0; i < std::min(static_cast<int>(indices.size()), 5); ++i) {
		std::cout << "Triangle " << i << ": (" 
				<< indices[i].x << ", " 
				<< indices[i].y << ", " 
				<< indices[i].z << ")" << std::endl;
	}

}

void Cloth::Update()
{
clock_t currT = clock();
	FPSCount++;
	interval += currT - prevT;
	if (interval >= 1000)
	{
		FPS = FPSCount;
		interval = 0;
		FPSCount = 0;
	}
	float deltaT = (float)(currT - prevT) / 1000.0f;
	prevT = currT;

	deltaT /= (float)numOfOversamples;
	for (int i = 0; i < numOfOversamples; i++)
	{
		for (auto sp : connections)
		{
			sp->ComputeForce();
		}

		for (auto tri : triangles)
		{
			tri->ComputeAerodynamicForce();
		}

		for (auto P : particles)
		{
			P->Integrate(deltaT);
		}
	}

	for (auto& n : normals)
	{
		n = glm::vec3(0.0f);
	}

	for (auto tri : triangles)
	{
		tri->ComputeNormal();
	}

	for (auto& n : normals)
	{
		n = glm::normalize(n);
	}

	//transfer to vbo
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(ptr, positions.data(), sizeof(glm::vec3) * positions.size());
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(ptr, normals.data(), sizeof(glm::vec3) * normals.size());
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Cloth::Draw(const glm::mat4& viewProjMtx, GLuint programID)
{
    glUseProgram(programID);

    glm::mat4 model(1.0);
    glm::vec3 color(1.0f, 0.0f, 1.0f);
	
    // Set the uniforms
    glUniformMatrix4fv(glGetUniformLocation(programID, "viewProj"), 1, GL_FALSE, (float*)&viewProjMtx);
    glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, (float*)&model);
    glUniform3fv(glGetUniformLocation(programID, "DiffuseColor"), 1, &color[0]);

    // Bind to the VAO
    glBindVertexArray(vao);

    // Draw points
    glPointSize(5.0f); // Set point size
    glDrawArrays(GL_POINTS, 0, positions.size()); // Draw points

    // Draw triangles
    glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0); // mode, count, type, indices

    // Unbind from the VAO
    glBindVertexArray(0);

    // Unbind from the shader program
    glUseProgram(0);

    // Draw the ground
    ground->Draw(viewProjMtx, programID);
	
}


void Cloth::TranslateFixedParticles(int axis, float shift)
{
	glm::vec3 delta(0.0f);
	delta[axis] = shift;

	for (int i : fixedParticleIdx)
	{
		positions[i] += delta;
	}

	/*for (auto& pos : positions)
	{
		pos += delta;
	}*/
}

void Cloth::RotateFixedParticles(int axis, float shift)
{
	glm::mat4 rotateMat(1.0f);

	if (axis == 0)
	{
		rotateMat = glm::eulerAngleX(shift);
	}
	else if (axis == 1)
	{
		rotateMat = glm::eulerAngleY(shift);
	}
	else if (axis == 2)
	{
		rotateMat = glm::eulerAngleZ(shift);
	}

	glm::vec3 midPoint = 
		(positions[fixedParticleIdx.front()] + positions[fixedParticleIdx.back()]) / 2.0f;

	glm::mat4 TransMat = 
		glm::translate(midPoint) * rotateMat * glm::translate(-midPoint);

	for (int i : fixedParticleIdx)
	{
		positions[i] = TransMat * glm::vec4(positions[i], 1.0f);
	}

	/*for (auto& pos : positions)
	{
		pos = TransMat * glm::vec4(pos, 1.0f);
	}*/

}