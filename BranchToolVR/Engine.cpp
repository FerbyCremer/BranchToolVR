#include "Engine.h"

Engine::Engine(){

	if (!InitGLFW() || !InitGLEW() || !InitVR()) {
		glfwTerminate();
		throw std::runtime_error("Failed to initialize Engine class. Exiting...");
	}
	
	// glew has been initialized
	renderer = new Render(window);

	// axis lines
	LineObject * l = new LineObject;
	l->GenerateAxis();
	renderer->AddObjectToScene(l);
	glLineWidth(12.0f);

	// TEMP: set dicom directory and panel values
	dicomObjects1 = new DicomObjectsContainer;
	dicomObjects1->AddObjects(renderer);
	dicomObjects1->Load(Constants::RELATIVE_DICOM_RESOURCE_PATH);	//"Resources\\DICOM\\torso\\DCM0"
	
	// scene
	ground = new ColorObject;
	ground->GenerateRoom();
	renderer->AddObjectToScene(ground);


	// call final render initializer after all objects have been added
	renderer->Finalize();

	Loop();
}

Engine::~Engine(){

}

bool Engine::InitGLFW() {

	// Initialise GLFW
	if (!glfwInit()) {
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window_size_x = DEFAULT_WINDOW_SIZE_X;
	window_size_y = DEFAULT_WINDOW_SIZE_Y;
	window = glfwCreateWindow(window_size_x, window_size_y, WINDOW_TITLE_STRING, NULL, NULL);

	if (window == NULL) {
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	return true;
}

bool Engine::InitGLEW() {

	glewExperimental = true;

	if (glewInit() != GLEW_OK) {
		return false;
	}

	return true;
}

bool Engine::InitVR() {

	return true;
}


void Engine::FakeControllerInput(float _deltaT) {

   
}

void Engine::Update() {

	//glm::vec4 p1 = controller_mm1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//glm::vec4 p2 = controller_mm1 * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	//glm::vec4 ray = p2 - p1;
	//
	//
	//dicomObjects1->dicom_panel->Interact(controller_mm1, glm::vec3(ray), glm::vec3(p1), controller_press1, true);
	//
	//// tab one
	//if (dicomObjects1->isovalue_slider->has_changed) {
	//	dicomObjects1->UpdateDicomPointCloud(dicomObjects1->isovalue_slider->curr);
	//	dicomObjects1->isovalue_slider->has_changed = false;
	//}
	//
	//if (dicomObjects1->isovalue_tol_slider->has_changed) {
	//	dicomObjects1->points->curr_tolerance = dicomObjects1->isovalue_tol_slider->curr;
	//	dicomObjects1->isovalue_tol_slider->has_changed = false;
	//}
	//
	//// tab two
	//if (dicomObjects1->scaleX_slider->has_changed) {
	//
	//	dicomObjects1->scaleX_slider->has_changed = false;
	//}
	//
	//if (dicomObjects1->scaleY_slider->has_changed) {
	//
	//	dicomObjects1->scaleY_slider->has_changed = false;
	//}
	//
	//if (dicomObjects1->scaleZ_slider->has_changed) {
	//
	//	dicomObjects1->scaleZ_slider->has_changed = false;
	//}
	//
	//// tab three
	//if (dicomObjects1->window_width_slider->has_changed) {
	//	dicomObjects1->viewer->orthoslice_texture->Load(dicomObjects1->imaging_data.data[150], dicomObjects1->window_width_slider->curr, dicomObjects1->window_center_slider->curr);
	//	dicomObjects1->window_width_slider->has_changed = false;
	//}
	//
	//if (dicomObjects1->window_center_slider->has_changed) {
	//	dicomObjects1->viewer->orthoslice_texture->Load(dicomObjects1->imaging_data.data[150], dicomObjects1->window_width_slider->curr, dicomObjects1->window_center_slider->curr);
	//	dicomObjects1->window_center_slider->has_changed = false;
	//}
	//
	//if (dicomObjects1->window_center_slider->has_changed) {
	//	dicomObjects1->viewer->orthoslice_texture->Load(dicomObjects1->imaging_data.data[150], dicomObjects1->window_width_slider->curr, dicomObjects1->window_center_slider->curr);
	//	dicomObjects1->window_center_slider->has_changed = false;
	//}


	//renderer->Interact(controller_mm1, controller_mm2, glm::vec3(ray), glm::vec3(p1), controller_press1);		
	//dicomObjects1->Update(controller_mm1, glm::vec3(ray), glm::vec3(p1), controller_press1);
}

void Engine::Loop() {


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

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
		
		begin = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(begin - end).count();
		

		dicomObjects1->Update(renderer->half_aspect, renderer->vr_info, renderer->cursor_info);
		renderer->RenderScene();
		
		//FakeControllerInput((float)ms);

		//int width, height;
		//glfwGetWindowSize(window, &width, &height);
		//glViewport(0, 0, width , height);
		////renderer->RenderSceneInternal(glm::perspective(90.0f, (float)width/(float)height, 0.001f, 1000.0f), glm::inverse(controller_mm1));
		//renderer->RenderUI(glm::perspective(90.0f, (float)width/(float)height, 0.001f, 1000.0f), glm::inverse(controller_mm1));
		
		
		
		//renderer->controller_pose1 = controller_mm1;
		//renderer->controller_press1 = controller_press1;

		//int width, height;
		//glfwGetWindowSize(window, &width, &height);
		//glViewport(width / 2, 0, width/2, height);
		//glScissor(width / 2, 0, width/2, height);
		//glEnable(GL_SCISSOR_TEST);
		//renderer->RenderSceneInternal(glm::perspective(90.0f, 1.0f, 0.1f, 1000.0f), glm::inverse(controller_mm1));

		glfwSwapBuffers(window);
		glfwPollEvents();		

		end = begin;
	} 

	glfwTerminate();
}
