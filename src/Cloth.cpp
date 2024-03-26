#include "Cloth.h"

Cloth::Cloth(int width, int height, GLuint programID) {
    color = glm::vec3(0.6f, 0.35f, 0.7f);
    model = glm::mat4(1.0f);
    // Generate a vertex array (VAO) and two vertex buffer objects (VBO).
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &positionVBO);
    glGenBuffers(1, &normalVBO);
    glGenBuffers(1, &ebo);

    Width = width;
    Height = height;
    int nPartRow = Width+1;
    int nPartCol = Height+1;

    ParticleMass = 1.232f;
    TotalMass = Width*Height*ParticleMass;
    float springConstant = 5.0f;
    float dampingConstant = 5.0f;
    float restLength = 1.0f;

    // Create particles
    for (int i=0; i<=Width; i++) {
        for (int j=0; j<=Height; j++) {
            Particle* p = new Particle();
            if (j == Height) {
                p->isFixed = true;
            }
            p->Position = glm::vec3(i, j, 0);
            p->Mass = ParticleMass;
            Particles.push_back(p);
        }
    }
	ground = new Ground(
		glm::vec3(5.0f, 0.0f, -5.0f), 
		25.0f, 
		programID);

    std::cout << "Particles created" << std::endl;

    std::cout << "Particles size: " << Particles.size() << std::endl;

    // Create springs
    for (int i=0; i<Width; i++) {
        for (int j=0; j<Height; j++) {
            // Bottom side of square
            SpringDamper* s1 = new SpringDamper;
            s1->P1 = Particles[i * nPartCol + j];
            s1->P2 = Particles[i * nPartCol + j + 1];
            s1->SpringConstant = springConstant;
            s1->DampingConstant = dampingConstant;
            s1->RestLength = restLength;
            Springs.push_back(s1);

            // Left side of square
            SpringDamper* s2 = new SpringDamper;
            s2->P1 = Particles[i * nPartCol + j];
            s2->P2 = Particles[i * nPartCol + j + nPartCol];
            s2->SpringConstant = springConstant;
            s2->DampingConstant = dampingConstant;
            s2->RestLength = restLength;
            Springs.push_back(s2);


            // Diagonals bottom left to top right
            SpringDamper* s3 = new SpringDamper;
            s3->P1 = Particles[i * nPartCol + j];
            s3->P2 = Particles[i * nPartCol + j + nPartCol + 1];
            s3->SpringConstant = springConstant;
            s3->DampingConstant = dampingConstant;
            s3->RestLength = restLength*sqrt(2);
            Springs.push_back(s3);


            // Diagonals top left to bottom right
            SpringDamper* s4 = new SpringDamper;
            s4->P1 = Particles[i * nPartCol + j + 1];
            s4->P2 = Particles[i * nPartCol + j + nPartCol];
            s4->SpringConstant = springConstant;
            s4->DampingConstant = dampingConstant;
            s4->RestLength = restLength*sqrt(2);
            Springs.push_back(s4);

            
            // Top side of cloth
            if (j == Height-1) {
                SpringDamper* s5 = new SpringDamper;
                s5->P1 = Particles[j * nPartCol + nPartCol + i];
                s5->P2 = Particles[j * nPartCol + nPartCol + i + 1];
                s5->SpringConstant = springConstant;
                s5->DampingConstant = dampingConstant;
                s5->RestLength = restLength;
                Springs.push_back(s5);

            }

            // Right side of cloth
            if (i == Width-1) {
                SpringDamper* s6 = new SpringDamper;
                s6->P1 = Particles[i + (nPartCol * j ) + 1];
                s6->P2 = Particles[i + (nPartCol * j )+ nPartCol + 1];
                s6->SpringConstant = springConstant;
                s6->DampingConstant = dampingConstant;
                s6->RestLength = restLength;
                Springs.push_back(s6);

            }
        }
    }

    std::cout << "Springs created" << std::endl;

    // Create triangles
    for (int i=0; i<Width-1; i++) {
        for (int j=0; j<Height-1; j++) {
            Triangle* t1 = new Triangle;
            t1->P1 = Particles[i * nPartCol + j];
            t1->P2 = Particles[i * nPartCol + j + 1];
            t1->P3 = Particles[(i+1) * nPartCol + j];
            t1->Normal = glm::normalize(glm::cross(t1->P2->Position - t1->P1->Position, t1->P3->Position - t1->P1->Position));
            Triangles.push_back(t1);

            Triangle* t2 = new Triangle;
            t2->P1 = Particles[(i+1) * nPartCol + j];
            t2->P2 = Particles[i * nPartCol + j + 1];
            t2->P3 = Particles[(i+1) * nPartCol + j + 1];
            t2->Normal = glm::normalize(glm::cross(t2->P2->Position - t2->P1->Position, t2->P3->Position - t2->P1->Position));
            Triangles.push_back(t2);
        }
    }

    std::cout << "Triangles created" << std::endl;

    // populate position and normal vectors
    for (int i=0; i<Particles.size(); i++) {
        positions.push_back(Particles[i]->Position);
        normals.push_back(Particles[i]->Normal);
    }

    // populate indices vector
    for (int i=0; i<Width; i++) {
        for (int j=0; j<Height; j++) {
            indices.push_back(i * nPartCol + j);
            indices.push_back(i * nPartCol + j + 1);
            indices.push_back((i+1) * nPartCol + j);
            indices.push_back((i+1) * nPartCol + j);
            indices.push_back(i * nPartCol + j + 1);
            indices.push_back((i+1) * nPartCol + j + 1);
        }
    }
}

Cloth::~Cloth() {
    for (int i=0; i<Particles.size(); i++) {
        delete Particles[i];
    }
    for (int i=0; i<Springs.size(); i++) {
        delete Springs[i];
    }
    for (int i=0; i<Triangles.size(); i++) {
        delete Triangles[i];
    }
    for(int i=0; i<positions.size(); i++) {
        positions.pop_back();
    }
    for(int i=0; i<normals.size(); i++) {
        normals.pop_back();
    }
    for(int i=0; i<indices.size(); i++) {
        indices.pop_back();
    }
    glDeleteBuffers(1, &positionVBO);
    glDeleteBuffers(1, &normalVBO);
    glDeleteVertexArrays(1, &meshVAO);
    glDeleteBuffers(1, &ebo);
}

void Cloth::Update(float deltaTime) {
    // Apply gravity
    for (int i=0; i<Particles.size(); i++) {
        Particles[i]->Force = glm::vec3(0, -0.1f, 0) * Particles[i]->Mass;
    }
    // Apply spring forces
    for (int i=0; i<Springs.size(); i++) {
        Springs[i]->ComputeForce();
    }

    // Aerodynamics
    const float airDensity = 0.2f; // kg/m^3 at sea level
    const float dragCoefficient = 1.28f;
    glm::vec3 windVelocity(0, 0, forwardSpeed);

    for (int i = 0; i < Triangles.size(); i++) {
        glm::vec3 v1 = Triangles[i]->P1->Velocity;
        glm::vec3 v2 = Triangles[i]->P2->Velocity;
        glm::vec3 v3 = Triangles[i]->P3->Velocity;
        
        // Average velocity of the triangle's vertices
        glm::vec3 averageVelocity = (v1 + v2 + v3) / 3.0f;
        glm::vec3 relativeVelocity = averageVelocity - windVelocity;
        float crossSectionalArea = 0.5f * glm::length(glm::cross(Triangles[i]->P2->Position - Triangles[i]->P1->Position, Triangles[i]->P3->Position - Triangles[i]->P1->Position));
        
        glm::vec3 force = -0.5f * airDensity * (glm::length(relativeVelocity) * glm::length(relativeVelocity)) * dragCoefficient * (crossSectionalArea * glm::dot(glm::normalize(relativeVelocity), Triangles[i]->Normal)) * Triangles[i]->Normal;
        
        if (glm::dot(relativeVelocity, Triangles[i]->Normal) > 0) {
            Triangles[i]->P1->ApplyForce(force);
            Triangles[i]->P2->ApplyForce(force);
            Triangles[i]->P3->ApplyForce(force);
        }
    }


    // Integrate
    for (int i=0; i<Particles.size(); i++) {
        Particles[i]->Integrate(deltaTime);
    }
    

    // Collision detection and response
    for (Particle* p : Particles) {
        if (p->Position.y < 0) {

            // Reflect the velocity around the y-axis to simulate a bounce
            glm::vec3 impulse = glm::vec3(0, -p->Velocity.y, 0);

            float restitution = 0.0002f; // restitution factor between 0 and 1
            p->ApplyImpulse(restitution * impulse);

            p->Position.y = 0;
        }
    }

    // write position and normal vectors to OpenGl data stuctures
    for (int i=0; i<Particles.size(); i++) {
        positions[i] = Particles[i]->Position;
        normals[i] = Particles[i]->Normal;
    }
}

// Draw using OpenGL
void Cloth::Draw(const glm::mat4& viewProjMtx, GLuint shader) {
    // Bind the VAO
    glBindVertexArray(meshVAO);
    // Bind the position VBO
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    // Upload the data
    glBufferData(GL_ARRAY_BUFFER, Particles.size()*sizeof(glm::vec3), positions.data(), GL_DYNAMIC_DRAW);
    // Set the vertex attribute pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // Bind the normal VBO
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    // Upload the data
    glBufferData(GL_ARRAY_BUFFER, Particles.size()*sizeof(glm::vec3), normals.data(), GL_DYNAMIC_DRAW);
    // Set the vertex attribute pointers
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // Bind the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // Upload the data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);
    // actiavte the shader program
    glUseProgram(shader);

    // get the locations and send the uniforms to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
    glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

    // Bind the VAO
    glBindVertexArray(meshVAO);

    // Draw the triangles
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO
    glBindVertexArray(0);

	ground->Draw(viewProjMtx);
}