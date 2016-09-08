#pragma once

#include "GL\glew.h"
#include "lodepng.h"
#include "pathtools.h"
#include "tinydir.h"
#include <string>
#include <iostream>
#include <vector>

#include "DicomReader.h"
#include "Constants.h"

class Texture{

	public:

		Texture();
		~Texture();
		bool Load(std::string _fp);
		bool Load(DicomSingle & _ds, int window_width, int window_center);
		void Bind();
		void Delete();

		static int global_counter;
		GLuint id;
		GLuint level;
		bool is_loaded;
};

