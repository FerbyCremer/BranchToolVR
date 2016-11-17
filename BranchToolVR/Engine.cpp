#include "Engine.h"
	ColorObject * box = new ColorObject;
	ColorObject * debug1 = new ColorObject;
	ColorObject * debug2 = new ColorObject;
	ColorObject * debug3 = new ColorObject;
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
	dicomObjects->Load(Constants::RELATIVE_DICOM_RESOURCE_PATH);	//"Resources\\DICOM\\torso\\DCM0"
	
	// scene
	ground = new ColorObject;
	ground->GenerateRoom();
	renderer->AddObjectToScene(ground);


	box->GenerateXYPrism(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(0.0f), -glm::vec3(0.25f, 0.25f, 0.25f));
	box->is_selectable = true;
	renderer->AddObjectToScene(box);

	debug1 = new ColorObject;
	debug1->GenerateSphere(10, 0.1f, false);
	debug1->SetDisplayColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	renderer->AddObjectToScene(debug1);

	debug2 = new ColorObject;
	debug2->GenerateSphere(10, 0.1f, false);
	debug2->SetDisplayColor(glm::vec4(1.0f, 0.0f, 0.25f, 1.0f));
	renderer->AddObjectToScene(debug2);

	debug3 = new ColorObject;
	debug3->GenerateSphere(10, 0.1f, false);
	debug3->SetDisplayColor(glm::vec4(0.0f, 0.5f, 0.25f, 1.0f));
	renderer->AddObjectToScene(debug3);


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

	if (glewInit() != GLEW_OK) 
	{
		return false;
	}

	return true;
}

void Engine::Update() 
{
		dicomObjects->Update(renderer->half_aspect, renderer->vr_info, renderer->cursor_info);
		renderer->RenderScene();

		static float rot = 0.025f;
		static float t = 0.0f;
		if (glfwGetKey(window, GLFW_KEY_G)) {
			box->Set_model_orientation(glm::vec3(t, 0.0f, 0.0f));
			t += rot;
		}
		if (glfwGetKey(window, GLFW_KEY_H)) {
			box->Set_model_orientation(glm::vec3(t, 0.0f, 0.0f));
			t -= rot;
			
		}
//std::cout << (t*360.0f)/6.14 << std::endl;

		if (box->is_selected)
		{
			if (box->is_double_selected) 
			{
				//std::cout << "here" << std::endl;
				glm::mat4 rot = MiscFunctions::RotateTo(box->cache_vec[5], box->cache_vec[6]);

				debug1->Set_world_position(box->cache_vec[5]);
				debug2->Set_world_position(box->cache_vec[6]);
				debug3->Set_world_position(box->cache_vec[8]);

				//std::cout << box->cache_vec[4].x << " " << box->cache_vec[4].y << " " << box->cache_vec[4].z << std::endl;
				//std::cout << box->cache_vec[6].x << " " << box->cache_vec[6].y << " " << box->cache_vec[6].z << std::endl;
				//Render::PrintMat4(rot);
				//std::cout << std::endl;
				glm::mat4 curr_pose = box->cache_pose[2] *glm::translate(glm::mat4(1.0f), box->cache_vec[0]) *rot* glm::translate(glm::mat4(1.0f), -box->cache_vec[0]) * box->cache_pose[0];
				//curr_pose = glm::translate(glm::mat4(1.0f), -box->cache_vec[0]) * box->cache_pose[0];
				glm::vec3 tmp = glm::vec3(box->cache_pose[2] * glm::vec4(box->cache_vec[0],1.0f));
				curr_pose = glm::translate(glm::mat4(1.0f), tmp)* rot*glm::mat4(glm::mat3(box->cache_pose[2]))* glm::translate(glm::mat4(1.0f), -box->cache_vec[0]) * box->cache_pose[0];
				//curr_pose = glm::translate(glm::mat4(1.0f), tmp)* rot* glm::translate(glm::mat4(1.0f), -box->cache_vec[0]) * box->cache_pose[0];
				box->Set_append_pose(curr_pose);

			}
			else 
			{
				glm::mat4 curr_pose = box->cache_pose[1] * box->cache_pose[0];
				box->Set_append_pose(curr_pose);
			}
			//glm::mat4 curr_pose = box->cache_pose[1] * box->cache_pose[0];
			//box->Set_append_pose(curr_pose);
		}

}

void Engine::Loop() 
{
	glm::vec3 p1(0.0f,0.0f,-1.0f);
	glm::vec3 p2(1.0f,0.0f,-1.0f);
	glm::vec3 c1(0.0f,0.0f,0.0f);
	glm::vec3 c2(1.0f,0.0f,0.0f);

	glm::vec3 q1(0.0f, 0.0f, -1.0f);
	glm::vec3 q2(0.0f, 1.0f, -1.0f);
	glm::vec3 d1(0.0f, 0.0f, 0.0f);
	glm::vec3 d2(0.0f, 1.0f, 0.0f);

	glm::mat4 m;
	glm::column(m, 0, glm::vec4(p1, 1.0f));
	glm::column(m, 0, glm::vec4(p2, 1.0f));
	glm::column(m, 0, glm::vec4(c1, 1.0f));
	glm::column(m, 0, glm::vec4(c2, 1.0f));


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
