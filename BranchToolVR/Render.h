#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <openvr.h>

#include "ColorObject.h"
#include "LineObject.h"
#include "DicomPointCloudObject.h"
#include "AbstractBaseObject.h"
#include "TextureObject.h"
#include "Texture.h"

struct VrData {
	glm::mat4 controller_pose;
	glm::vec3 controller_world_pos;
	glm::mat4 head_pose_inv;
	glm::mat4 left_eye_proj;
	glm::mat4 right_eye_proj;
	glm::mat4 left_eye_transform_inv;
	glm::mat4 right_eye_transform_inv;
	bool controller_press;
	bool hmd_connected;

	VrData() {
		controller_pose = glm::mat4(1.0f);
		controller_world_pos = glm::vec3(0.0f);
		head_pose_inv = glm::mat4(1.0f);
		left_eye_proj = glm::mat4(1.0f);
		right_eye_proj = glm::mat4(1.0f);
		left_eye_transform_inv = glm::mat4(1.0f);
		right_eye_transform_inv = glm::mat4(1.0f);
		hmd_connected = false;
		controller_press = false;
	}
};

struct CursorData {
	glm::vec2 normalized_cursor_position;
	bool is_pressed;
	bool first_press;

	CursorData() {
		normalized_cursor_position = glm::vec2(0.0f, 0.0f);
		is_pressed = false;
		first_press = true;
	}
};


struct Light {
	glm::vec3 position;
	ColorObject marker;

	Light() {
		marker.GenerateSphere(11, 0.05f, true);
	}

	Light(float _radius) {
		marker.GenerateSphere(11, _radius, true);
	}
};

enum TexturesEnum {
	WOOD_TEXTURE,
	FONT_TEXTURE,
	CURR_ORTHOSLICE_TEXTURE,
	CURR_NR_TEXTURES, // keep as last element of enum
};

struct ShaderProgram {
	GLuint id;
	GLuint * uniforms;
	GLuint num_uniforms;
};

struct FramebufferDesc{
	GLuint m_nDepthBufferId;
	GLuint m_nRenderTextureId;
	GLuint m_nRenderFramebufferId;
	GLuint m_nResolveTextureId;
	GLuint m_nResolveFramebufferId;
};

struct ShadowMap {
	GLuint depth;
	GLuint fbo;
	glm::mat4 P;
	glm::mat4 V;
};

class Render{

	public:
		Render(GLFWwindow* _window);
		~Render();
		void AddObjectToScene(std::vector<AbstractBaseObject*> bsos);
		void AddObjectToScene(AbstractBaseObject* abso);
		void AddObjectToScene(ColorObject * bso);
		void AddObjectToScene(DicomPointCloudObject * dpco);
		void AddObjectToScene(LineObject * l);
		void AddObjectToScene(TextureObject * t);
		void AddObjectToUi(AbstractBaseObject* abso);
		void AddObjectToUi(TextureObject * t);
		void AddObjectToUi(ColorObject * c);
		void RenderScene();
		void Interact(glm::mat4 _controllerPose1, glm::mat4 _controllerPose2, glm::vec3 _ray, glm::vec3 _pos, bool _pressed);
		void UpdateHMDMatrixPose();
		void ResetSeatedPose();
		void Finalize();
		void RenderUI(int level);
		void FakeInput();
		static void window_size_callback(GLFWwindow* window, int width, int height);
		static int window_size_x;
		static int window_size_y;
		static int half_window_size_x;
		static int half_window_size_y;
		static float aspect;
		static float half_aspect;
		static VrData vr_info;
		static CursorData cursor_info;

	//private:

		// ui variables
		static glm::vec4 ui_quadrant_ortho[4];
		static glm::vec4 ui_quadrant_ortho_aspect[4];
		glm::vec3 ui_panel_size;
		glm::vec3 half_ui_panel_size;
		static glm::mat4 ui_projection;
		static glm::mat4 ui_view;
		void UpdateCursor();

		// glfw reference (must be initialized before constructor)
		GLFWwindow * window;

		// Textures
		int num_textures;
		Texture ** textures;

		// Lights
		int num_lights;
		Light * lights;
		void UpdateLights();

		// shadows
		ShadowMap sm;
		void RenderShadows();

		// shader programs
		ShaderProgram color;
		ShaderProgram texture;
		ShaderProgram line;
		ShaderProgram dicom_point_cloud;
		ShaderProgram branch_point;
		ShaderProgram branch_line;
		ShaderProgram ui_color;
		ShaderProgram ui_texture;
		ShaderProgram shadow;
		
		// object containers
		std::vector<ColorObject*> color_ui_elements;
		std::vector<TextureObject*> texture_ui_elements;
		std::vector<ColorObject*> color_objects;
		std::vector<TextureObject*> texture_objects;
		std::vector<LineObject*> line_objects;
		std::vector<DicomPointCloudObject*> dicom_point_cloud_objects;

		// VR variables
		FramebufferDesc leftEyeDesc;
		FramebufferDesc rightEyeDesc;
		vr::IVRSystem *m_pHMD;
		vr::IVRRenderModels *m_pRenderModels;
		vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
		glm::mat4 m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];
		glm::mat4 m_mat4HMDPose;
		uint32_t m_nRenderWidth;
		uint32_t m_nRenderHeight;
		glm::mat4 controller_pose1;
		ColorObject * controller;
		bool controller_press1;
		
		// internal functions
		bool InitVR();
		void RenderEyes();

		bool createFrameBuffer(ShadowMap &sm);
		void RenderSceneInternal(glm::mat4 _P, glm::mat4 _V);
		
		// static helper functions
		static void PrintMat4(glm::mat4 m);
		static glm::mat4 ValveMat34ToGlmMat4Inv(vr::HmdMatrix34_t _mIN);
		static glm::mat4 ValveMat34ToGlmMat4(vr::HmdMatrix34_t _mIN);
		static glm::mat4 ValveMat4ToGlmMat4(vr::HmdMatrix44_t _mIN);
		static glm::mat4 ValveMat4ToGlmMat4Inv(vr::HmdMatrix44_t _mIN);
		static bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc);
		static std::string ReadFile(std::string _filePath);
		static GLuint CreateShader(GLint target, std::string& src);
		static GLuint CompileGLShader(std::string programName, std::string shaderDir);
};
