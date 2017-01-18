#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MiscFunctions.h"
#include "Constants.h"

enum ObjectTypes
{
	COLOR_OBJECT,
	TEXTURE_OBJECT,
	LINE_OBJECT,
	DICOM_POINT_CLOUD_OBJECT
};

struct InteractionCache
{
	glm::mat4 to_controller_space_initial;
	glm::mat4 controller_pose_initial;
	glm::mat4 controller_pose_updated;
	glm::mat4 append_pose_initial;
	glm::vec3 controller_world_position_initial;

	// dual gesture variables	
	glm::vec3 primary_collision_point_world_initial;
	glm::vec3 primary_collision_point_world_current;
	glm::vec3 primary_collision_point_controller_space_initial;
	glm::vec3 secondary_collision_point_world_initial;
	glm::vec3 secondary_collision_point_world_current;
	glm::vec3 secondary_collision_point_controller_space_initial;
	glm::vec3 primary_to_secondary_collision_point_initial;
	glm::vec3 primary_to_secondary_collision_point_current;
	float initial_scale;
};

class AbstractBaseObject
{
	public:
		AbstractBaseObject();
		~AbstractBaseObject();

		virtual void Load() = 0;
		virtual int Type() = 0;
		virtual void SetSelected(bool _selected) { is_selected = _selected; };

		bool is_loaded;
		bool is_hidden;
		bool is_selectable;
		bool is_clickable;
		bool is_clicked;
		bool is_selected;
		bool is_double_selected;
		bool is_double_selectable;
		int id;
		short ui_quadrant;
		int level;
		int controllerSelectorId;
		int controllerSelectorIdPrev;
		glm::mat4 ui_transform;
		static int id_counter;
		glm::mat4 GetModelMatrix();
		void SetClick();
		bool WasClicked();
		glm::mat4 model_matrix;
		glm::mat4 base_model_matrix;
		glm::mat4 ui_model_matrix;

		void Set_world_position(const glm::vec3& v);
		void Set_model_orientation(const glm::vec3& v);
		void Set_model_positionX(float _x);
		void Set_model_positionY(const float _y);
		void Set_model_positionZ(const float _z);
		void Set_model_position(const glm::vec3& v);
		void Set_scale(const float & _scale);
		void Set_append_pose(const glm::mat4& m);
		void ResetPositionAndRotation();

		virtual bool TestCollision(glm::vec3 _ray, glm::vec3 _pos, glm::vec3 & _collisionPoint);
		bool TestBoundingSphere(glm::vec3 _ray, glm::vec3 _pos);
		glm::mat4 getDoubleSelectionTransform();
		glm::mat4 getDoubleSelectionScaleDifference();

	//private:

		// model matrix components
		glm::vec3 world_position;
		glm::vec3 model_orientation;
		glm::vec3 model_position;
		float scale;
		glm::mat4 append_pose;
		InteractionCache cache;
		void CalcModelMatrix();

		// vertex data
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

		// bounding sphere variables
		glm::vec3 model_max;
		glm::vec3 center;
		float model_max_length;
		void SetBoundingSphere();
};

