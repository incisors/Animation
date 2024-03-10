#include "Window.h"

// Window Properties
int Window::width;
int Window::height;
const char *Window::windowTitle = "Lets go LVG";

// Objects to render
// Cube* Window::cube;
Skeleton *Window::skel;
Skin *Window::skin;
Player *Window::player;
Cloth* Window::cloth;

// Camera Properties
Camera *Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;

// std::string Window::skelFilename;

// Constructors and desctructors
bool Window::initializeProgram()
{
    // Create a shader program with a vertex shader and a fragment shader.
    shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

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

    cloth = new Cloth(
		4.0f, // size 
		100.0, //mass 
		25, // N particles
		glm::vec3(-2.0f, 3.0f, 0.0f), //topleft
		glm::vec3(1.0f, 0.0f, 0.0f), // horizontal
		glm::vec3(0.0f, -1.0f, 0.0f), // vertical
		shaderProgram);
    
    std::cout << "Initialized objects" << std::endl;
    
    // Skeleton* skel = new Skeleton();
    // skel.Load(skelFilename);
    // // Create a cube
    // cube = new Cube();
    // cube = new Cube(glm::vec3(-1, 0, -2), glm::vec3(1, 1, 1));

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
    // Delete the shader program.
    glDeleteProgram(shaderProgram);
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
        GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cerr << "OpenGL error at update: " << err << std::endl;
	}
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
        cloth->Update();
    }

}

void Window::displayCallback(GLFWwindow *window)
{

    // Clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the object.
    // cube->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
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
		std::cerr << "OpenGL error at update: " << err << std::endl;
        exit(1);
	}
	if (cloth != nullptr)
	{
		cloth->Draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	}

    glfwPollEvents();

    plotSkeletonImGUI();
    // plotClothImGUI();

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

void Window::plotClothImGUI()
{
	//start imgui new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//bool show_demo_window = true;
	//ImGui::ShowDemoWindow(&show_demo_window);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	//gui window
	ImGui::Begin("Coefficients", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2(Window::width - 260, 0));
	ImGui::SetWindowSize(ImVec2(260, Window::height));

	std::string fps = "FPS: " + std::to_string(cloth->GetFPS());
	ImGui::Text(fps.c_str());

	if (ImGui::TreeNode("Fixed Points"))
	{
		float posShift = 0.05f;
		if (ImGui::Button("\t+\t###XP"))
		{
			cloth->TranslateFixedParticles(0, posShift);
		}
		ImGui::SameLine();
		if (ImGui::Button("\t-\t###XM"))
		{
			cloth->TranslateFixedParticles(0, -posShift);
		}
		ImGui::SameLine();
		ImGui::Text("Axis X");

		if (ImGui::Button("\t+\t###YP"))
		{
			cloth->TranslateFixedParticles(1, posShift);
		}
		ImGui::SameLine();
		if (ImGui::Button("\t-\t###YM"))
		{
			cloth->TranslateFixedParticles(1, -posShift);
		}
		ImGui::SameLine();
		ImGui::Text("Axis Y");

		if (ImGui::Button("\t+\t###ZP"))
		{
			cloth->TranslateFixedParticles(2, posShift);
		}
		ImGui::SameLine();
		if (ImGui::Button("\t-\t###ZM"))
		{
			cloth->TranslateFixedParticles(2, -posShift);
		}
		ImGui::SameLine();
		ImGui::Text("Axis Z");

		float rotShift = 0.02f;
		if (ImGui::Button("\t+\t###RXP"))
		{
			cloth->RotateFixedParticles(0, rotShift);
		}
		ImGui::SameLine();
		if (ImGui::Button("\t-\t###RXM"))
		{
			cloth->RotateFixedParticles(0, -rotShift);
		}
		ImGui::SameLine();
		ImGui::Text("Rot X");

		if (ImGui::Button("\t+\t###RYP"))
		{
			cloth->RotateFixedParticles(1, rotShift);
		}
		ImGui::SameLine();
		if (ImGui::Button("\t-\t###RYM"))
		{
			cloth->RotateFixedParticles(1, -rotShift);
		}
		ImGui::SameLine();
		ImGui::Text("Rot Y");

		if (ImGui::Button("\t+\t###RZP"))
		{
			cloth->RotateFixedParticles(2, rotShift);
		}
		ImGui::SameLine();
		if (ImGui::Button("\t-\t###RZM"))
		{
			cloth->RotateFixedParticles(2, -rotShift);
		}
		ImGui::SameLine();
		ImGui::Text("Rot Z");

		ImGui::NewLine();

		int numOfFixedPoints = cloth->GetFixedParticleNum();
		for (int i = 0; i < numOfFixedPoints; i++)
		{
			glm::vec3 pos = cloth->GetFixedParticlePos(i);
			std::string name = "Pos " + std::to_string(i);
			if (ImGui::InputFloat3(name.c_str(), glm::value_ptr(pos)))
			{
				cloth->SetFixedParticlePos(i, pos);
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Cloth Coefficients"))
	{
		float clothTotalMass = cloth->GetMass();
		if (ImGui::DragFloat("Mass", &clothTotalMass))
		{
			cloth->SetMass(abs(clothTotalMass));
		}

		float g = cloth->GetGravityAcce();
		if (ImGui::DragFloat("g", &g))
		{
			cloth->SetGravityAcce(abs(g));
		}

		float groundPos = cloth->GetGroundPos();
		if (ImGui::DragFloat("Ground", &groundPos))
		{
			cloth->SetGroundPos(groundPos);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("SpringDamper Coefficients"))
	{
		float k_spring = cloth->GetSpringConst();
		if (ImGui::DragFloat("Spring Constant", &k_spring))
		{
			cloth->SetSpringConst(abs(k_spring));
		}

		float k_damper = cloth->GetDampingConst();
		if (ImGui::DragFloat("Damping Constant", &k_damper))
		{
			cloth->SetDampingConst(abs(k_damper));
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Aerodynamic Coefficients"))
	{
		glm::vec3 windVelocity = cloth->GetWindVelocity();
		if (ImGui::InputFloat3("V_wind", glm::value_ptr(windVelocity)))
		{
			cloth->SetWindVelocity(windVelocity);
			/*std::cout << "Wind, x: " << windVelocity.x
				<< ", y: " << windVelocity.y
				<< ", z: " << windVelocity.z << std::endl;*/
		}


		float rho = cloth->GetFluidDensity();
		if (ImGui::DragFloat("rho", &rho))
		{
			cloth->SetFluidDensity(abs(rho));
		}

		float C_d = cloth->GetDragConst();
		if (ImGui::DragFloat("C_d", &C_d))
		{
			cloth->SetDragConst(abs(C_d));
			//std::cout << "C_d: " << C_d << std::endl;
		}

		ImGui::TreePop();
	}

	ImGui::End();

	//draw imgui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
