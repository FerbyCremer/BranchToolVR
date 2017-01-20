#pragma once

#include <string>
#include <glm/glm.hpp>

#define DEFAULT_WINDOW_SIZE_X 1000
#define DEFAULT_WINDOW_SIZE_Y 1000
#define WINDOW_TITLE_STRING "Branch Tool VR"
#define DEFAULT_ISOVALUE 1340
#define DEFAULT_ISOVALUE_TOLERANCE 10
#define MAX_ISOVALUE_TOLERANCE 30
#define DEFAULT_SHADOW_RES 2048
#define DEFAULT_WINDOW_CENTER 1191
#define DEFAULT_WINDOW_WIDTH 1066

namespace DirectoryInfo
{
	static const std::string RELATIVE_RESOURCES_PATH = "..//../BranchToolVR/Resources/";
	static const std::string RELATIVE_SHADERS_DIR = RELATIVE_RESOURCES_PATH + "../Shaders/";
	static const std::string RELATIVE_TEXTURES_DIR = RELATIVE_RESOURCES_PATH + "Textures/";
	static const std::string RELATIVE_MODELS_DIR = RELATIVE_RESOURCES_PATH + "Models/";
	static const std::string RELATIVE_DICOM_DIR = RELATIVE_RESOURCES_PATH + "DICOM/";
	static const std::string STARTUP_DICOM_SET = "torso/dcm0";
	static const std::string POINT_CLOUD_HANDLE_MODEL = "pointCloudFrame2";
	static const std::string COARSE_VIEWER_SLICE_HANDLE_MODEL = "coarseViewerHandle2";
	static const std::string IMGUI_FRAME_MODEL = "imguiFrame";
	static const std::string MODEL_EXT = ".wobj"; // rename from obj to avoid conflicts with obj files which are set in gitignore
}

namespace Constants 
{
	static const glm::vec3 DICOM_PANEL_DIMENSIONS = glm::vec3(1.5f, 0.5f, 0.025f);
	static const glm::vec4 UI_SELECTOR_COLOR = glm::vec4(0.25f, 0.25f, 1.0f,0.5f);
	static const glm::vec4 STATIC_DICOM_MESH_COLOR = glm::vec4(0.89f, 0.3f, 0.89f, 1.0f);
	static const glm::vec4 BASE_BRANCH_POINT_COLOR = glm::vec4(1.0f, 0.2f, 0.4f, 1.0f);
	static const glm::vec4 SELECTED_BRANCH_POINT_COLOR = glm::vec4(0.9f, 0.3f, 0.9f, 1.0f);
	static const glm::vec4  HOVER_BRANCH_POINT_COLOR = glm::vec4(0.9f, 0.9f, 0.3f, 1.0f);
	static const glm::vec4 CLEAR_COLOR = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
	static const glm::vec3 AMBIENT_LIGHT = glm::vec3(0.1f, 0.1f, 0.1f);
	static const glm::vec3 X_AXIS = glm::vec3(1.0f,0.0f,0.0f);
	static const glm::vec3 Y_AXIS = glm::vec3(0.0f,1.0f,0.0f);
	static const glm::vec3 Z_AXIS = glm::vec3(0.0f,0.0f,1.0f);
	static const glm::vec3 ZERO_VEC3 = glm::vec3(0.0f, 0.0f, 0.0f);
	static const float DEFAULT_SELECTOR_SCALE = 0.2f;
	static const float TWOPI = 6.28318530718f;
	static const float DEFAULT_POINT_CLOUD_SCALE = 1.0f;
	static const float PI = 3.14159265359f;
	static const float SQRT3 = 1.73205080757f;
	static const glm::ivec2 UI_DIMENSIONS = glm::ivec2(2048, 2048);
}