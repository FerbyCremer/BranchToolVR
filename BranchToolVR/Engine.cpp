#include "Engine.h"
	ColorObject * box = new ColorObject;
	ColorObject * debug1 = new ColorObject;
	ColorObject * debug2 = new ColorObject;
	ColorObject * debug3 = new ColorObject;
	ColorObject * debug4 = new ColorObject;
Engine::Engine()
{
	if (!InitGLFW() || !InitGLEW()) 
	{
		glfwTerminate();
		throw std::runtime_error("Failed to initialize Engine class. Exiting...");
	}
	
	// glew has been initialized
	renderer = new Render(window);

	// axis lines
	axis = new LineObject;
	axis->GenerateAxis();
	renderer->AddObjectToScene(axis);

	// TEMP: set dicom directory and panel values
	dicomObjects = new DicomObjectsContainer;
	dicomObjects->AddObjects(renderer);
	dicomObjects->Load(DirectoryInfo::STARTUP_DICOM_SET);
	
	// scene
	ground = new ColorObject;
	ground->GenerateRoom();
	renderer->AddObjectToScene(ground);


	box->GenerateXYPrism(glm::vec3(1.0f), glm::vec2(0.0f), glm::vec3(0));
	box->is_selectable = true;
	box->SetDisplayColor(glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
	box->Set_world_position(glm::vec3(-2.0f, 0.2f, 0.0f));
	renderer->AddObjectToScene(box);

	debug1 = new ColorObject;
	debug1->GenerateSphere(10, 0.1f, false);
	debug1->SetDisplayColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//renderer->AddObjectToScene(debug1);

	debug2 = new ColorObject;
	debug2->GenerateSphere(10, 0.1f, false);
	debug2->SetDisplayColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	//renderer->AddObjectToScene(debug2);

	debug3 = new ColorObject;
	debug3->GenerateSphere(10, 0.2f, false);
	debug3->SetDisplayColor(glm::vec4(0.0f, 0.0f, 1.25f, 1.0f));
	//renderer->AddObjectToScene(debug3);

	debug4 = new ColorObject;
	debug4->GenerateSphere(10, 0.2f, false);
	debug4->SetDisplayColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//renderer->AddObjectToScene(debug4);


	// call final render initializer after all objects have been added
	renderer->Finalize();

	Loop();
}

Engine::~Engine()
{
}

bool Engine::InitGLFW() 
{
	// Initialise GLFW
	if (!glfwInit()) 
	{
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 0);
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
		dicomObjects->Update(renderer->half_aspect, renderer->vr_info, renderer->cursor_info);
		renderer->RenderScene();

		// test dual motion controller box
		if (box->is_selected)
		{
			if (box->is_double_selected) 
			{
				glm::mat4 rot = MiscFunctions::RotateTo(box->cache.primary_to_secondary_collision_point_initial, box->cache.primary_to_secondary_collision_point_current);
				glm::mat4 curr_pose = box->cache.controller_pose_updated * glm::translate(glm::mat4(1.0f), box->cache.primary_collision_point_controller_space_initial)* rot* glm::translate(glm::mat4(1.0f), -box->cache.primary_collision_point_controller_space_initial) * box->cache.to_controller_space_initial;
				box->Set_append_pose(curr_pose);
			}
			else 
			{
				glm::mat4 curr_pose = box->cache.controller_pose_updated * box->cache.to_controller_space_initial;
				box->Set_append_pose(curr_pose);
			}
		}

}

void Engine::Loop() 
{
	auto begin = std::chrono::high_resolution_clock::now();
	auto end = std::chrono::high_resolution_clock::now();

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) 
	{
		begin = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(begin - end).count();
		
		Update();

		glfwSwapBuffers(window);
		glfwPollEvents();		

		end = begin;
	} 

	glfwTerminate();
}
