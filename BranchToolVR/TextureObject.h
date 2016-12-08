#pragma once

#include <vector>

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <tiny_obj_loader.h>

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
		void readObjFromFile(std::string _name);
		void readObjFromFile(std::string _name, float _scale, glm::vec3 _offset);
		void readObjFromFile(std::string _name, float _scale);
		void Load();
		int Type();
		void SetSelected(bool _selected);
		void Finalize();
		void AddRectangularPrism(glm::vec3 _scale, glm::vec3 _offset);
		void AddLetter(float _scaleX, float _scaleY, glm::vec2 _uvPadding, glm::vec2 _padding, glm::vec3 _offset, int _ascii);
};

