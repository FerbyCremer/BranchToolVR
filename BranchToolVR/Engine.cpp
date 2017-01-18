#include "Engine.h"

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

	ImGui_ImplGlfwGL3_Init(window, true);
	bool show_test_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImColor(114, 144, 154);

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
}

void Engine::Loop() 
{
	auto begin = std::chrono::high_resolution_clock::now();
	auto end = std::chrono::high_resolution_clock::now();

	ColorObject* debug1 = new ColorObject();
	debug1->GenerateSphere(10, 0.001f, false);
	renderer->AddObjectToScene(debug1);


	bool show_test_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImColor(114, 144, 154);

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) 
	{
		begin = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(begin - end).count();
		

		if (dicomObjects->imgui_panel->is_selected)
		{
			//glm::vec4 colp_to_model_space = glm::inverse(dicomObjects->imgui_panel->GetModelMatrix()) * glm::vec4(dicomObjects->imgui_panel->cache.primary_collision_point_world_current, 1.0f);
			//dicomObjects->imgui_panel_coords = glm::vec2(colp_to_model_space);
			//debug1->Set_world_position(glm::vec3(dicomObjects->imgui_panel->cache.primary_collision_point_world_current));

			// find collision with controller ray and xy plane (imgui panel model space)
			glm::vec3 curr_pos = glm::vec3(glm::inverse(dicomObjects->imgui_panel->GetModelMatrix())*dicomObjects->imgui_panel->cache.controller_pose_updated* glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
			glm::vec3 curr_ray = glm::vec3(glm::inverse(dicomObjects->imgui_panel->GetModelMatrix())*dicomObjects->imgui_panel->cache.controller_pose_updated* glm::vec4(0.0f, 0.0f, -1.0f, 1.0f)) - curr_pos;
			float t = (-curr_pos.z) / curr_ray.z;
			glm::vec3 xy_plane_intersection = curr_pos + curr_ray*t;

			dicomObjects->imgui_panel_coords = glm::vec2(xy_plane_intersection) + glm::vec2(0.001f,0.0001f);
			debug1->Set_world_position(glm::vec3(dicomObjects->imgui_panel->GetModelMatrix()*glm::vec4(xy_plane_intersection,1.0f)));
		}

		
		
		glfwPollEvents();


		ImGui_ImplGlfwGL3_NewFrame(renderer->vr_info, dicomObjects->imgui_panel_coords, dicomObjects->imgui_panel->is_selected);
		Update();

		// 1. Show a simple window
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
		{
			static float f = 0.0f;
			ImGui::Text("Hello, world!");
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
			ImGui::ColorEdit3("clear color", (float*)&clear_color);
			if (ImGui::Button("Test Window")) show_test_window ^= 1;
			if (ImGui::Button("Another Window")) show_another_window ^= 1;
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		
		// 2. Show another simple window, this time using an explicit Begin/End pair
		if (show_another_window)
		{
			ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
			ImGui::Begin("Another Window", &show_another_window);
			ImGui::Text("Hello");
			ImGui::End();
		}
		
		// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
		if (show_test_window)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
			ImGui::ShowTestWindow(&show_test_window);
		}
		
		// Rendering
		glBindFramebuffer(GL_FRAMEBUFFER, rt->fbo);
		glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)));
		glClear(GL_DEPTH_BUFFER_BIT);
		ImGui::Render();


		glfwSwapBuffers(window);


		end = begin;
	} 

	glfwTerminate();
}
