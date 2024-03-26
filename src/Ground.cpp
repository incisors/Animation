#include "Ground.h"

#define EPSILON 1e-2f

Ground::Ground(glm::vec3 center, float size, GLuint _programID)
{
	programID = _programID;
	
	transMat = glm::translate(center);
	scaleMat = glm::scale(glm::vec3(size, size, size));
	model = transMat * scaleMat;

	points.push_back(glm::vec3(-0.5f, 0.0f, -0.5f));
	points.push_back(glm::vec3(-0.5f, 0.0f, 0.5f));
	points.push_back(glm::vec3(0.5f, 0.0f, 0.5f));
	points.push_back(glm::vec3(0.5f, 0.0f, -0.5f));

	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	indices.push_back(glm::ivec3(0, 1, 2));
	indices.push_back(glm::ivec3(0, 2, 3));


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
		sizeof(glm::vec3) * points.size(),
		points.data(),
		GL_STATIC_DRAW);
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
		normals.data(),
		GL_STATIC_DRAW);
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
}


Ground::~Ground()
{
	// Delete the VBOs and the VAO.
	glDeleteBuffers(2, vbos);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void Ground::SetGroundLevel(float level)
{
	model = glm::translate(glm::vec3(0.0f, level - EPSILON, 0.0f));
}

void Ground::SetGroundCenter(glm::vec3 center)
{
	transMat = glm::translate(center);
	model = transMat * scaleMat;
}

void Ground::SetGroundSize(float _size)
{
	scaleMat = glm::scale(glm::vec3(_size, _size, _size));
	model = transMat * scaleMat;
}

void Ground::Draw(const glm::mat4& viewProjMtx)
{
	glUseProgram(programID);

	glm::vec3 color(0.8f, 0.2f, 0.1f);

	// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(programID, "viewProj"), 1, GL_FALSE, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(programID, "DiffuseColor"), 1, &color[0]);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0); // mode, count, type, indices

	// Unbind from the VAO and shader program.
	glBindVertexArray(0);
	glUseProgram(0);
}