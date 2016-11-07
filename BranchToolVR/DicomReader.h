#pragma once

#include <glm/glm.hpp>
#include <windows.h>
#include <Windows.h>
#include <thread>
#include <string>
#include <iostream>
#include <imebra.h>
#include <tinydir.h>
#include <pathtools.h>

#include "Constants.h"

struct IsovaluePoint {
	glm::vec3 pos;
	float diff; // difference from mean
};

struct DicomSingle {
	std::vector<short> isovalues;
	int width;
	int height;
	int layer;
	float spacing;
	float pixel_spacing;
	std::string series_id;
};

struct DicomSet {
	std::vector<DicomSingle> data;
	std::string series_id;
	glm::vec3 scale;
	bool is_loaded;
	int isovalue;
	int isovalue_tolerance;
	int window_center;
	int window_width;
	int current_index;

	DicomSet() {
		series_id = "";
		scale = glm::vec3(0.0f, 0.0f, 0.0f);
		current_index = 0;
		is_loaded = false;
		isovalue = DEFAULT_ISOVALUE;
		isovalue_tolerance = DEFAULT_ISOVALUE_TOLERANCE;
		window_center = DEFAULT_WINDOW_CENTER;
		window_width = DEFAULT_WINDOW_WIDTH;
	}
};

class DicomReader{
	public:
		static DicomSet ReadSet(std::string _dirFilePath, bool * finished);
		static DicomSingle ReadSingle(std::string _filePath);
};

