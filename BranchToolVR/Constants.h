#pragma once

#include <string>

#define DEFAULT_WINDOW_SIZE_X 1000
#define DEFAULT_WINDOW_SIZE_Y 1000
#define WINDOW_TITLE_STRING "Branch Tool VR"
#define DEFAULT_ISOVALUE 1340
#define DEFAULT_ISOVALUE_TOLERANCE 10
#define MAX_ISOVALUE_TOLERANCE 30
#define DEFAULT_SHADOW_RES 2048
#define DEFAULT_WINDOW_CENTER 1191
#define DEFAULT_WINDOW_WIDTH 1066


namespace Constants {
	static const std::string RELATIVE_RESOURCE_PATH = "..//../BranchToolVR/Resources/";
	static const std::string RELATIVE_DICOM_RESOURCE_PATH = "Resources//CT_H_50";  // "..//../BranchToolVR/Resources/DICOM/torso";
	static const glm::vec3 DICOM_PANEL_DIMENSIONS = glm::vec3(1.5f, 0.5f, 0.025f);
	static const glm::vec4 UI_SELECTOR_COLOR = glm::vec4(0.25f, 0.25f, 1.0f,0.5f);
	static const glm::vec3 DEFAULT_SELECTOR_SCALE = glm::vec3(0.2f, 0.2f, 0.2f);
	static const glm::vec3 AMBIENT_LIGHT = glm::vec3(0.1f, 0.1f, 0.1f);
	static const glm::vec3 X_AXIS = glm::vec3(1.0f,0.0f,0.0f);
	static const glm::vec3 Y_AXIS = glm::vec3(0.0f,1.0f,0.0f);
	static const glm::vec3 Z_AXIS = glm::vec3(0.0f,0.0f,1.0f);
	static const glm::vec3 ZERO_VEC3 = glm::vec3(0.0f, 0.0f, 0.0f);
}