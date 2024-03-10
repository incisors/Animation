#include "Skin.h"

Skin::Skin(std::string _fileName, Skeleton *_skel, GLint _programID)
{
	fileName = _fileName;
	skel = _skel;
	shaderID = _programID;

	load();
	ckeckNormals();

	for (size_t i = 0; i < bindings.size(); i++)
	{
		invBindings[i] = glm::inverse(bindings[i]);
	}

	// Generate a vertex array (VAO), vertex buffer objects (VBO) and EBO.
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbos);
	glGenBuffers(1, &ebo);

	// VAO.
	glBindVertexArray(vao);

	// positions VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * currPositions.size(), nullptr, GL_STREAM_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// normal VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * currNormals.size(), nullptr, GL_STREAM_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// vertex index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * triangles.size(), triangles.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

Skin::~Skin()
{
	glDeleteBuffers(2, vbos);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void Skin::load()
{
	Tokenizer fileToken;
	fileToken.Open(fileName.c_str());

	char tokenBuff[20];
	fileToken.GetToken(tokenBuff);

	while (tokenBuff[0] != '\0')
	{
		if (strcmp(tokenBuff, "positions") == 0)
		{
			int count = fileToken.GetInt();
			fileToken.GetToken(tokenBuff);
			while (count > 0)
			{
				glm::vec3 pos;
				pos.x = fileToken.GetFloat();
				pos.y = fileToken.GetFloat();
				pos.z = fileToken.GetFloat();
				positions.push_back(pos);
				count--;
			}
			fileToken.GetToken(tokenBuff);
		}
		else if (strcmp(tokenBuff, "normals") == 0)
		{
			int count = fileToken.GetInt();
			fileToken.GetToken(tokenBuff);
			while (count > 0)
			{
				glm::vec3 norm;
				norm.x = fileToken.GetFloat();
				norm.y = fileToken.GetFloat();
				norm.z = fileToken.GetFloat();
				normals.push_back(glm::normalize(norm));
				count--;
			}
			fileToken.GetToken(tokenBuff);
		}
		else if (strcmp(tokenBuff, "triangles") == 0)
		{
			int count = fileToken.GetInt();
			fileToken.GetToken(tokenBuff);
			while (count > 0)
			{
				glm::vec3 tri;
				tri.x = fileToken.GetInt();
				tri.y = fileToken.GetInt();
				tri.z = fileToken.GetInt();
				triangles.push_back(tri);
				count--;
			}
			fileToken.GetToken(tokenBuff);
		}
		else if (strcmp(tokenBuff, "skinweights") == 0)
		{
			int count = fileToken.GetInt();
			fileToken.GetToken(tokenBuff);
			while (count > 0)
			{
				skinweights.push_back(std::vector<std::pair<int, float>>());
				for (int bindsCount = fileToken.GetInt(); bindsCount > 0; bindsCount--)
				{
					int jointIdx = fileToken.GetInt();
					float weight = fileToken.GetFloat();
					skinweights.back().push_back({jointIdx, weight});
				}
				count--;
			}
			fileToken.GetToken(tokenBuff);
		}
		else if (strcmp(tokenBuff, "bindings") == 0)
		{
			int count = fileToken.GetInt();
			fileToken.GetToken(tokenBuff);
			while (count > 0)
			{
				fileToken.GetToken(tokenBuff);
				if (strcmp(tokenBuff, "matrix") == 0)
				{
					bindings.push_back(glm::mat4(1.0));
					fileToken.GetToken(tokenBuff);
					for (int i = 0; i < 4; i++)
					{
						bindings.back()[i].x = fileToken.GetFloat();
						bindings.back()[i].y = fileToken.GetFloat();
						bindings.back()[i].z = fileToken.GetFloat();
					}
					fileToken.GetToken(tokenBuff);
				}
				count--;
			}
			fileToken.GetToken(tokenBuff);
		}
		fileToken.GetToken(tokenBuff);
	}

	fileToken.Close();

	currPositions.assign(positions.begin(), positions.end());
	currNormals.assign(normals.begin(), normals.end());
	invBindings.assign(bindings.begin(), bindings.end());
}

void Skin::ckeckNormals()
{
	for (glm::ivec3 &triIdx : triangles)
	{
		glm::vec3 triNorm = glm::cross(
			positions[triIdx.y] - positions[triIdx.x],
			positions[triIdx.z] - positions[triIdx.y]);
		triNorm = glm::normalize(triNorm);

		if (glm::dot(triNorm, normals[triIdx.x]) < 0)
		{
			normals[triIdx.x] *= -1;
		}

		if (glm::dot(triNorm, normals[triIdx.y]) < 0)
		{
			normals[triIdx.y] *= -1;
		}

		if (glm::dot(triNorm, normals[triIdx.z]) < 0)
		{
			normals[triIdx.z] *= -1;
		}
	}
}

void Skin::update()
{
	// if (skel == nullptr)
	// 	return;

	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	for (size_t i = 0; i < positions.size(); i++)
	{
		glm::vec3 newPos(0.0f);
		for (auto &weight : skinweights[i])
		{
			if (skel != nullptr)
			{
				newPos += glm::vec3(weight.second * skel->getWorldMatrix(weight.first) * invBindings[weight.first] * glm::vec4(positions[i], 1.0f));
			}
			else
			{
				newPos += glm::vec3(weight.second * glm::mat4(1.0f) * glm::vec4(positions[i], 1.0f));
			}
		}
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * i, sizeof(glm::vec3), &newPos);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	for (size_t i = 0; i < normals.size(); i++)
	{
		glm::vec3 newNorm(0.0f);
		for (auto &weight : skinweights[i])
		{
			if (skel != nullptr)
			{
				newNorm += glm::vec3(weight.second * glm::transpose(glm::inverse(skel->getWorldMatrix(weight.first))) * glm::transpose(bindings[weight.first]) * glm::vec4(normals[i], 0.0f));
			}
			else
			{
				newNorm += glm::vec3(weight.second * glm::transpose(glm::inverse(glm::mat4(1.0f))) * glm::vec4(normals[i], 0.0f));
			}
		}
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * i, sizeof(glm::vec3), &newNorm);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Skin::draw(const glm::mat4 &viewProjMtx, GLuint shader)
{
	glUseProgram(shader);

	glm::mat4 model(1.0);
	glm::vec3 color(1.0f, 0.95f, 1.0f);

	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float *)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float *)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, triangles.size() * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}