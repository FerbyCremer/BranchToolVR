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
#include <stdio.h>
#include <cstdlib>

#include "ColorObject.h"
#include "LineObject.h"
#include "DicomPointCloudObject.h"
#include "AbstractBaseObject.h"
#include "TextureObject.h"
#include "Texture.h"
#include "RenderModel.h"
#include "MiscFunctions.h"
#include "MiscStructs.h"

struct Light 
{
	glm::vec3 position;
	ColorObject marker;
	float intensity;

	Light()
	{
		marker.GenerateSphere(11, 0.2f, true);
	}

	Light(float _radius) 
	{
		marker.GenerateSphere(11, _radius, true);
	}
};

enum TexturesEnum 
{
	WOOD_TEXTURE,
	FONT_TEXTURE,
	CURR_ORTHOSLICE_TEXTURE,
	COARSE_VIEWER_SLICE_HANDLE_TEXTURE,
	CURR_NR_TEXTURES, // keep as last element of enum
};

struct ShaderProgram 
{
	GLuint id;
	GLuint * uniforms;
	GLuint num_uniforms;
};

struct FramebufferDesc 
{
	GLuint m_nDepthBufferId;
	GLuint m_nRenderTextureId;
	GLuint m_nRenderFramebufferId;
	GLuint m_nResolveTextureId;
	GLuint m_nResolveFramebufferId;
};

struct ShadowMap
{
	GLuint depth;
	GLuint fbo;
	glm::mat4 P;
	glm::mat4 V;
};

class Render
{

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
		void Interact();
		void UpdateHMDMatrixPose();
		void Finalize();
		void RenderUI(int level);
		void FakeInput(int controllerIndex);
		void DetectCollision(VrMotionController & _controller);
		void BindTextures();
		void LoadTextures();
		void LoadShaders();


		static void window_size_callback(GLFWwindow* window, int width, int height);
		static int window_size_x;
		static int window_size_y;
		static int half_window_size_x;
		static int half_window_size_y;
		static int fourth_window_size_x;
		static int fourth_window_size_y;
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
		ShaderProgram recieve_shadow_color;
		
		// object containers
		std::vector<AbstractBaseObject*> all_objects;
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
		bool m_rbShowTrackedDevice[vr::k_unMaxTrackedDeviceCount];
		glm::mat4 m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];
		glm::mat4 m_mat4HMDPose;
		uint32_t m_nRenderWidth;
		uint32_t m_nRenderHeight;
		
		// motion controller
		ColorObject * controller_pointer1;
		ColorObject * controller_pointer2;
		AbstractBaseObject * selected_element1;
		AbstractBaseObject * selected_element2;
		glm::mat4 spoofControllerView;
		
		// internal functions
		bool InitVR();
		void RenderEyes();

		bool createShadowMap(ShadowMap &sm);
		void RenderSceneInternal(glm::mat4 _P, glm::mat4 _V);

		std::vector<CGLRenderModel*> m_vecRenderModels;
		CGLRenderModel *m_rTrackedDeviceToRenderModel[vr::k_unMaxTrackedDeviceCount];
		CGLRenderModel* FindOrLoadRenderModel(const char *pchRenderModelName);
		void SetupRenderModels();
		void SetupRenderModelForTrackedDevice(vr::TrackedDeviceIndex_t unTrackedDeviceIndex);
		
		// static helper functions
		static glm::mat4 ValveMat34ToGlmMat4Inv(vr::HmdMatrix34_t _mIN);
		static glm::mat4 ValveMat34ToGlmMat4(vr::HmdMatrix34_t _mIN);
		static glm::mat4 ValveMat4ToGlmMat4(vr::HmdMatrix44_t _mIN);
		static glm::mat4 ValveMat4ToGlmMat4Inv(vr::HmdMatrix44_t _mIN);
		static bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc);
		static std::string ReadFile(std::string _filePath);
		static GLuint CompileGLShader(std::string programName);
		static GLuint CreateShader(GLint target, std::string& src);

};
