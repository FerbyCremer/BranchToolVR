#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

#include "MiscFunctions.h"
#include "Constants.h"

template <typename T>
struct vertex_attribute {
	std::vector<T> raw_data;
	GLuint buffer_id;
	std::string name;

	void Load() {
		glGenBuffers(1, &buffer_id);
		glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(T)*raw_data.size(), &raw_data[0], GL_STATIC_DRAW);
	}
};

class AbstractBaseObject
{
	public:
		AbstractBaseObject();
		~AbstractBaseObject();

		//std::vector<vertex_attribute<typename T>*> vertices;
		//virtual void Configure() = 0;
		virtual void Load() = 0;
		virtual int Type() = 0;

		bool is_loaded;
		bool is_hidden;
		bool is_selectable;
		bool is_selected;
		int id;
		short ui_quadrant;
		int level;
		int controllerSelectorId;
		glm::mat4 ui_transform;
		static int id_counter;
		glm::mat4 GetModelMatrix();
		glm::mat4 model_matrix;
		glm::mat4 base_model_matrix;
		glm::mat4 ui_model_matrix;

		void Set_world_position(const glm::vec3& v);
		void Set_model_orientation(const glm::vec3& v);
		void Set_model_positionX(float _x);
		void Set_model_positionY(const float _y);
		void Set_model_positionZ(const float _z);
		void Set_model_position(const glm::vec3& v);
		void Set_scale(const glm::vec3& v);
		void Set_append_pose(const glm::mat4& m);
		void Set_cache_pose(const glm::mat4& m, int index);

		bool TestCollision(glm::vec3 _ray, glm::vec3 _pos, glm::vec3 & _collisionPoint);
		bool TestBoundingSphere(glm::vec3 _ray, glm::vec3 _pos);

	//private:

		// model matrix components
		void CalcModelMatrix();
		glm::vec3 world_position;
		glm::vec3 model_orientation;
		glm::vec3 model_position;
		glm::vec3 scale;
		glm::mat4 append_pose;
		glm::mat4 cache_pose[4];

		// opengl and triangulation data

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;

		GLuint texture_id;
		GLuint num_vertices;
		GLuint vao;
		GLuint positions_buffer;
		GLuint normals_buffer;
		GLuint uvs_buffer;


		// bounding box variables
		glm::vec3 model_max;
		glm::vec3 center;
		float model_max_length;
		void SetBoundingSphere();
};

