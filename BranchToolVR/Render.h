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

class Render{

	public:
		Render();
		~Render();
		void AddObjectToScene(std::vector<AbstractBaseObject*> bsos);
		void AddObjectToScene(AbstractBaseObject* abso);
		void AddObjectToScene(ColorObject * bso);
		void AddObjectToScene(DicomPointCloudObject * dpco);
		void AddObjectToScene(LineObject * l);
		void AddObjectToScene(TextureObject * t);
		void RenderScene(GLFWwindow* window);
		void Interact(glm::mat4 _controllerPose1, glm::mat4 _controllerPose2, glm::vec3 _ray, glm::vec3 _pos, bool _pressed);
		void UpdateHMDMatrixPose();
		void ResetSeatedPose();

	//private:

		// Textures
		int num_textures;
		Texture ** textures;

		// Lights
		int num_lights;
		Light * lights;
		void UpdateLights();

		// shader programs
		ShaderProgram color;
		ShaderProgram texture;
		ShaderProgram line;
		ShaderProgram dicom_point_cloud;
		ShaderProgram branch_point;
		ShaderProgram branch_line;
		
		// object containers
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
		bool controller_press1;
		
		// internal functions
		bool InitVR();
		void RenderEyes();
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
