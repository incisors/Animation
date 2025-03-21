#pragma once

#include "Camera.h"
#include "Cube.h"
#include "Shader.h"
#include "Player.h"
#include "Animation.h"
#include "core.h"
#include "Tokenizer.h"
#include "Skeleton.h"
#include "Skin.h"
#include "Cloth.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream>
#include <vector>
#include "glm/gtc/type_ptr.hpp"
#include "ParticleSystem.h"
#include "shier.h"
class Window {
public:
    // Window Properties
    static int width;
    static int height;
    static const char* windowTitle;

    // Objects to render
    static Cube* cube;
    static Skeleton* skel;
    static Skin* skin;
    static Player* player;
    static Cloth* cloth;
    static ParticleSystem* ps;

    // Shader Program
    static GLuint shaderProgram;
	static GLuint ptShaderProgram;

    // Act as Constructors and desctructors
    static bool initializeProgram();
    static bool initializeObjects(std::string skelfile, std::string skinfile, std::string animfile);
    static void cleanUp();

    // for the Window
    static GLFWwindow* createWindow(int width, int height);
    static void resizeCallback(GLFWwindow* window, int width, int height);

    // update and draw functions
    static void idleCallback();
    static void displayCallback(GLFWwindow*);

    // helper to reset the camera
    static void resetCamera();

    // callbacks - for interaction
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_callback(GLFWwindow* window, double currX, double currY);

    static void plotSkeletonImGUI();
    static void plotClothImGUI();
    static void plotParticleSystemImGUI();

	// FPS count
	static clock_t deltaT;
	static clock_t prevT;
	static int countFPS;
	static int currFPS;

    static float newPosX;
    static float newPosY;
    static float newPosZ;
    static float angle;

};