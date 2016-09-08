#pragma once

#include <GL/glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <vector>

#include "DicomReader.h"
#include "ColorObject.h"

static glm::vec4 base_branch_point_color = glm::vec4(1.0f,0.2f,0.4f,1.0f);
static glm::vec4 selected_branch_point_color = glm::vec4(0.9f, 0.3f, 0.9f, 1.0f);

struct BranchPoint {
	glm::vec3 position;
	int id;
	std::vector<int> neighbors;
	glm::vec4 color;
	bool is_selected;
	BranchPoint(glm::vec3 _inPos, int _id) {
		position = _inPos;
		id = _id;
		color = glm::vec4(1.0, 0.0f, 0.0f, 1.0f);
		is_selected = false;
	}

	glm::vec4 getColor() {
		if (is_selected) {
			return selected_branch_point_color;
		}
		return base_branch_point_color;
	}
};

class DicomPointCloudObject : public AbstractBaseObject{

	public:

		DicomPointCloudObject();
		~DicomPointCloudObject();
		void GenerateDicomPointCloud(DicomSet & _ds, int isovalue, int max_tolerance);
		void SetWorldPosition(glm::vec3 _inWpos);
		void SetModelPosition(glm::vec3 _inMpos);
		void SetModelOrientation(glm::vec3 _inOri);
		void SetAppendPose(glm::mat4 _inPose);
		bool TestCollision(glm::vec3 _ray, glm::vec3 _pos, glm::vec3 & _cp);
		void Clear();
		void Load();
		int Type();
		int curr_tolerance;
		void Interact(glm::mat4& _controllerPose, glm::vec3& _ray, glm::vec3& _pos, bool _pressed);

		glm::vec3 lower_bounds;
		glm::vec3 upper_bounds;
		glm::vec3 box_scale;
		glm::vec3 voxel_scale;
		//float scale;
		ColorObject * branch_point_marker;
		std::vector<BranchPoint> branch_points;
		int current_bp_selection;
		BranchPoint* GetBranchPointByID(int id);


	//private:
		ColorObject * handle;
		unsigned int num_vertices;
		unsigned int num_instances;
		bool is_loaded;
		// vertex attributes for point cloud shader
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texture_coords;
		std::vector<glm::vec3> instanced_positions;
		std::vector<int>   states;
		std::vector<GLfloat>   isovalue_differences;

		GLuint vao;
		GLuint positions_buffer;
		GLuint normals_buffer;
		GLuint texture_coords_buffer;
		GLuint instanced_positions_buffer;
		GLuint states_buffer;
		GLuint isovalue_differences_buffer;

		void GenerateCube(glm::vec3 _scale, glm::vec3 _offset);
		void GenerateSphere(float _scale);
};
