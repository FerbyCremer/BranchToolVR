#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "AbstractBaseObject.h"
#include "DicomReader.h"
#include "MiscFunctions.h"

class ColorObject : public AbstractBaseObject{

	public:

		ColorObject();
		~ColorObject();

		// functions
		void GenerateXYPlane(float _scaleX, float _scaleY, glm::vec2 _padding, glm::vec3 _offset);
		void GenerateXYPlane(float _scaleX, float _scaleY, float _padding, glm::vec3 _offset);
		void GenerateXYPrism(float _scaleX, float _scaleY, float _scaaleZ, glm::vec2 _padding, glm::vec3 _offset);
		void GenerateXYPrism(glm::vec3 _scale, glm::vec2 _padding, glm::vec3 _offset);
		void GenerateSphere(int _res, float _radius, bool _invNormals);
		void GenerateIsosurfaceFromDicomSet(DicomSet & _dSet, int _isovalue);
		void GenerateController();
		void GenerateRoom();
		void SetDisplayColor(const glm::vec4 & _inColor);
		void SetSelected(bool _isSelected);
		bool TestCollision(glm::vec3 _ray, glm::vec3 _pos, glm::vec3 & _cp, bool world_space);
		bool TestCollision(glm::vec3 _inPos);
		glm::vec4 GetDisplayColor();

		// vertex attributes TODO privatize
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;

	//private:

		void SetBoundingBox();
		void AddRectangularPrism(glm::vec3 _scale, glm::vec3 _offset);
		void Finalize();
		void Load();
		int Type();

		glm::vec3 min;
		glm::vec3 max;
		glm::vec4 display_color;
		float display_color_modifier;

		// opengl vars
		GLuint num_vertices;
		GLuint vao;
		GLuint positions_buffer;
		GLuint normals_buffer;

};
