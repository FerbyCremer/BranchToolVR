#pragma once

// ext
#include <GL/glew.h>
#include <glm/gtc/matrix_access.hpp>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <openvr.h>
#include <chrono>

// int
#include "Render.h"
#include "ColorObject.h"
#include "LineObject.h"
#include "DicomReader.h"
#include "UiPanel.h"
#include "DicomObjectsContainer.h"
#include "Constants.h"

class Engine
{

	public:

		Engine();
		~Engine();

	private:

		// objects
		DicomObjectsContainer * dicomObjects;
		ColorObject * ground;
		LineObject * axis;

		// functions
		bool InitGLFW();
		bool InitGLEW();
		void Loop();
		void Update();

		// glfw vars
		GLFWwindow * window;

		// internal classes - pointers to allocate after Init()
		Render * renderer;
};
