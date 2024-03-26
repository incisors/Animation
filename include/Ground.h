#pragma once
#include "core.h"
#include <vector>
class Ground
{
private:
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<glm::ivec3> indices;

	glm::mat4 model;

	glm::mat4 transMat;
	glm::mat4 scaleMat;

	GLuint programID;
	GLuint vao, vbos[2], ebo;

public:
	Ground(glm::vec3 topleft, float size, GLuint programID);
	~Ground();

	void SetGroundLevel(float level);
	void Draw(const glm::mat4& viewProjMtx);

	void SetGroundCenter(glm::vec3 center);
	void SetGroundSize(float _size);
};