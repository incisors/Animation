#include "Window.h"

// Window Properties
int Window::width;
int Window::height;
const char *Window::windowTitle = "Lets go";

// Objects to render
Cube* Window::cube;
Skeleton *Window::skel;
Skin *Window::skin;
Player *Window::player;
Cloth* Window::cloth;
ParticleSystem* Window::ps;

float Window::newPosX;
float Window::newPosY;
float Window::newPosZ;
float Window::angle;

// Camera Properties
Camera *Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;
GLuint Window::ptShaderProgram;

clock_t Window::deltaT = 0;
clock_t Window::prevT;
int Window::countFPS = 0;
int Window::currFPS = 0;
// std::string Window::skelFilename;

// Constructors and desctructors
bool Window::initializeProgram()
{
GLfloat pointSizeRange[2];
glGetFloatv(GL_POINT_SIZE_RANGE, pointSizeRange);
std::cout << "Minimum supported point size: " << pointSizeRange[0] << std::endl;
std::cout << "Maximum supported point size: " << pointSizeRange[1] << std::endl;

    // Create a shader program with a vertex shader and a fragment shader.
    shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
    ptShaderProgram = LoadShaders("shaders/point.vert", "shaders/point.frag");
    // ptShaderProgram = LoadShaders("shaders/shader_particle.vert", "shaders/shader_particle.frag");

    // Check the shader program.
    if (!shaderProgram)
    {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }
    else
    {
        printf("Successfully initialized shader program!\n");
    }

    return true;
}

bool Window::initializeObjects(std::string skelfile, std::string skinfile, std::string animfile)
{

    if (!skelfile.empty())
    {
        skel = new Skeleton();
        skel->load(skelfile.c_str());
    }
    if (!skinfile.empty())
    {
        if (!skelfile.empty())
        {
            skin = new Skin(skinfile, skel, shaderProgram);
        }
        else
        {
            skin = new Skin(skinfile, nullptr, shaderProgram);
        }

        if (!animfile.empty())
        {
            Animation *anim = new Animation(animfile);
            player = new Player(skel, anim);
        }
    }

    // cloth = new Cloth(15, 15, shaderProgram);
    if (cloth != nullptr)
    {
        newPosY = cloth->Height;
        newPosZ = 0.0f;    
    }


    angle = 0.0f;
    // particleSys = new ParticleSystem(particleShaderProgram, shaderProgram);
    // cube = new Cube();
    // cube = new Cube(glm::vec3(-1, 0, -2), glm::vec3(1, 1, 1));

    ps = new ParticleSystem(ptShaderProgram, shaderProgram);

	prevT = clock();

    std::cout << "Initialized objects" << std::endl;
    
    // Skeleton* skel = new Skeleton();
    // skel.Load(skelFilename);
    // // Create a cube
    // cube = new Cube();

    return true;
}

void Window::cleanUp()
{
    // Deallcoate the objects.
    // delete cube;
    delete skel;
    delete skin;
    delete player;
    delete cloth;
    if (ps) delete ps;
    // Delete the shader program.
    glDeleteProgram(shaderProgram);
    glDeleteProgram(ptShaderProgram);

}

// for the Window
GLFWwindow *Window::createWindow(int width, int height)
{
    // Initialize GLFW.
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return NULL;
    }

    // 4x antialiasing.
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Apple implements its own version of OpenGL and requires special treatments
    // to make it uses modern OpenGL.
    // Ensure that minimum OpenGL version is 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Enable forward compatibility and allow a modern OpenGL context
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create the GLFW window.
    GLFWwindow *window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

    // Check if the window could not be created.
    if (!window)
    {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return NULL;
    }

    // Make the context of the window.
    glfwMakeContextCurrent(window);

    // Set swap interval to 1.
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    // set up the camera
    Cam = new Camera();
    Cam->SetAspect(float(width) / float(height));

    // initialize the interaction variables
    LeftDown = RightDown = false;
    MouseX = MouseY = 0;

    // Call the resize callback to make sure things get drawn immediately.
    Window::resizeCallback(window, width, height);

    return window;
}

void Window::resizeCallback(GLFWwindow *window, int width, int height)
{
#ifdef __APPLE__
    // In case your Mac has a retina display.
    glfwGetFramebufferSize(window, &width, &height);
#endif
    Window::width = width;
    Window::height = height;
    // Set the viewport size.
    glViewport(0, 0, width, height);

    Cam->SetAspect(float(width) / float(height));
}

// update and draw functions
void Window::idleCallback()
{
    // Perform any updates as necessary.
    Cam->Update();
    if (cube != nullptr) {
        cube->update();
    }

    if (player != nullptr)
    {
        player->update();
    }

    if (skel != nullptr)
    {
        skel->update(glm::mat4(1.0f));
    }

    if (skin != nullptr)
    {
        skin->update();
    }

    if (cloth != nullptr)
    {
        cloth->Update(0.01f);
    }
    if (ps != nullptr)
    {
        ps->Update();
    }
}

void Window::displayCallback(GLFWwindow *window)
{
    //FPS
	clock_t currT = clock();
	deltaT += currT - prevT;
	prevT = currT;
	countFPS++;
	if (deltaT >= 1000)
	{
		currFPS = countFPS;
		countFPS = 0;
		deltaT -= 1000;
	}
    
    // Clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the object.
    if (cube != nullptr) {
        cube->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
    }

    if (skin != nullptr)
    {
        skin->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
    }
    else if (skel != nullptr)
    {
        skel->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
    }
    GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error at upda1te: " << err << std::endl;
        exit(1);
	}
	if (cloth != nullptr)
	{
		cloth->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	}

    if (ps != nullptr)
    {
        ps->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
    }
    glfwPollEvents();

    if (skel != nullptr) {
        plotSkeletonImGUI();
    }
    if (cloth != nullptr) {
        plotClothImGUI();
    }
    if (ps != nullptr) {
        plotParticleSystemImGUI();
    }


    // skel->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
    // Gets events, including input such as keyboard and mouse or window resizing.

    // Swap buffers.
    
    glfwSwapBuffers(window);
}

// helper to reset the camera
void Window::resetCamera()
{
    Cam->Reset();
    Cam->SetAspect(float(Window::width) / float(Window::height));
}

// callbacks - for Interaction
void Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    /*
     * TODO: Modify below to add your key callbacks.
     */

    // Check for a key press.
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            // Close the window. This causes the program to also terminate.
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;

        case GLFW_KEY_R:
            resetCamera();
            break;

        default:
            break;
        }
    }
}

void Window::mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    const ImGuiIO &io = ImGui::GetIO();
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        LeftDown = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        RightDown = (action == GLFW_PRESS);
    }
}

void Window::cursor_callback(GLFWwindow *window, double currX, double currY)
{
    int maxDelta = 100;
    int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
    int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

    MouseX = (int)currX;
    MouseY = (int)currY;

    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2((float)currX, (float)currY);

    // Move camera
    // NOTE: this should really be part of Camera::Update()
    if (LeftDown)
    {
        const float rate = 1.0f;
        Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
        Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
    }
    if (RightDown)
    {
        const float rate = 0.005f;
        float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
        Cam->SetDistance(dist);
    }
}
void Window::plotSkeletonImGUI(){

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (skel != nullptr)
    {
        ImGui::Begin("Joints");
        int index = 0;
        for (auto tmp_ptr : skel->joints)
        {
            if (tmp_ptr)
            {
                ImGui::PushID(index); // 使用索引作为ID
                if (ImGui::Button("+"))
                {
                    tmp_ptr->incX();
                }
                ImGui::SameLine();
                ImGui::Text("%s x: %f", tmp_ptr->name.c_str(), tmp_ptr->dofx->getValue());
                ImGui::SameLine();
                if (ImGui::Button("-"))
                {
                    tmp_ptr->decX();
                }
                ImGui::PopID();
                index++;
                ImGui::PushID(index);
                if (ImGui::Button("+"))
                {
                    tmp_ptr->incY();
                }
                ImGui::SameLine();
                ImGui::Text("%s y: %f", tmp_ptr->name.c_str(), tmp_ptr->dofy->getValue());
                ImGui::SameLine();
                if (ImGui::Button("-"))
                {
                    tmp_ptr->decY();
                }
                ImGui::PopID();
                index++;
                ImGui::PushID(index);
                if (ImGui::Button("+"))
                {
                    tmp_ptr->incZ();
                }
                ImGui::SameLine();
                ImGui::Text("%s z: %f", tmp_ptr->name.c_str(), tmp_ptr->dofz->getValue());
                ImGui::SameLine();
                if (ImGui::Button("-"))
                {
                    tmp_ptr->decZ();
                }
                ImGui::PopID();
                index++;
            }
            else
            {
                std::cerr << "JOINT NOT FOUND!" << std::endl;
            }
        }
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}
float SpringConstant = 0.0f;


void Window::plotClothImGUI() {

    // Tell OpenGL a new frame is about to begin
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGUI window creation
    ImGui::Begin("Cloth Controls");

    // ImGui to control the position of the particles in the cloth if they are fixed
    ImGui::SliderFloat("X Position", &newPosX, -1.0f, 1.0f);
    ImGui::SliderFloat("Y Position", &newPosY, 0.0f, cloth->Height + 10.0f);
    ImGui::SliderFloat("Z Position", &newPosZ, -10.0f, 20.0f);

    // if particle is fixed, set the new position while keeping the fixed particles in a row
    for(int i = 0; i < cloth->Particles.size(); i++) {
        if(cloth->Particles[i]->isFixed) {
            cloth->Particles[i]->Position.x += newPosX * 0.01;
            cloth->Particles[i]->Position.y = newPosY;
            cloth->Particles[i]->Position.z = newPosZ;
        }
    }

    
    
    // // ImGui to rotate the particles in the cloth that are fixed around the z-axis
    // ImGui::SliderFloat("Angle", &angle, -1.0f, 1.0f);
    // float radians = glm::radians(angle);

    // // Assuming originalFixedPositions is a member variable of the Cloth class that stores the original positions.
    // for(size_t i = 0; i < cloth->Particles.size(); ++i) {
    //     Particle* p = cloth->Particles[i];
    //     if (p->isFixed) {
    //         // Get the original position
    //         glm::vec3 originalPos = cloth->positions[i];
    //         // Apply rotation around the z-axis
    //         p->Position.x = originalPos.x * glm::cos(radians) - originalPos.y * glm::sin(radians);
    //         p->Position.y = originalPos.x * glm::sin(radians) + originalPos.y * glm::cos(radians);
    //         // z remains the same since it's a rotation around the z-axis
    //         p->Position.z = originalPos.z;
    //     }
    // }

   
    // ImGui to control the wind speed
    ImGui::SliderFloat("Wind Speed", &cloth->forwardSpeed, 0.0f, 3.0f);
    ImGui::SliderFloat("Spring Constant", &SpringConstant, 0.0f, 10.0f);
    ImGui::SliderFloat("Particle Mass", &cloth->ParticleMass, 0.0f, 10.0f);

    for (SpringDamper* sd : cloth->Springs) {
        sd->SpringConstant = SpringConstant;
    }

    // buttons to go to next and previous joint
    // if (ImGui::Button("Next Joint") && skeleton->currJoint < skeleton->allJoints.size() - 1) {
    //     skeleton->currJoint++;
    // }

    // if (ImGui::Button("Previous Joint") && skeleton->currJoint > 0) {
    //     skeleton->currJoint--;
    // }

    // Joint* currentJoint = skeleton->allJoints[skeleton->currJoint];
    
    // int n = 0;
    // for (DOF* dof : currentJoint->dofs) {
    //     ImGui::SliderFloat("DOF slider: " + *std::to_string(n).c_str(), &dof->value, dof->min, dof->max);
    //     n++;
    // }

    // // Reset button to reset dof values
    // if (ImGui::Button("Reset")) {
    //     for (DOF* dof : currentJoint->dofs) {
    //         dof->value = 0.0f;
    //         // skeleton->Load(filename);
    //     }
    // }

    // // Toggle button to show the skin
    // if (ImGui::Button("Toggle Skin")) {
    //     skinDrawn = !skinDrawn;
    // }

    // Ends the window
    ImGui::End();

    // Renders the ImGUI elements
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void Window::plotParticleSystemImGUI() {
    // Tell OpenGL a new frame is about to begin
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGUI window creation
    ImGui::Begin("Particle System Controls");

    // Slider for particle creation rate
    ImGui::SliderFloat("Creation Rate", &ps->creationRate, 0.01f, 20.0f);

    // Sliders for initial position and variance
    ImGui::SliderFloat3("Initial Position", &ps->initialPos[0], -100.0f, 100.0f);
    ImGui::SliderFloat3("Initial Position Variance", &ps->initialPosVar[0], 0.0f, 100.0f);

    // Sliders for initial velocity and variance
    ImGui::SliderFloat3("Initial Velocity", &ps->initialVelocity[0], -10.0f, 10.0f);
    ImGui::SliderFloat3("Initial Velocity Variance", &ps->initialVelocityVar[0], 0.0f, 5.0f);

    // Sliders for initial life span and variance
    ImGui::SliderFloat("Initial Life Span", &ps->initialLifeSpan, 0.0f, 1000.0f);
    ImGui::SliderFloat("Initial Life Span Variance", &ps->initialLifeSpanVar, 0.0f, 50.0f);

    // Sliders for physics parameters
    ImGui::SliderFloat("Gravity", &ps->g, -5.0f, 10.0f);
    ImGui::SliderFloat("Air Density", &ps->airDensity, 0.0f, 2.0f);
    ImGui::SliderFloat("Drag Coefficient", &ps->dragConst, 0.0f, 1.0f);
    ImGui::SliderFloat("Particle Radius", &ps->radius, 0.1f, 25.0f);
    ImGui::SliderFloat("Collision Elasticity", &ps->elasticity, 0.0f, 1.0f);
    ImGui::SliderFloat("Collision Friction", &ps->friction, 0.0f, 1.0f);

    // Ends the window
    ImGui::End();

    // Renders the ImGUI elements
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
