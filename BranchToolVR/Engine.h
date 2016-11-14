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
#include "Camera.h"
#include "ColorObject.h"
#include "LineObject.h"
#include "DicomReader.h"
#include "UiPanel.h"
#include "DicomObjectsContainer.h"
#include "Constants.h"

class Engine{

	public:

		Engine();
		~Engine();

	private:

		DicomObjectsContainer * dicomObjects1;
		ColorObject * controller;
		ColorObject * ground;

		bool InitGLFW();
		bool InitGLEW();
		bool InitVR();
		void FakeControllerInput(float _deltaT);
		void Loop();
		void Update();


		// glfw vars
		GLFWwindow * window;
		int window_size_x;
		int window_size_y;

		// controller state data
		glm::mat4 controller_mm1;
		bool controller_press1;
		glm::mat4 controller_mm2;
		bool controller_press2;

		// internal classes - pointers to allocate after Init()
		Render * renderer;
		Camera * nonVR_camera;
};
