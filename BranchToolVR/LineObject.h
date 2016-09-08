#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <vector>

#include "DicomReader.h"

class LineObject {

public:

	LineObject();
	~LineObject();
	void GenerateAxis();

	//private:

	// base vertex attributes
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> colors;

	// opengl vars
	GLuint num_vertices;
	GLuint vao;
	GLuint positions_buffer;
	GLuint colors_buffer;

	// functions
	void LoadBuffers();
};
