#pragma once

#include <string>
#include <iostream>
#include <vector>

#include "GL\glew.h"
#include "lodepng.h"
#include "pathtools.h"
#include "tinydir.h"

#include "DicomReader.h"
#include "Constants.h"
#include "MiscFunctions.h"

class Texture
{
	public:
		Texture();
		~Texture();
		void Bind(int _level);
		bool Load(std::string _fp);
		bool Load(DicomSingle & _ds, int window_width, int window_center);
	
	private:
		void Delete();
		static int texture_id_counter;
		GLuint gl_id;
		int id;
		bool is_loaded;
};
