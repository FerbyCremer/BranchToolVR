#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <vector>

#include "AbstractBaseObject.h"
#include "DicomReader.h"
#include "MiscFunctions.h"

class TextureObject : public AbstractBaseObject{

	public:

		TextureObject();
		~TextureObject();

		void GenerateXYPlane(float _scaleX, float _scaleY, glm::vec2 _padding, glm::vec3 _offset);
		void GenerateXYPlane(float _scaleX, float _scaleY, float _padding, glm::vec3 _offset);
		void GenerateText(std::string _text, float _scale, glm::vec2 _padding, glm::vec3 _offset);
		void GenerateText(std::string _text, glm::vec2 _scale, glm::vec2 _padding, glm::vec3 _offset);
		
	//private:

		void Load();
		int Type();
		void Finalize();
		void AddRectangularPrism(glm::vec3 _scale, glm::vec3 _offset);
		void AddLetter(float _scaleX, float _scaleY, glm::vec2 _uvPadding, glm::vec2 _padding, glm::vec3 _offset, int _ascii);

		// vertex attributes
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;

		// opengl vars
		GLuint texture_id;
		GLuint num_vertices;
		GLuint vao;
		GLuint positions_buffer;
		GLuint normals_buffer;
		GLuint uvs_buffer;
};

