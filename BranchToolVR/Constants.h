#pragma once

#include <string>

#define DEFAULT_WINDOW_SIZE_X 1000
#define DEFAULT_WINDOW_SIZE_Y 1000
#define WINDOW_TITLE_STRING "Branch Tool VR"
#define DEFAULT_ISOVALUE 1340
#define DEFAULT_ISOVALUE_TOLERANCE 10
#define MAX_ISOVALUE_TOLERANCE 30
#define DEFAULT_WINDOW_WIDTH 1000
#define DEFAULT_WINDOW_CENTER 0


namespace Constants {
	static const std::string RELATIVE_RESOURCE_PATH = "..//../BranchToolVR/Resources/";
	static const std::string RELATIVE_DICOM_RESOURCE_PATH = "..//../BranchToolVR/Resources/DICOM/";
	static const glm::vec3 DICOM_PANEL_DIMENSIONS = glm::vec3(1.5f, 0.5f, 0.025f);
}