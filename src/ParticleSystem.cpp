#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(GLuint ID, GLuint groundID)
{
	groundSize = 25.0f;
	groundCenter = glm::vec3(0.0f);
	groundTopLeft = groundCenter - glm::vec3(125.0f, 0.0f, 125.0f);
	groundBottomRight = groundCenter + glm::vec3(125.0f, 0.0f, 125.0f);
	ground = new Ground(groundCenter, groundSize, groundID);

	radius = 0.1f;
	mass = 0.1f;
	g = 0.1f;
	airDensity = 5.225f;
	dragConst = 0.1f;
	windSpeed = glm::vec3(0.0f);
	elasticity = 0.8f;
	friction = 0.01f;
	
	creationRate = 0.5;

	model = glm::scale(glm::vec3(radius));

	initialPos = glm::vec3(5.0f, 30.0f, -100.0f);
	initialPosVar = glm::vec3(0.0f, 0.0f, 0.0f);
	initialVelocity = glm::vec3(0.0f, 0.0f, 1.3f);
	initialVelocityVar = glm::vec3(0.0f);
	initialLifeSpan = 200.0f;
	initialLifeSpanVar = 0.0f;

    programID = ID;

    particles.resize(maxParticles);
    positions.resize(maxParticles);
    colors.resize(maxParticles);

    std::cout << "Creating particles..." << std::endl;
    std::cout << "maxParticles: " << maxParticles << std::endl;
    std::cout << particles.size() << std::endl;
    for (auto& p : particles)
    {
        p = new GeneralParticle(&radius, &mass, &g,
            &airDensity, &dragConst, &windSpeed,
            &elasticity, &friction,
            &groundTopLeft, &groundBottomRight);

        if (p == nullptr) {
            std::cerr << "Failed to allocate memory for particle." << std::endl;
            exit(-1); // 或者处理错误的其他方式
        }
    }

    LoadSpriteTexture();
    LoadSphere();
    glUseProgram(programID);

    // Generate a vertex array (VAO) and vertex buffer objects (VBOs).
    glGenVertexArrays(1, &vao);
    glGenBuffers(4, vbos);  // 生成四个缓冲区

    // Bind to the VAO.
    glBindVertexArray(vao);

    // 1st VBO, particle positions
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * maxParticles, nullptr, GL_STREAM_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // 2nd VBO, particle colors
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * maxParticles, nullptr, GL_STREAM_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    // Unbind from the VAO and VBOs.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(0);

    prevT = clock()/10;
    srand((unsigned)prevT);
}

ParticleSystem::~ParticleSystem()
{

}




void ParticleSystem::Update()
{
    clock_t currT = clock()/10;
    float deltaT = (currT - prevT) / 1000.0f;
    prevT = currT;
    creationInterval += deltaT;

    // 计算应该生成的新粒子数量
    int newParticleNum = std::min(static_cast<int>(creationInterval * creationRate), maxParticles - currParticleNum);
    creationInterval -= newParticleNum / creationRate;

    // 更新现有粒子的状态和位置
    currParticleNum = 0;
    deltaT /= (float)overSample;
    for (int i = 0; i < maxParticles; i++)
    {
        if (particles[i]->GetStatus())
        {
            for (int j = 0; j < overSample; j++)
            {
                particles[i]->Update(deltaT);
            }
            particles[i]->GetPosition(positions[currParticleNum]);
            particles[i]->GetColor(colors[currParticleNum]);
            particles[i]->SetRadius(radius);
            currParticleNum++;
        }
    }
   for (int i = 0; i < newParticleNum; i++)
    {
        int idleIndex = FindIdleParticle();
        if (idleIndex == -1)
        {
            // 没有找到非活动的粒子，跳出循环
            break;
        }

        GeneralParticle* idleParticle = particles[idleIndex];
        idleParticle->SetPosition(
            glm::vec3(
                sqrt(initialPosVar.x) * randNormal(randEng) + initialPos.x,
                sqrt(initialPosVar.y) * randNormal(randEng) + initialPos.y,
                sqrt(initialPosVar.z) * randNormal(randEng) + initialPos.z)
        );
        idleParticle->SetVelocity(
            glm::vec3(
                sqrt(initialVelocityVar.x) * randNormal(randEng) + initialVelocity.x,
                sqrt(initialVelocityVar.y) * randNormal(randEng) + initialVelocity.y,
                sqrt(initialVelocityVar.z) * randNormal(randEng) + initialVelocity.z)
        );
        idleParticle->SetLifeSpan(
            sqrt(initialLifeSpanVar) * randNormal(randEng) + initialLifeSpan);
        idleParticle->SetColor(
            glm::vec3(
                rand() / (float)RAND_MAX * 0.5f + 0.5f,
                rand() / (float)RAND_MAX * 0.5f + 0.5f,
                rand() / (float)RAND_MAX * 0.5f + 0.5f)
        );
        idleParticle->SetActive();
        currParticleNum++;
        // 不再需要设置位置和颜色，因为这些已经由 FindIdleParticle 中的 idleParticle 获得
    }

}


int ParticleSystem::FindIdleParticle()
{
	for (int i = lastIdx; i < maxParticles && i < particles.size(); i++)
	{
		if (!particles[i]->GetStatus())
		{
			lastIdx = (i + 1) % maxParticles;
			return i;
		}
	}

	for (int i = 0; i < lastIdx && i < particles.size(); i++)
	{
		if (!particles[i]->GetStatus())
		{
			lastIdx = (i + 1) % maxParticles;
			return i;
		}
	}

	return -1;
}


// void ParticleSystem::RenderSphere() {
//     glBindVertexArray(sphereVAO);
//     glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
//     glBindVertexArray(0);
// }


// void ParticleSystem::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
//     glUseProgram(shader);
//     glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, GL_FALSE, glm::value_ptr(viewProjMtx));

//     // Render each particle as a sphere
//     for (int i = 0; i < currParticleNum; i++) {
//         glm::mat4 model = glm::translate(glm::mat4(1.0f), positions[i]) * glm::scale(glm::mat4(1.0f), glm::vec3(radius));
//         glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
//         RenderSphere();
//     }

//     glUseProgram(0);
//             GLuint error;
//     while ((error = glGetError()) != GL_NO_ERROR) {
//         std::cerr << "OpenGL error at load sphere: " << error << std::endl;
//         exit(-1);
//     }
// }

void ParticleSystem::Draw(const glm::mat4& viewProjMtx, GLuint shader)
{
    glUseProgram(shader);

    glUniform1f(glGetUniformLocation(shader, "pointSize"), 20.0f); // 根据需要调整大小

    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, spriteTexture);
    glUniform1i(glGetUniformLocation(shader, "spriteTexture"), 0);
    
    glPointSize(10.0f);
    // Bind the VAO
    glBindVertexArray(vao);

    // Bind the position VBO and upload the data
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Bind the color VBO and upload the data
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Set the shader uniforms
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, GL_FALSE, (float*)&viewProjMtx);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);

    // Draw the points
    glDrawArrays(GL_POINTS, 0, currParticleNum);

    // Unbind the VAO
    glBindVertexArray(0);

    // for (int i = 0; i < currParticleNum; i++)
    // {
    //     if (particles[i]->isActive) {
    //         Dodecahedron dodecahedron(radius, particles[i]->position);
    //         dodecahedron.draw();
    //     }

    // }

    Dodecahedron dodecahedron(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    dodecahedron.draw();
    glUseProgram(0);
    
	ground->Draw(viewProjMtx);
}


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"  // 用于加载图像的库

void ParticleSystem::LoadSpriteTexture()
{
    int width, height, nrChannels;
    std::string texturePath1 = "shpere.png";
    unsigned char* data = stbi_load(texturePath1.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glGenTextures(1, &spriteTexture);
        glBindTexture(GL_TEXTURE_2D, spriteTexture);

        // 设置纹理参数
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 加载纹理数据
        if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }

        // 生成mipmap
        glGenerateMipmap(GL_TEXTURE_2D);

        // 解绑纹理
        glBindTexture(GL_TEXTURE_2D, 0);

        // 释放图像内存
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load texture: " << texturePath1 << std::endl;
    }
}

#include <fstream>
#include <sstream>

void ParticleSystem::LoadSphere() {
    std::ifstream file("sphere.obj");
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            sphereVertices.push_back(vertex);
        } else if (prefix == "f") {
            unsigned int index;
            std::string face;
            while (ss >> face) {
                std::stringstream faceStream(face);
                faceStream >> index;
                sphereIndices.push_back(index - 1); // OBJ index starts from 1
            }
        }
    }

    // Create VAO, VBO, and EBO
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(glm::vec3), sphereVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

}
