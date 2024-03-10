#pragma once

#ifndef SKIN_H
#define SKIN_H

#include "core.h"
#include "Skeleton.h"
#include "Tokenizer.h"
#include <vector>
#include <string>
#include <cstring>

class Skin {
private:
	std::string fileName;


	GLuint shaderID;
	GLuint vao, vbos[2], ebo;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;

	std::vector<glm::vec3> currPositions;
	std::vector<glm::vec3> currNormals;

	std::vector<glm::ivec3> triangles;

	std::vector<std::vector<std::pair<int, float>>> skinweights;
	//<index, weight> for joint

	std::vector<glm::mat4> bindings;
	std::vector<glm::mat4> invBindings;

	Skeleton* skel;

public:
	Skin(std::string _fileName, Skeleton* _skel, GLint _programID);
	~Skin();

	void load();
	void ckeckNormals();
	void setSkeleton(Skeleton* _skel) { skel = _skel; }
	void update();
	void draw(const glm::mat4& viewProjMtx, GLuint shader);
};


#endif // SKIN_H