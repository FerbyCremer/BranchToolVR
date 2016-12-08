#pragma once

#include <GL/glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <vector>

#include "DicomReader.h"
#include "ColorObject.h"
#include "TextureObject.h"
#include "Constants.h"

struct BranchPoint 
{
	glm::vec3 position;
	int id;
	std::vector<int> neighbors;
	glm::vec4 color;
	bool is_selected;

	BranchPoint(glm::vec3 _inPos, int _id) 
	{
		position = _inPos;
		id = _id;
		color = glm::vec4(1.0, 0.0f, 0.0f, 1.0f);
		is_selected = false;
	}

	glm::vec4 getColor() 
	{
		if (is_selected) 
		{
			return Constants::SELECTED_BRANCH_POINT_COLOR;
		}
		return Constants::BASE_BRANCH_POINT_COLOR;
	}
};

class DicomPointCloudObject : public AbstractBaseObject
{
	public:
		DicomPointCloudObject();
		~DicomPointCloudObject();
		void GenerateDicomPointCloud(DicomSet & _ds, int isovalue, int max_tolerance);
		bool TestCollision(glm::vec3 _ray, glm::vec3 _pos, glm::vec3 & _cp);
		void Clear();
		void Load();
		int Type();
		int curr_tolerance;

		glm::vec3 lower_bounds;
		glm::vec3 upper_bounds;
		glm::vec3 voxel_scale;
		ColorObject * branch_point_marker;
		std::vector<BranchPoint> branch_points;
		int current_bp_selection;
		BranchPoint* GetBranchPointByID(int id);

	//private:
		TextureObject * handle;
		ColorObject * bounding_cube;
		unsigned int num_vertices;
		unsigned int num_instances;
		bool first_load;
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
