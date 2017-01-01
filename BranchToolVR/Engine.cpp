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


	bool show_test_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImColor(114, 144, 154);

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) 
	{
		begin = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(begin - end).count();
		
		
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();
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
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);
		ImGui::Render();


		glfwSwapBuffers(window);


		end = begin;
	} 

	glfwTerminate();
}
