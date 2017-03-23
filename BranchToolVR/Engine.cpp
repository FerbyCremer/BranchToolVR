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
	glfwSwapInterval(1);

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
	double last_frame_time = glfwGetTime();
	int set_elapsed_time = 5.0f;
	int frame_counter = 0;


	double max_fps = 60.0f;
	double min_frame_time = 1.0f/ max_fps;
	double frame_start = 0, frame_end = 0;

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) 
	{
		frame_start = glfwGetTime();
		double frame_time = frame_end - frame_start;

		if (frame_time < min_frame_time)
		{
			Sleep((min_frame_time - frame_time)*1000.0f);
		}

		glfwPollEvents();
		Update();
		glfwSwapBuffers(window);

		double curr_time = glfwGetTime();

		if ((curr_time - last_frame_time) >= set_elapsed_time)
		{
			std::cout << (double)frame_counter / set_elapsed_time << std::endl;
			last_frame_time = curr_time;
			frame_counter = 0;
		}
		frame_counter++;

		frame_end = glfwGetTime();
	} 

	glfwTerminate();
}
