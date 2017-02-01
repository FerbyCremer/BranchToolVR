#include "Engine.h"

Engine::Engine()
{
	if (!InitGLFW() || !InitGLEW())
	{
		glfwTerminate();
		throw std::runtime_error("Failed to initialize Engine class. Exiting...");
	}
	
	// pass valid glfw window to the renderer
	renderer = new Render(window);

	InitObjects();

	Loop();
}

void Engine::InitObjects()
{
	// axis lines
	LineObject* axis = new LineObject;
	axis->GenerateAxis();
	renderer->AddObjectToScene(axis);

	// add the ground plane
	ColorObject* ground = new ColorObject;
	ground->GenerateGround();
	renderer->AddObjectToScene(ground);	
	
	// init dicom objects with the startup set, TODO: add in VR folder selection
	doc = new DicomObjectsContainer;
	doc->Load(DirectoryInfo::STARTUP_DICOM_SET);
	doc->AddObjects(renderer);
}

Engine::~Engine()
{
	delete doc;
}

bool Engine::InitGLFW() 
{
	if (!glfwInit()) 
	{
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, DEFAULT_NR_SAMPLES);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(DEFAULT_WINDOW_SIZE_X, DEFAULT_WINDOW_SIZE_X, WINDOW_TITLE_STRING, NULL, NULL);

	if (window == NULL) 
	{
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	return true;
}

bool Engine::InitGLEW() 
{
	glewExperimental = true;
	return glewInit() == GLEW_OK;
}

void Engine::Update() 
{
	doc->Update(renderer->GetVrData(), renderer->GetCursorData());
	renderer->SceneLoop();
}

void Engine::Loop() 
{
	double frame_time_start = glfwGetTime();
	double frame_time_end = glfwGetTime();

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) 
	{
		frame_time_start = glfwGetTime();
		double prev_frame_time = frame_time_start - frame_time_end;
	
		glfwPollEvents();
		Update();
		glfwSwapBuffers(window);

		//TODO: limit fps
		frame_time_end = frame_time_start;
	} 

	glfwTerminate();
}
