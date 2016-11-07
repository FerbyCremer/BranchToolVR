#include "Render.h"

// static variables
int Render::window_size_x = 0;
int Render::window_size_y = 0;
int Render::half_window_size_x = 0;
int Render::half_window_size_y = 0;
float Render::aspect = 1.0f;
float Render::half_aspect = 1.0f;
glm::vec4 Render::ui_quadrant_ortho[4];
glm::vec4 Render::ui_quadrant_ortho_aspect[4];
VrData Render::vr_info;
CursorData Render::cursor_info;
glm::mat4 Render::ui_view;
glm::mat4 Render::ui_projection;

Render::Render(GLFWwindow *_window){

	// configure glfw vars
	window = _window;
	glfwSetWindowSizeCallback(window, Render::window_size_callback);

	// set clears
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	glClearDepthf(1.0f);

	// enable alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	// culling
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);

	// ui
	glEnable(GL_SCISSOR_TEST);

	// controller
	controller1 = new ColorObject;
	controller1->GenerateController();
	AddObjectToScene(controller1);

	controller2 = new ColorObject;
	controller2->GenerateController();
	AddObjectToScene(controller2);
	controller2->SetDisplayColor(glm::vec4(0.5f, 0.25f, 0.1f, 1.0f));

	// shadow
	createFrameBuffer(sm);
	float shadow_width = 5.0f;
	sm.P = glm::ortho(-shadow_width, shadow_width, -shadow_width, shadow_width, -100.0f, 100.0f);
	sm.V = glm::lookAt(glm::vec3(2.5f,20.0f,2.5f),glm::vec3(-0.1f,0.0f,-0.1f),glm::vec3(0.0f,1.0f,0.0f));

	// initializers

	InitVR();

	std::string shader_directory = "./Shaders/";

	color.id = CompileGLShader("color", shader_directory);
	color.num_uniforms = 11;
	color.uniforms = new GLuint[color.num_uniforms];
	color.uniforms[0] = glGetUniformLocation(color.id, "P");
	color.uniforms[1] = glGetUniformLocation(color.id, "V");
	color.uniforms[2] = glGetUniformLocation(color.id, "M");
	color.uniforms[3] = glGetUniformLocation(color.id, "color");
	color.uniforms[4] = glGetUniformLocation(color.id, "lights[0].pos"); // change to uniform buffer
	color.uniforms[5] = glGetUniformLocation(color.id, "lights[1].pos");
	color.uniforms[6] = glGetUniformLocation(color.id, "lights[2].pos");
	color.uniforms[7] = glGetUniformLocation(color.id, "shadow");
	color.uniforms[8] = glGetUniformLocation(color.id, "shadowP");
	color.uniforms[9] = glGetUniformLocation(color.id, "shadowV");
	color.uniforms[10] = glGetUniformLocation(color.id, "ambient");

	texture.id = CompileGLShader("texture", shader_directory);
	texture.num_uniforms = 5;
	texture.uniforms = new GLuint[texture.num_uniforms];
	texture.uniforms[0] = glGetUniformLocation(texture.id, "P");
	texture.uniforms[1] = glGetUniformLocation(texture.id, "V");
	texture.uniforms[2] = glGetUniformLocation(texture.id, "M");
	texture.uniforms[3] = glGetUniformLocation(texture.id, "diffuse_texture");
	texture.uniforms[4] = glGetUniformLocation(texture.id, "ambient");

	line.id = CompileGLShader("line", shader_directory);
	line.num_uniforms = 3;
	line.uniforms = new GLuint[color.num_uniforms];
	line.uniforms[0] = glGetUniformLocation(line.id, "P");
	line.uniforms[1] = glGetUniformLocation(line.id, "V");
	line.uniforms[2] = glGetUniformLocation(line.id, "M");

	dicom_point_cloud.id = CompileGLShader("dicom_point_cloud", shader_directory);
	dicom_point_cloud.num_uniforms = 14;
	dicom_point_cloud.uniforms = new GLuint[dicom_point_cloud.num_uniforms];
	dicom_point_cloud.uniforms[0] = glGetUniformLocation(dicom_point_cloud.id, "P");
	dicom_point_cloud.uniforms[1] = glGetUniformLocation(dicom_point_cloud.id, "V");
	dicom_point_cloud.uniforms[2] = glGetUniformLocation(dicom_point_cloud.id, "M");
	dicom_point_cloud.uniforms[3] = glGetUniformLocation(dicom_point_cloud.id, "tolerance");
	dicom_point_cloud.uniforms[4] = glGetUniformLocation(dicom_point_cloud.id, "lights[0].pos"); // change to uniform buffer
	dicom_point_cloud.uniforms[5] = glGetUniformLocation(dicom_point_cloud.id, "lights[1].pos");
	dicom_point_cloud.uniforms[6] = glGetUniformLocation(dicom_point_cloud.id, "lights[2].pos");
	dicom_point_cloud.uniforms[7] = glGetUniformLocation(dicom_point_cloud.id, "scale");
	dicom_point_cloud.uniforms[8] = glGetUniformLocation(dicom_point_cloud.id, "lower_bounds");
	dicom_point_cloud.uniforms[9] = glGetUniformLocation(dicom_point_cloud.id, "upper_bounds");
	dicom_point_cloud.uniforms[10] = glGetUniformLocation(dicom_point_cloud.id, "box_scale");
	dicom_point_cloud.uniforms[11] = glGetUniformLocation(dicom_point_cloud.id, "scale");
	dicom_point_cloud.uniforms[12] = glGetUniformLocation(dicom_point_cloud.id, "eye_pos");
	dicom_point_cloud.uniforms[13] = glGetUniformLocation(dicom_point_cloud.id, "ambient");

	branch_point.id = CompileGLShader("branch_point", shader_directory);
	branch_point.num_uniforms = 7;
	branch_point.uniforms = new GLuint[branch_point.num_uniforms];
	branch_point.uniforms[0] = glGetUniformLocation(branch_point.id, "P");
	branch_point.uniforms[1] = glGetUniformLocation(branch_point.id, "V");
	branch_point.uniforms[2] = glGetUniformLocation(branch_point.id, "M");
	branch_point.uniforms[3] = glGetUniformLocation(branch_point.id, "instanced_position");
	branch_point.uniforms[4] = glGetUniformLocation(branch_point.id, "lower_bounds"); 
	branch_point.uniforms[5] = glGetUniformLocation(branch_point.id, "scale");
	branch_point.uniforms[6] = glGetUniformLocation(branch_point.id, "color");

	branch_line.id = CompileGLShader("branch_line", shader_directory);
	branch_line.num_uniforms = 10;
	branch_line.uniforms = new GLuint[branch_line.num_uniforms];
	branch_line.uniforms[0] = glGetUniformLocation(branch_line.id, "P");
	branch_line.uniforms[1] = glGetUniformLocation(branch_line.id, "V");
	branch_line.uniforms[2] = glGetUniformLocation(branch_line.id, "M");
	branch_line.uniforms[3] = glGetUniformLocation(branch_line.id, "lower_bounds");
	branch_line.uniforms[4] = glGetUniformLocation(branch_line.id, "scale");
	branch_line.uniforms[5] = glGetUniformLocation(branch_line.id, "pos1");
	branch_line.uniforms[6] = glGetUniformLocation(branch_line.id, "pos2");
	branch_line.uniforms[7] = glGetUniformLocation(branch_line.id, "lights[0].pos"); // change to uniform buffer
	branch_line.uniforms[8] = glGetUniformLocation(branch_line.id, "lights[1].pos");
	branch_line.uniforms[9] = glGetUniformLocation(branch_line.id, "lights[2].pos");

	ui_texture.id = CompileGLShader("ui_texture", shader_directory);
	ui_texture.num_uniforms = 4;
	ui_texture.uniforms = new GLuint[branch_line.num_uniforms];
	ui_texture.uniforms[0] = glGetUniformLocation(ui_texture.id, "P");
	ui_texture.uniforms[1] = glGetUniformLocation(ui_texture.id, "V");
	ui_texture.uniforms[2] = glGetUniformLocation(ui_texture.id, "M");
	ui_texture.uniforms[3] = glGetUniformLocation(ui_texture.id, "diffuse_texture");

	ui_color.id = CompileGLShader("ui_color", shader_directory);
	ui_color.num_uniforms = 4;
	ui_color.uniforms = new GLuint[branch_line.num_uniforms];
	ui_color.uniforms[0] = glGetUniformLocation(ui_color.id, "P");
	ui_color.uniforms[1] = glGetUniformLocation(ui_color.id, "V");
	ui_color.uniforms[2] = glGetUniformLocation(ui_color.id, "M");
	ui_color.uniforms[3] = glGetUniformLocation(ui_color.id, "color");

	shadow.id = CompileGLShader("shadow", shader_directory);
	shadow.num_uniforms = 3;
	shadow.uniforms = new GLuint[branch_line.num_uniforms];
	shadow.uniforms[0] = glGetUniformLocation(shadow.id, "P");
	shadow.uniforms[1] = glGetUniformLocation(shadow.id, "V");
	shadow.uniforms[2] = glGetUniformLocation(shadow.id, "M");

	// load textures
	textures = new Texture*[CURR_NR_TEXTURES];
	for (int i = 0; i < CURR_NR_TEXTURES - 1; ++i) {
		textures[i] = new Texture;
	}
	textures[WOOD_TEXTURE]->Load("wood");
	textures[FONT_TEXTURE]->Load("fontGlyph4096");

	// set up lights
	num_lights = 3; // must be same as shader declarations
	lights = new Light[num_lights];
	for (int i = 0; i < num_lights; ++i) {
		lights[i].marker.is_selectable = true;
		lights[i].marker.SetDisplayColor(glm::vec4(1.0f, 0.8f, 0.3f,1.0f));
		AddObjectToScene(&lights[i].marker);
	}		

	lights[0].position = glm::vec3(0.0f, 0.5f, 0.0f);
	lights[0].marker.Set_world_position(lights[0].position);

	lights[1].position = glm::vec3(2.0f, 1.0f, 0.0f);
	lights[1].marker.Set_world_position(lights[1].position);

	lights[2].position = glm::vec3(0.0f, 1.0f, 2.0f);
	lights[2].marker.Set_world_position(lights[2].position);

	glLineWidth(2.0f);

}

Render::~Render(){

}

void Render::Finalize() {
	// initialize glfw window variables
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	window_size_callback(window, width, height);
}

void Render::window_size_callback(GLFWwindow* window, int width, int height){
	window_size_x = width;
	window_size_y = height;
	half_window_size_x = window_size_x / 2;
	half_window_size_y = window_size_y / 2;
	aspect = (float)window_size_x / (float)window_size_y;
	half_aspect = (float)half_window_size_x / (float)half_window_size_y;
	float inverse_half_aspect = (float)half_window_size_y / (float)half_window_size_x;

	float ui_aspect = (float)half_window_size_x / (float)window_size_y;
	ui_projection = glm::perspective(90.0f, ui_aspect, 0.001f, 1000.0f);
	ui_view = glm::mat4(1.0f);

	float tmpX0 = ui_quadrant_ortho[0].x * half_aspect;
	float tmpY0 = ui_quadrant_ortho[0].x * inverse_half_aspect;

	float tmpX1 = ui_quadrant_ortho[1].x * half_aspect;
	float tmpY1 = ui_quadrant_ortho[1].x * inverse_half_aspect;
	
	float tmpX00 = ui_quadrant_ortho[0].y * half_aspect;

	if (tmpY0 < ui_quadrant_ortho[0].y) {
		ui_quadrant_ortho_aspect[0] = glm::vec4(-tmpX00, tmpX00, -ui_quadrant_ortho[0].y, ui_quadrant_ortho[0].y);
	}
	else {
		ui_quadrant_ortho_aspect[0] = glm::vec4(-ui_quadrant_ortho[0].x, ui_quadrant_ortho[0].x, -tmpY0, tmpY0);
	}

	float tmpX11 = ui_quadrant_ortho[1].y * half_aspect;

	if (tmpY1 < ui_quadrant_ortho[1].y) {
		ui_quadrant_ortho_aspect[1] = glm::vec4(-tmpX11, tmpX11, -ui_quadrant_ortho[1].y, ui_quadrant_ortho[1].y);
	}
	else {
		ui_quadrant_ortho_aspect[1] = glm::vec4(-ui_quadrant_ortho[1].x, ui_quadrant_ortho[1].x, -tmpY1, tmpY1);
	}
}

bool Render::InitVR() {

	// Loading the SteamVR Runtime
	vr::EVRInitError eError = vr::VRInitError_None;
	m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

	if (eError != vr::VRInitError_None){
		m_pHMD = NULL;
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		vr_info.hmd_connected = false;
		controller1->is_hidden = true;
		return false;
	}

	//m_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError);
	//if (!m_pRenderModels){
	//	m_pHMD = NULL;
	//	vr::VR_Shutdown();
	//	char buf[1024];
	//	sprintf_s(buf, sizeof(buf), "Unable to get render model interface: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
	//	return false;
	//}

	m_pHMD->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);

	CreateFrameBuffer(m_nRenderWidth, m_nRenderHeight, leftEyeDesc);
	CreateFrameBuffer(m_nRenderWidth, m_nRenderHeight, rightEyeDesc);
	
	vr_info.hmd_connected = true;
	return true;
}

void Render::AddObjectToScene(std::vector<AbstractBaseObject*> cObjs) {
	for (AbstractBaseObject* co : cObjs) {
		AddObjectToScene(co);
	}
}

void Render::AddObjectToScene(AbstractBaseObject* abso) {
	if (abso == NULL) {
		return;
	}
	if (abso->Type() == 0) {
		ColorObject * s = static_cast<ColorObject*>(abso);
		AddObjectToScene(s);
	}
	if (abso->Type() == 1) {
		TextureObject * s = static_cast<TextureObject*>(abso);
		AddObjectToScene(s);
	}
}

void Render::AddObjectToUi(AbstractBaseObject* abso) {
	if (abso == NULL) {
		return;
	}
	if (abso->Type() == 0) {
		ColorObject * s = static_cast<ColorObject*>(abso);
		AddObjectToUi(s);
	}
	if (abso->Type() == 1) {
		TextureObject * s = static_cast<TextureObject*>(abso);
		AddObjectToUi(s);
	}
}

void Render::AddObjectToScene(ColorObject * co) {
	if (co != NULL) {

		color_objects.push_back(co);
	}
}

void Render::AddObjectToScene(DicomPointCloudObject * dpco) {
	if (dpco != NULL) {
		dicom_point_cloud_objects.push_back(dpco);
		if (dpco->handle != NULL)
		color_objects.push_back(dpco->handle);
	}
}

void Render::AddObjectToScene(LineObject * l) {
	if (l != NULL)
		line_objects.push_back(l);
}

void Render::AddObjectToScene(TextureObject * t) {
	if (t != NULL)
		texture_objects.push_back(t);
}

void Render::AddObjectToUi(TextureObject * t) {
	if (t != NULL)
		texture_ui_elements.push_back(t);
}

void Render::AddObjectToUi(ColorObject * c) {
	if (c != NULL)
		color_ui_elements.push_back(c);
}

void Render::RenderEyes() {
	if (!m_pHMD)
		return;

	// left eye
	glBindFramebuffer(GL_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
	glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
	RenderSceneInternal(vr_info.left_eye_proj, vr_info.right_eye_transform_inv);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_MULTISAMPLE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, leftEyeDesc.m_nResolveFramebufferId);

	glBlitFramebuffer(0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight,GL_COLOR_BUFFER_BIT,GL_LINEAR);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
	// right eye
	glBindFramebuffer(GL_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId);
	glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
	RenderSceneInternal(vr_info.right_eye_proj, vr_info.right_eye_transform_inv);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_MULTISAMPLE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rightEyeDesc.m_nResolveFramebufferId);

	glBlitFramebuffer(0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	vr::Texture_t leftEyeTexture = { (void*)leftEyeDesc.m_nResolveTextureId, vr::API_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
	vr::Texture_t rightEyeTexture = { (void*)rightEyeDesc.m_nResolveTextureId, vr::API_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
}

void Render::ResetSeatedPose() {
	m_pHMD->ResetSeatedZeroPose();
}

void Render::UpdateLights() {
	for (int i = 0; i < num_lights; ++i) {
		if (lights[i].marker.is_selected) {
			lights[i].marker.Set_append_pose(lights[i].marker.cache_pose[1] * lights[i].marker.cache_pose[0]);
			lights[i].position = glm::vec3(lights[i].marker.GetModelMatrix()*glm::vec4(0.0f,0.0f,0.0f,1.0f));
		}
	}
}

void Render::UpdateCursor() {
	static bool fp = true;
	if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_1)) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		float nx = 2.0f*xpos / (float)window_size_x - 1.0f;
		float ny = 2.0f*(window_size_y - ypos) / (float)window_size_y - 1.0f;
		
		cursor_info.normalized_cursor_position = glm::vec2(nx, ny);
		cursor_info.is_pressed = true;
		
		if (fp) {
			cursor_info.first_press = true;
			fp = false;
		}
		else {
			cursor_info.first_press = false;
		}
	}
	else {
		fp = true;
		cursor_info.first_press = false;
		cursor_info.is_pressed = false;
	}
}

void Render::RenderShadows() {

	glBindFramebuffer(GL_FRAMEBUFFER, sm.depth);
	glDisable(GL_SCISSOR_TEST);

	glViewport(0, 0, DEFAULT_SHADOW_RES, DEFAULT_SHADOW_RES);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(shadow.id);
	glUniformMatrix4fv(shadow.uniforms[0], 1, GL_FALSE, glm::value_ptr(sm.P));
	glUniformMatrix4fv(shadow.uniforms[1], 1, GL_FALSE, glm::value_ptr(sm.V));

	for (ColorObject* & cObj : color_objects) {
		if (cObj->is_hidden || !cObj->is_loaded)
			continue;

		glUniformMatrix4fv(shadow.uniforms[2], 1, GL_FALSE, glm::value_ptr(cObj->GetModelMatrix()));

		glBindVertexArray(cObj->vao);
		glEnableVertexAttribArray(0);

		glDrawArrays(GL_TRIANGLES, 0, cObj->num_vertices);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	for (TextureObject* & tObj : texture_objects) {
		if (tObj->is_hidden || !tObj->is_loaded)
			continue;

		glUniformMatrix4fv(shadow.uniforms[2], 1, GL_FALSE, glm::value_ptr(tObj->GetModelMatrix()));

		glBindVertexArray(tObj->vao);
		glEnableVertexAttribArray(0);

		glDrawArrays(GL_TRIANGLES, 0, tObj->num_vertices);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	glUseProgram(dicom_point_cloud.id);

	glUniformMatrix4fv(dicom_point_cloud.uniforms[0], 1, GL_FALSE, glm::value_ptr(sm.P));
	glUniformMatrix4fv(dicom_point_cloud.uniforms[1], 1, GL_FALSE, glm::value_ptr(sm.V));

	//glm::vec4 head_pos = glm::inverse(m_mat4HMDPose) * glm::vec4(0.01f, 0.05f, 0.0f, 1.0f);
	glm::vec4 head_pos = controller_pose1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	glUniform3fv(dicom_point_cloud.uniforms[4], 1, glm::value_ptr(lights[0].position));
	glUniform3fv(dicom_point_cloud.uniforms[5], 1, glm::value_ptr(lights[1].position));
	glUniform3fv(dicom_point_cloud.uniforms[6], 1, glm::value_ptr(lights[2].position));
	glUniform3fv(dicom_point_cloud.uniforms[12], 1, glm::value_ptr(head_pos));

	for (DicomPointCloudObject* & dpco : dicom_point_cloud_objects) {

		if (!dpco->is_loaded)
			continue;

		glUniformMatrix4fv(dicom_point_cloud.uniforms[2], 1, GL_FALSE, glm::value_ptr(dpco->GetModelMatrix()));
		glUniform1i(dicom_point_cloud.uniforms[3], dpco->curr_tolerance);
		glUniform3fv(dicom_point_cloud.uniforms[7], 1, glm::value_ptr(dpco->scale));
		glUniform3fv(dicom_point_cloud.uniforms[8], 1, glm::value_ptr(dpco->lower_bounds));
		glUniform3fv(dicom_point_cloud.uniforms[9], 1, glm::value_ptr(dpco->upper_bounds));
		glUniform3fv(dicom_point_cloud.uniforms[10], 1, glm::value_ptr(dpco->box_scale));

		glBindVertexArray(dpco->vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glVertexAttribDivisor(0, 0);
		glVertexAttribDivisor(1, 0);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);

		glDrawArraysInstanced(GL_TRIANGLES, 0, dpco->num_vertices, dpco->num_instances);

	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_SCISSOR_TEST);
}

void Render::RenderScene() {

	// update state
	UpdateCursor();
	UpdateLights();
	RenderShadows();
	Interact(vr_info.controller_pose, glm::mat4(1.0f),glm::vec3(vr_info.controller_pose* glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)), glm::vec3(vr_info.controller_pose* glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), vr_info.controller_press);

	glEnable(GL_SCISSOR_TEST);
	// ui
	RenderUI(0);
	RenderUI(1);
	glDisable(GL_SCISSOR_TEST);


	// vr
	RenderEyes();
	UpdateHMDMatrixPose();
}

void Render::RenderUI(int level) {

	if (level == 0) {
		// clear and render HMD view, or if no hmd is connected, show controllers perspective
		glViewport(0, 0, half_window_size_x, window_size_y);
		glScissor(0, 0, half_window_size_x, window_size_y);
		if (vr_info.hmd_connected) {
			RenderSceneInternal(glm::perspective(90.0f,1.0f,0.1f,100.0f), vr_info.head_pose_inv);
		}
		else {
			RenderSceneInternal(glm::perspective(90.0f, aspect*0.5f, 0.1f, 100.0f), glm::inverse(controller1->GetModelMatrix()));
		}

		// clear ui quadrant 0
		glViewport(half_window_size_x, 0,  half_window_size_x, window_size_y);
		glScissor(half_window_size_x, 0, half_window_size_x, window_size_y);
		glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)));
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// clear ui quadrant 1
		glViewport(half_window_size_x, half_window_size_y, half_window_size_x, window_size_y);
		glScissor(half_window_size_x, half_window_size_y, half_window_size_x, window_size_y);
		glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f)));
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
	}

	// TODO: remove unecessary uniform and viewport calls
	glm::mat4 _P = glm::mat4(1.0f);
	glm::mat4 _V = glm::mat4(1.0f);

	//////////////////////////////////////////////////////////////////////////////////////////////////

	glUseProgram(ui_color.id);

	glUniformMatrix4fv(ui_color.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));

	for (ColorObject* & cUiObj : color_ui_elements) {
		if (cUiObj->is_hidden || !cUiObj->is_loaded || cUiObj->level != level)
			continue;

		if (cUiObj->ui_quadrant == 0) {
			_P = glm::ortho(ui_quadrant_ortho_aspect[0].x, ui_quadrant_ortho_aspect[0].y, ui_quadrant_ortho_aspect[0].z, ui_quadrant_ortho_aspect[0].w, -100.0f, 100.0f);
			glUniformMatrix4fv(ui_color.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
			glViewport(half_window_size_x, 0, half_window_size_x, half_window_size_y);
			glScissor(half_window_size_x, 0, half_window_size_x, half_window_size_y);
		}
		else if (cUiObj->ui_quadrant == 1) {
			_P = glm::ortho(ui_quadrant_ortho_aspect[1].x, ui_quadrant_ortho_aspect[1].y, ui_quadrant_ortho_aspect[1].z, ui_quadrant_ortho_aspect[1].w, -100.0f, 100.0f);
			glUniformMatrix4fv(ui_color.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
			glViewport(half_window_size_x, half_window_size_y, half_window_size_x, half_window_size_y);
			glScissor(half_window_size_x, half_window_size_y, half_window_size_x, half_window_size_y);
		}

		glUniformMatrix4fv(ui_color.uniforms[2], 1, GL_FALSE, glm::value_ptr(cUiObj->ui_model_matrix));
		glUniform4fv(ui_color.uniforms[3], 1, glm::value_ptr(cUiObj->GetDisplayColor()));

		glBindVertexArray(cUiObj->vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_TRIANGLES, 0, cUiObj->num_vertices);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	glUseProgram(ui_texture.id);

	for (int i = 0; i < CURR_NR_TEXTURES; ++i) {
		textures[i]->Bind();
	}

	glUniformMatrix4fv(ui_texture.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));

	for (TextureObject* & tUiObj : texture_ui_elements) {
		if (tUiObj->is_hidden || !tUiObj->is_loaded || tUiObj->level != level)
			continue;

		if (tUiObj->ui_quadrant == 0) {
			_P = glm::ortho(ui_quadrant_ortho_aspect[0].x, ui_quadrant_ortho_aspect[0].y, ui_quadrant_ortho_aspect[0].z, ui_quadrant_ortho_aspect[0].w, -100.0f, 100.0f);
			glUniformMatrix4fv(ui_texture.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
			glViewport(half_window_size_x, 0, half_window_size_x, half_window_size_y);
			glScissor(half_window_size_x, 0, half_window_size_x, half_window_size_y);
		}
		else if (tUiObj->ui_quadrant == 1) {
			_P = glm::ortho(ui_quadrant_ortho_aspect[1].x, ui_quadrant_ortho_aspect[1].y, ui_quadrant_ortho_aspect[1].z, ui_quadrant_ortho_aspect[1].w, -100.0f, 100.0f);
			glUniformMatrix4fv(ui_texture.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
			glViewport(half_window_size_x, half_window_size_y, half_window_size_x, half_window_size_y);
			glScissor(half_window_size_x, half_window_size_y, half_window_size_x, half_window_size_y);
		}

		glUniformMatrix4fv(ui_texture.uniforms[2], 1, GL_FALSE, glm::value_ptr(tUiObj->ui_model_matrix));
		glUniform1i(ui_texture.uniforms[3], tUiObj->texture_id);

		glBindVertexArray(tUiObj->vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glDrawArrays(GL_TRIANGLES, 0, tUiObj->num_vertices);
	}
}

void Render::RenderSceneInternal(glm::mat4 _P, glm::mat4 _V) {

	if (glfwGetKey(window, GLFW_KEY_Z)) {
		_P = sm.P;
		_V = sm.V;
	}


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	////////////////////////////////////////////////////////////////////////////////////////////////
	
	glUseProgram(line.id);
	
	glUniformMatrix4fv(line.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
	glUniformMatrix4fv(line.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));
	
	for (LineObject* & l : line_objects) {

		glUniformMatrix4fv(line.uniforms[2], 1, GL_FALSE, glm::value_ptr(glm::mat4()));
		
		glBindVertexArray(l->vao);	
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		
		glDrawArrays(GL_LINES, 0, l->num_vertices);
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	
	glUseProgram(color.id);
	
	glUniformMatrix4fv(color.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
	glUniformMatrix4fv(color.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));

	glUniform3fv(color.uniforms[4], 1, glm::value_ptr(lights[0].position));
	glUniform3fv(color.uniforms[5], 1, glm::value_ptr(lights[1].position));
	glUniform3fv(color.uniforms[6], 1, glm::value_ptr(lights[2].position));
	glUniform3fv(color.uniforms[10], 1, glm::value_ptr(Constants::AMBIENT_LIGHT));

	glActiveTexture(GL_TEXTURE0 + 8);
	glBindTexture(GL_TEXTURE_2D, sm.depth);
	glUniform1i(color.uniforms[7], 8);	
	glUniformMatrix4fv(color.uniforms[8], 1, GL_FALSE, glm::value_ptr(sm.P));
	glUniformMatrix4fv(color.uniforms[9], 1, GL_FALSE, glm::value_ptr(sm.V));
	
	for (ColorObject* & cObj : color_objects) {
		if (cObj->is_hidden || !cObj->is_loaded)
			continue;

		glUniformMatrix4fv(color.uniforms[2], 1, GL_FALSE, glm::value_ptr(cObj->GetModelMatrix()));
		glUniform4fv(color.uniforms[3], 1, glm::value_ptr(cObj->GetDisplayColor()));	

		glBindVertexArray(cObj->vao);	
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_TRIANGLES, 0, cObj->num_vertices);
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////

	glUseProgram(texture.id);

	for (int i = 0; i < CURR_NR_TEXTURES; ++i) {
		textures[i]->Bind();
	}

	glUniformMatrix4fv(texture.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
	glUniformMatrix4fv(texture.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));
	glUniform3fv(texture.uniforms[4], 1, glm::value_ptr(Constants::AMBIENT_LIGHT));

	for (TextureObject* & tObj : texture_objects) {
		if (tObj->is_hidden || !tObj->is_loaded)
			continue;

		glUniformMatrix4fv(texture.uniforms[2], 1, GL_FALSE, glm::value_ptr(tObj->GetModelMatrix()));
		glUniform1i(texture.uniforms[3], tObj->texture_id);
		glBindVertexArray(tObj->vao);	
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glDrawArrays(GL_TRIANGLES, 0, tObj->num_vertices);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	
	glUseProgram(dicom_point_cloud.id);
	
	glUniformMatrix4fv(dicom_point_cloud.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
	glUniformMatrix4fv(dicom_point_cloud.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));

	//glm::vec4 head_pos = glm::inverse(m_mat4HMDPose) * glm::vec4(0.01f, 0.05f, 0.0f, 1.0f);
	glm::vec4 head_pos = controller_pose1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	glUniform3fv(dicom_point_cloud.uniforms[4], 1, glm::value_ptr(lights[0].position));
	glUniform3fv(dicom_point_cloud.uniforms[5], 1, glm::value_ptr(lights[1].position));
	glUniform3fv(dicom_point_cloud.uniforms[6], 1, glm::value_ptr(lights[2].position));
	glUniform3fv(dicom_point_cloud.uniforms[12], 1, glm::value_ptr(head_pos));
	glUniform3fv(dicom_point_cloud.uniforms[13], 1, glm::value_ptr(Constants::AMBIENT_LIGHT));

	for (DicomPointCloudObject* & dpco : dicom_point_cloud_objects) {

		if (!dpco->is_loaded)
			continue;

		glUniformMatrix4fv(dicom_point_cloud.uniforms[2], 1, GL_FALSE, glm::value_ptr(dpco->GetModelMatrix()));
		glUniform1i(dicom_point_cloud.uniforms[3], dpco->curr_tolerance);
		glUniform3fv(dicom_point_cloud.uniforms[7], 1, glm::value_ptr(dpco->scale));
		glUniform3fv(dicom_point_cloud.uniforms[8], 1, glm::value_ptr(dpco->lower_bounds));
		glUniform3fv(dicom_point_cloud.uniforms[9], 1, glm::value_ptr(dpco->upper_bounds));
		glUniform3fv(dicom_point_cloud.uniforms[10], 1, glm::value_ptr(dpco->box_scale));

		glBindVertexArray(dpco->vao);	
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glVertexAttribDivisor(0, 0);
		glVertexAttribDivisor(1, 0);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);

		glDrawArraysInstanced(GL_TRIANGLES, 0, dpco->num_vertices, dpco->num_instances);
	
		glUseProgram(branch_point.id);

		glUniformMatrix4fv(branch_point.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
		glUniformMatrix4fv(branch_point.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));
		glUniformMatrix4fv(branch_point.uniforms[2], 1, GL_FALSE, glm::value_ptr(dpco->GetModelMatrix()));
		glUniform3fv(branch_point.uniforms[7], 1, glm::value_ptr(lights[0].position));
		glUniform3fv(branch_point.uniforms[8], 1, glm::value_ptr(lights[1].position));
		glUniform3fv(branch_point.uniforms[9], 1, glm::value_ptr(lights[2].position));

		glUniform3fv(branch_point.uniforms[4], 1, glm::value_ptr(dpco->lower_bounds));
		glUniform3fv(branch_point.uniforms[5], 1, glm::value_ptr(dpco->scale));

		glBindVertexArray(dpco->branch_point_marker->vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		for (BranchPoint & bp : dpco->branch_points) {
			glUniform4fv(branch_point.uniforms[6], 1, glm::value_ptr(bp.getColor()));
			glUniform3fv(branch_point.uniforms[3], 1, glm::value_ptr(bp.position));
			glDrawArrays(GL_TRIANGLES, 0, dpco->branch_point_marker->num_vertices);
		}

		glUseProgram(branch_line.id);
		glUniformMatrix4fv(branch_line.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
		glUniformMatrix4fv(branch_line.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));
		glUniformMatrix4fv(branch_line.uniforms[2], 1, GL_FALSE, glm::value_ptr(dpco->GetModelMatrix()));
		glUniform3fv(branch_line.uniforms[3], 1, glm::value_ptr(dpco->lower_bounds));
		glUniform3fv(branch_line.uniforms[4], 1, glm::value_ptr(dpco->scale));

		for (BranchPoint & bp : dpco->branch_points) {
			for (int j = 0; j < bp.neighbors.size(); ++j) {

				if (bp.neighbors[j] < bp.id) {
					BranchPoint* neighbor1 = dpco->GetBranchPointByID(bp.neighbors[j]);
					if (neighbor1 != NULL) {
						glUniform3fv(branch_line.uniforms[5], 1, glm::value_ptr(bp.position));
						glUniform3fv(branch_line.uniforms[6], 1, glm::value_ptr(neighbor1->position));
						//glUniform3fv(branch_line.uniforms[5], 1, glm::value_ptr(glm::vec3(0.0f,0.0f,0.0f)));
						//glUniform3fv(branch_line.uniforms[6], 1, glm::value_ptr(glm::vec3(5.0f,5.0f,5.0f)));
						glDrawArrays(GL_LINES, 0, 2);
					}
				}
			}
		}

	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}

void Render::Interact(glm::mat4 _controllerPose1, glm::mat4 _controllerPose2, glm::vec3 _ray, glm::vec3 _pos, bool _pressed) {

	static int controllerSelection = -1;
	static ColorObject * current_selection = NULL;
	static ColorObject * current_selection2 = NULL;
	glm::vec4 p1 = _controllerPose1 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 p2 = _controllerPose1 * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	glm::vec3 ray = glm::vec3(p2 - p1);
	static glm::vec3 collision_point;
	static bool first_press = true;

	if (_pressed && first_press) {
		current_selection = NULL;
		float min_dist = 999999999.0f;
		for (ColorObject* & cObj : color_objects) {
			if (cObj->is_selectable && cObj->TestCollision(_ray, _pos, collision_point, true)) {
				float curr_dist = glm::length(collision_point - _pos);
				if (glm::dot(ray, collision_point - _pos) > 0 && curr_dist < min_dist) {
					min_dist = curr_dist;
					current_selection = cObj;
				}
			}
		}
		if (current_selection != NULL && current_selection->controllerSelectorId == -1) {
			current_selection->controllerSelectorId = 0;
			current_selection->SetSelected(true);
			current_selection->Set_cache_pose(_controllerPose1, 1);//[1] = _controllerPose1;
			current_selection->Set_cache_pose(_controllerPose1, 2);//[2] = _controllerPose1;
			current_selection->Set_cache_pose(current_selection->append_pose,3);//[3] = current_selection->append_pose;
			current_selection->Set_cache_pose(glm::inverse(_controllerPose1) * current_selection->append_pose,0);//[0] = glm::inverse(_controllerPose1) * current_selection->append_pose;
		}
		first_press = false;
	}
	else if (_pressed && current_selection != NULL && current_selection->controllerSelectorId == 0) {
		current_selection->Set_cache_pose( _controllerPose1,1);
	}
	else if (_pressed && current_selection != NULL && current_selection->controllerSelectorId == -1) {
		current_selection->controllerSelectorId = 0;
		current_selection->SetSelected(true);
		current_selection->Set_cache_pose(_controllerPose1, 1);//[1] = _controllerPose1;
		current_selection->Set_cache_pose(_controllerPose1, 2);//[2] = _controllerPose1;
		current_selection->Set_cache_pose(current_selection->append_pose, 3);//[3] = current_selection->append_pose;
		current_selection->Set_cache_pose(glm::inverse(_controllerPose1) * current_selection->append_pose, 0);
	}
	else if (!_pressed && current_selection != NULL){
		if (current_selection->controllerSelectorId == 0)
			current_selection->controllerSelectorId = -1;
		current_selection->SetSelected(false);
		current_selection = NULL;
		first_press = true;
	}
	else if (!_pressed) {
		if (current_selection!=NULL && current_selection->controllerSelectorId == 0)
			current_selection->controllerSelectorId = -1;


		current_selection = NULL;
		first_press = true;
	}

	static bool first_press21 = true;

	if (current_selection == NULL && _pressed && first_press21) {
		for (DicomPointCloudObject* & dpcObj : dicom_point_cloud_objects) {
			if (dpcObj->TestCollision(_ray, _pos, collision_point)) {

			}
		}
		first_press21 = false;
	}
	else if (!_pressed) {
		first_press21 = true;
	}

	/// duplicate code controller 2
	//tmp

	p1 = controller_pose2 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	p2 = controller_pose2 * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	ray = glm::vec3(p2 - p1);
	static glm::vec3 collision_point2;
	static bool first_press2 = true;

	_pos = glm::vec3(p1);
	_ray = ray;

	if (controller_press2 && first_press2) {
		current_selection2 = NULL;
		float min_dist = 999999999.0f;
		for (ColorObject* & cObj : color_objects) {
			if (cObj->is_selectable && cObj->TestCollision(_ray, _pos, collision_point2, true)) {
				float curr_dist = glm::length(collision_point2 - _pos);
				if (glm::dot(ray, collision_point2 - _pos) > 0 && curr_dist < min_dist) {
					min_dist = curr_dist;
					current_selection2 = cObj;
				}
			}
		}
		if (current_selection2 != NULL && current_selection2->controllerSelectorId == -1) {
			current_selection2->controllerSelectorId = 1;
			current_selection2->SetSelected(true);
			current_selection2->Set_cache_pose(controller_pose2, 1);//[1] = _controllerPose1;
			current_selection2->Set_cache_pose(controller_pose2, 2);//[2] = _controllerPose1;
			current_selection2->Set_cache_pose(current_selection2->append_pose, 3);//[3] = current_selection->append_pose;
			current_selection2->Set_cache_pose(glm::inverse(controller_pose2) * current_selection2->append_pose, 0);//[0] = glm::inverse(_controllerPose1) * current_selection->append_pose;
		}
		first_press2 = false;
	}
	else if (controller_press2 && current_selection2 != NULL && current_selection2->controllerSelectorId == 1) {
		current_selection2->Set_cache_pose(controller_pose2, 1);
	}
	else if (_pressed && current_selection2 != NULL && current_selection2->controllerSelectorId == -1) {
		current_selection2->controllerSelectorId = 1;
		current_selection2->SetSelected(true);
		current_selection2->Set_cache_pose(controller_pose2, 1);//[1] = _controllerPose1;
		current_selection2->Set_cache_pose(controller_pose2, 2);//[2] = _controllerPose1;
		current_selection2->Set_cache_pose(current_selection2->append_pose, 3);//[3] = current_selection->append_pose;
		current_selection2->Set_cache_pose(glm::inverse(controller_pose2) * current_selection2->append_pose, 0);
	}
	else if (!controller_press2 && current_selection2 != NULL) {
		if (current_selection2->controllerSelectorId == 1)
			current_selection2->controllerSelectorId = -1;
		current_selection2->SetSelected(false);
		current_selection2 = NULL;
		first_press2 = true;
	}
	else if (!controller_press2) {
		if (current_selection2 != NULL && current_selection2->controllerSelectorId == 1)
			current_selection2->controllerSelectorId = -1;
		current_selection2 = NULL;
		first_press2 = true;
	}

	static bool first_press3 = true;

	if (current_selection2 == NULL && controller_press2 && first_press3) {
		for (DicomPointCloudObject* & dpcObj : dicom_point_cloud_objects) {
			if (dpcObj->TestCollision(_ray, _pos, collision_point2)) {

			}
		}
		first_press3 = false;
	}
	else if (!controller_press2) {
		first_press3 = true;
	}

	// test same selection	
	static bool once = true;
	if (current_selection == current_selection2 && current_selection != NULL) {
		
		static float initial_dist;
		static float new_dist;
		static float initial_scale;

		if (once) {
			initial_dist = glm::length(vr_info.controller_world_pos - vr_info.controller_world_pos2);
			initial_scale = current_selection->scale.x;
			once = false;
		}
		else {
			float curr_dist = glm::length(vr_info.controller_world_pos - vr_info.controller_world_pos2);
			float ratio = curr_dist / initial_dist;
			float new_scale = ratio*initial_scale;
			std::cout << new_scale << std::endl;
			current_selection->Set_scale(glm::vec3(new_scale));
		}
	}
	else {
		once = true;
	}

}

void Render::PrintMat4(glm::mat4 m) {
	std::cout << m[0][0] << " " << m[0][1] << " " << m[0][2] << " " << m[0][3] << std::endl;
	std::cout << m[1][0] << " " << m[1][1] << " " << m[1][2] << " " << m[1][3] << std::endl;
	std::cout << m[2][0] << " " << m[2][1] << " " << m[2][2] << " " << m[2][3] << std::endl;
	std::cout << m[3][0] << " " << m[3][1] << " " << m[3][2] << " " << m[3][3] << std::endl;
}

void Render::FakeInput() {
	float move_rate = 0.025f;
	float rot_rate = 0.025f;

	// time based movement
	//float delta_time_factor = (_deltaT + 1.0f) / 16.67f;
	//move_rate *= delta_time_factor;
	//rot_rate *= delta_time_factor;

	// controller 1
	glm::vec4 p1 = controller1->GetModelMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 p2 = controller1->GetModelMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	glm::vec3 ray = glm::vec3(p2 - p1);
	

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		controller1->world_position += move_rate*ray;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		controller1->world_position.x -= move_rate;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		controller1->world_position -= move_rate*ray;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		controller1->world_position.x += move_rate;
	}

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	
	glm::vec2 mpos;
	mpos.x = width / 4 - xpos;
	mpos.y = height / 2 - ypos;

	int focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
	
	if (focused) {
		glfwSetCursorPos(window, width / 4, height / 2);
		controller1->model_orientation.x += rot_rate*mpos.x;
		controller1->model_orientation.y += glm::clamp(rot_rate*mpos.y, -1.2f, 1.2f);
		controller1->CalcModelMatrix();
		controller_press1 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		controller1->SetSelected(controller_press1);
		vr_info.controller_pose = controller1->GetModelMatrix();
		vr_info.controller_press = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
		glm::mat4 tmp = controller1->GetModelMatrix();
		vr_info.controller_world_pos = glm::vec3(tmp[0][3], tmp[1][3], tmp[2][3]);
	}
	else {
	
	}

	// controller 2

	p1 = controller2->GetModelMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	p2 = controller2->GetModelMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	ray = glm::vec3(p2 - p1);

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		controller2->world_position += move_rate*ray;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		controller2->world_position.x -= move_rate;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		controller2->world_position -= move_rate*ray;
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		controller2->world_position.x += move_rate;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		controller2->model_orientation.x += move_rate;
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		controller2->model_orientation.x -= move_rate;
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		controller2->model_orientation.y += move_rate;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		controller2->model_orientation.y -= move_rate;
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		controller2->world_position.y += move_rate;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		controller2->world_position.y -= move_rate;
	}

	static bool once = true;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && once) {
		controller_press2 = !controller_press2;
		controller2->SetSelected(controller_press2);
		once = false;
	}
	else if(!glfwGetKey(window, GLFW_KEY_N)) {
		once = true;
	}


	controller2->CalcModelMatrix();
	controller_pose2 = controller2->GetModelMatrix();
}


void Render::UpdateHMDMatrixPose(){
	if (!m_pHMD) {
		vr_info.hmd_connected = false;
		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			FakeInput();
		}
		controller1->is_hidden = true;
		return;
	}

	controller1->is_hidden = false;
	controller2->is_hidden = false;

	vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid){
		//m_mat4HMDPose = ValveMat34ToGlmMat4Inv(m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking) *ValveMat34ToGlmMat4Inv(m_pHMD->GetSeatedZeroPoseToStandingAbsoluteTrackingPose());
		vr_info.head_pose_inv = ValveMat34ToGlmMat4Inv(m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking);// *ValveMat34ToGlmMat4Inv(m_pHMD->GetSeatedZeroPoseToStandingAbsoluteTrackingPose());

		vr_info.left_eye_proj = ValveMat4ToGlmMat4(m_pHMD->GetProjectionMatrix(vr::Eye_Left, 0.1f, 1000.0f, vr::API_OpenGL));
		vr_info.left_eye_transform_inv = ValveMat34ToGlmMat4Inv(m_pHMD->GetEyeToHeadTransform(vr::Eye_Left)) *vr_info.head_pose_inv;

		vr_info.right_eye_proj = ValveMat4ToGlmMat4(m_pHMD->GetProjectionMatrix(vr::Eye_Right, 0.1f, 1000.0f, vr::API_OpenGL));
		vr_info.right_eye_transform_inv = ValveMat34ToGlmMat4Inv(m_pHMD->GetEyeToHeadTransform(vr::Eye_Right)) *vr_info.head_pose_inv;
	}

	int controllerIndex = 0;
	for (vr::TrackedDeviceIndex_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; ++unTrackedDevice){
		if (!m_pHMD->IsTrackedDeviceConnected(unTrackedDevice))
			continue;

		if (m_pHMD->GetTrackedDeviceClass(unTrackedDevice) != vr::TrackedDeviceClass_Controller)
			continue;


		if (!m_rTrackedDevicePose[unTrackedDevice].bPoseIsValid)
			continue;

		if (controllerIndex == 0) {
			vr_info.controller_pose = ValveMat34ToGlmMat4(m_rTrackedDevicePose[unTrackedDevice].mDeviceToAbsoluteTracking);

			vr_info.controller_world_pos = glm::vec3(vr_info.controller_pose * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
			controller1->model_matrix = vr_info.controller_pose;
		
			vr::VRControllerState_t state;
			if (m_pHMD->GetControllerState(unTrackedDevice, &state))
			{
				vr_info.controller_press = state.ulButtonPressed != 0;
			}
			controllerIndex++;
		}
		else if (controllerIndex == 1) {
			vr_info.controller_pose2 = ValveMat34ToGlmMat4(m_rTrackedDevicePose[unTrackedDevice].mDeviceToAbsoluteTracking);

			vr_info.controller_world_pos2 = glm::vec3(vr_info.controller_pose2 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
			controller2->model_matrix = vr_info.controller_pose2;

			vr::VRControllerState_t state;
			if (m_pHMD->GetControllerState(unTrackedDevice, &state))
			{
				vr_info.controller_press2 = state.ulButtonPressed != 0;
			}
			controllerIndex++;
		}
	}






	// tmp
	controller_press2 = vr_info.controller_press2;
	controller_pose2 = vr_info.controller_pose2;
}

glm::mat4 Render::ValveMat34ToGlmMat4Inv(vr::HmdMatrix34_t _mIN) {
	return glm::inverse(glm::mat4(
		_mIN.m[0][0], _mIN.m[1][0], _mIN.m[2][0], 0.0,
		_mIN.m[0][1], _mIN.m[1][1], _mIN.m[2][1], 0.0,
		_mIN.m[0][2], _mIN.m[1][2], _mIN.m[2][2], 0.0,
		_mIN.m[0][3], _mIN.m[1][3], _mIN.m[2][3], 1.0f));
}

glm::mat4 Render::ValveMat34ToGlmMat4(vr::HmdMatrix34_t _mIN) {
	return glm::mat4(
		_mIN.m[0][0], _mIN.m[1][0], _mIN.m[2][0], 0.0,
		_mIN.m[0][1], _mIN.m[1][1], _mIN.m[2][1], 0.0,
		_mIN.m[0][2], _mIN.m[1][2], _mIN.m[2][2], 0.0,
		_mIN.m[0][3], _mIN.m[1][3], _mIN.m[2][3], 1.0f);
}

glm::mat4 Render::ValveMat4ToGlmMat4Inv(vr::HmdMatrix44_t _mIN) {
	return glm::inverse(glm::mat4(
		_mIN.m[0][0], _mIN.m[1][0], _mIN.m[2][0], _mIN.m[3][0],
		_mIN.m[0][1], _mIN.m[1][1], _mIN.m[2][1], _mIN.m[3][1],
		_mIN.m[0][2], _mIN.m[1][2], _mIN.m[2][2], _mIN.m[3][2],
		_mIN.m[0][3], _mIN.m[1][3], _mIN.m[2][3], _mIN.m[3][3]));
}

glm::mat4 Render::ValveMat4ToGlmMat4(vr::HmdMatrix44_t _mIN) {
	return glm::mat4(
		_mIN.m[0][0], _mIN.m[1][0], _mIN.m[2][0], _mIN.m[3][0],
		_mIN.m[0][1], _mIN.m[1][1], _mIN.m[2][1], _mIN.m[3][1],
		_mIN.m[0][2], _mIN.m[1][2], _mIN.m[2][2], _mIN.m[3][2],
		_mIN.m[0][3], _mIN.m[1][3], _mIN.m[2][3], _mIN.m[3][3]);
}

std::string Render::ReadFile(std::string _filePath) {
	std::ifstream t(_filePath);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	return str;
}

GLuint Render::CompileGLShader(std::string programName, std::string shaderDir){

	std::string vs_src = Render::ReadFile(shaderDir + programName + ".vs.glsl");
	std::string fs_src = Render::ReadFile(shaderDir + programName + ".fs.glsl");

	GLuint unProgramID = glCreateProgram();

	GLuint nSceneVertexShader = Render::CreateShader(GL_VERTEX_SHADER, vs_src);
	glAttachShader(unProgramID, nSceneVertexShader);
	glDeleteShader(nSceneVertexShader); 

	GLuint  nSceneFragmentShader = Render::CreateShader(GL_FRAGMENT_SHADER, fs_src);
	glAttachShader(unProgramID, nSceneFragmentShader);
	glDeleteShader(nSceneFragmentShader);

	glLinkProgram(unProgramID);

	GLint programSuccess = GL_TRUE;
	glGetProgramiv(unProgramID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE){
		printf("%s - Error linking program %d!\n", programName.c_str(), unProgramID);
		glDeleteProgram(unProgramID);
		return 0;
	}

	glUseProgram(unProgramID);
	glUseProgram(0);

	return unProgramID;
}

GLuint Render::CreateShader(GLint target, std::string& src) {

	GLuint shader = glCreateShader(target);
	const char* c_str = src.c_str();
	GLint len = src.length();

	glShaderSource(shader, 1, &c_str, &len);
	glCompileShader(shader);

	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &fShaderCompiled);

	if (fShaderCompiled != GL_TRUE){
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		for (auto const& value : errorLog) {
			std::cout << value;
		}

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

bool Render::CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc){

	glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

	glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);

	glGenTextures(1, &framebufferDesc.m_nRenderTextureId);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

	glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);

	glGenTextures(1, &framebufferDesc.m_nResolveTextureId);
	glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);

	// check FBO status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE){
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

bool Render::createFrameBuffer(ShadowMap &sm) {

	glGenFramebuffers(1, &sm.fbo);

	glGenTextures(1, &sm.depth);
	glBindTexture(GL_TEXTURE_2D, sm.depth);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, DEFAULT_SHADOW_RES, DEFAULT_SHADOW_RES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sm.depth, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}
