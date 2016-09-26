#include "Engine.h"

Engine::Engine(){

	if (!InitGLFW() || !InitGLEW() || !InitVR()) {
		glfwTerminate();
		throw std::runtime_error("Failed to initialize Engine class. Exiting...");
	}
	
	// glew has been initialized
	renderer = new Render(window);
	nonVR_camera = new Camera;

	// axis lines
	LineObject * l = new LineObject;
	l->GenerateAxis();
	renderer->AddObjectToScene(l);
	glLineWidth(12.0f);

	// dicom panel
	dicom_panel.GenerateDicomPanel(renderer);
	//dicom_panel.addToRenderer();
	//renderer->AddObjectToScene(dicom_panel.GetObjects());
	dicom_panel.SetWorldPosition(glm::vec3(0.0f, 0.0f, -1.0f));
	dicom_panel.SetModelOrientation(glm::vec3(0.8f, 0.2f, 0.2f));
	isovalue_slider = dicom_panel.GetSliderByName("isovalue");
	isovalue_tol_slider = dicom_panel.GetSliderByName("isovalue tolerance");
	scaleX_slider = dicom_panel.GetSliderByName("X");
	scaleY_slider = dicom_panel.GetSliderByName("Y");
	scaleZ_slider = dicom_panel.GetSliderByName("Z");
	window_width_slider = dicom_panel.GetSliderByName("window width");
	window_center_slider = dicom_panel.GetSliderByName("window center");
	clear_branching_slider = dicom_panel.GetSliderByName("reset selection");

	// TEMP: set dicom directory and panel values
	dicomObjects1 = new DicomObjectsContainer;
	dicomObjects1->AddObjects(renderer);
	dicomObjects1->Load("Resources\\DICOM\\torso\\DCM0");

	// scene
	ground = new ColorObject;
	ground->GenerateRoom();
	renderer->AddObjectToScene(ground);

	// controller
	controller = new ColorObject;
	controller->GenerateController();
	controller->world_position = glm::vec3(0.2f, 0.2f, 0.2f);
	controller->SetDisplayColor(glm::vec4(0.1f, 0.1f, 1.0f,1.0f));
	//renderer->AddObjectToScene(controller);

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

    
	float move_rate = 0.025f;
	float rot_rate =  0.025f;
	float delta_time_factor = (_deltaT+1.0f)/16.67f;
	move_rate *= delta_time_factor;
	//rot_rate *= delta_time_factor;


	glm::vec4 p1 = controller_mm1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 p2 = controller_mm1 * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	glm::vec3 ray = glm::vec3(p2 - p1);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		controller->world_position += move_rate*ray;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		controller->world_position.x -= move_rate;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		controller->world_position -= move_rate*ray;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		controller->world_position.x += move_rate;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		renderer->ResetSeatedPose();
	}

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	glm::vec2 mpos;
	mpos.x = width / 4 - xpos;
	mpos.y = height / 4 - ypos; 

	int focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);

	if (focused) {
		glfwSetCursorPos(window, width/4, height/4);
		controller->model_orientation.x += rot_rate*mpos.x;
		controller->model_orientation.y += glm::clamp(rot_rate*mpos.y, -1.2f, 1.2f);
		controller->CalcModelMatrix();
		controller_mm1 = controller->GetModelMatrix();
		controller_press1 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		controller->SetSelected(controller_press1);	
	}
	else {

	}
}

void Engine::Update() {

	glm::vec4 p1 = controller_mm1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 p2 = controller_mm1 * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	glm::vec4 ray = p2 - p1;
	

	dicom_panel.Interact(controller_mm1, glm::vec3(ray), glm::vec3(p1), controller_press1);
	
	// tab one
	if (isovalue_slider->has_changed) {
		dicomObjects1->UpdateDicomPointCloud(isovalue_slider->curr);
		isovalue_slider->has_changed = false;
	}

	if (isovalue_tol_slider->has_changed) {
		dicomObjects1->points->curr_tolerance = isovalue_tol_slider->curr;
		isovalue_tol_slider->has_changed = false;
	}

	// tab two
	if (scaleX_slider->has_changed) {

		scaleX_slider->has_changed = false;
	}

	if (scaleY_slider->has_changed) {

		scaleY_slider->has_changed = false;
	}

	if (scaleZ_slider->has_changed) {

		scaleZ_slider->has_changed = false;
	}
	
	// tab three
	if (window_width_slider->has_changed) {
		dicomObjects1->viewer->orthoslice_texture->Load(dicomObjects1->imaging_data.data[150], window_width_slider->curr, window_center_slider->curr);
		window_width_slider->has_changed = false;
	}

	if (window_center_slider->has_changed) {
		dicomObjects1->viewer->orthoslice_texture->Load(dicomObjects1->imaging_data.data[150], window_width_slider->curr, window_center_slider->curr);
		window_center_slider->has_changed = false;
	}

	if (window_center_slider->has_changed) {
		dicomObjects1->viewer->orthoslice_texture->Load(dicomObjects1->imaging_data.data[150], window_width_slider->curr, window_center_slider->curr);
		window_center_slider->has_changed = false;
	}


	renderer->Interact(controller_mm1, controller_mm2, glm::vec3(ray), glm::vec3(p1), controller_press1);		
	dicomObjects1->Update(controller_mm1, glm::vec3(ray), glm::vec3(p1), controller_press1);
}

void Engine::Loop() {

	auto begin = std::chrono::high_resolution_clock::now();
	auto end = std::chrono::high_resolution_clock::now();

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
		
		begin = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(begin - end).count();
		
		renderer->RenderScene();
		Update();


		FakeControllerInput((float)ms);

		//int width, height;
		//glfwGetWindowSize(window, &width, &height);
		//glViewport(0, 0, width , height);
		////renderer->RenderSceneInternal(glm::perspective(90.0f, (float)width/(float)height, 0.001f, 1000.0f), glm::inverse(controller_mm1));
		//renderer->RenderUI(glm::perspective(90.0f, (float)width/(float)height, 0.001f, 1000.0f), glm::inverse(controller_mm1));
		
		
		
		renderer->controller_pose1 = controller_mm1;
		renderer->controller_press1 = controller_press1;

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
