
#include "GeneralParticle.h"

const float EPSILON = 1e-5;

GeneralParticle::GeneralParticle(float* _radius, float* _mass, float* _g,
	float* _airDensity, float* _dragConst, glm::vec3* _windSpeed,
	float* _elasticity, float* _friction, 
	glm::vec3* _groundTopleft, glm::vec3* _groundBottomRight)
{
	radius = _radius;
	mass = _mass;
	g = _g;
	airDensity = _airDensity;
	dragConst = _dragConst;
	elasticity = _elasticity;
	friction = _friction;
	windSpeed = _windSpeed;
	groundTopleft = _groundTopleft;
	groundBottomRight = _groundBottomRight;

	lifeSpan = 0.0f;
	isActive = false;
	position = glm::vec3(0.0f);
	velocity = glm::vec3(0.0f);
	color = glm::vec3(0.0f);
}

void GeneralParticle::Update(float deltaT)
{
	lifeSpan -= deltaT;
	if (lifeSpan < 0.0f)
	{
		lifeSpan = 0.0f;
		isActive = false;
		return;
	}

	glm::vec3 force(0.0f, -(*g) * (*mass), 0.0f);
	glm::vec3 v_close = velocity - (*windSpeed);
	force -= 0.5f * (*airDensity) * glm::length(v_close) * v_close * (*dragConst) 
		* glm::pi<float>() * (*radius) * (*radius);

	glm::vec3 a = force / (*mass);
	velocity += a * deltaT;
	position += velocity * deltaT;

	GroundCollision();
}
void GeneralParticle::Render(const glm::mat4& viewProjMtx, GLuint shader)
{
    if (!isActive) return;

    glUseProgram(shader);

    glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, GL_FALSE, (float*)&viewProjMtx);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(glm::translate(position)));
    glUniform3fv(glGetUniformLocation(shader, "color"), 1, glm::value_ptr(color));

    // 绑定VAO并更新VBO数据

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	GLenum err;

    glBufferData(GL_ARRAY_BUFFER, sizeof(position), glm::value_ptr(position), GL_STATIC_DRAW);
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error at render back: " << err << std::endl;
        exit(-1);
	}
    // 绘制点
	glPointSize(10.0f);  // 设置点的大小，根据需要调整
    glDrawArrays(GL_POINTS, 0, 1);

    // 解绑VAO
    glBindVertexArray(0);
    glUseProgram(0);

}

void GeneralParticle::GroundCollision()
{
	float groundPos = groundTopleft->y + (*radius);
	if (position.x >= groundTopleft->x && position.x <= groundBottomRight->x
		&& position.z >= groundTopleft->z && position.z <= groundBottomRight->z
		&& position.y <= groundPos)
	{
		float v_y = velocity.y;
		if (v_y < EPSILON)
		{
			position.y = groundPos + EPSILON;
		}
		else
		{
			position.y = 2.0f * groundPos - position.y + EPSILON;
		}
		velocity.y = 0.0f;
		float v_tan = glm::length(velocity);
		if (v_tan > EPSILON)
		{
			velocity /= v_tan;
			float j = abs((1 + (*elasticity)) * v_y);
			v_tan -= (*friction) * j;
			if (v_tan < 0.0f) v_tan = 0.0f;
			velocity *= v_tan;
			velocity.y = abs(v_y) * (*elasticity);
		}
		else
		{
			velocity.y = abs(v_y) * (*elasticity);
		}
	}
}