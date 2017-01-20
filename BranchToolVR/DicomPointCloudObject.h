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
	bool is_selected;
	static int id_assigner;

	BranchPoint(glm::vec3 _inPos)
	{
		position = _inPos;
		id = id_assigner++;
		is_selected = false;
	}

	BranchPoint()
	{
		position = glm::vec3(0.0f);
		id = id_assigner++;
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
		void SetAppendPose(glm::mat4 _in);
		void Clear();
		void Load();
		void GenerateCube(glm::vec3 _scale, glm::vec3 _offset);
		void GenerateSphere(float _scale);
		bool TestCollision(glm::vec3 _ray, glm::vec3 _pos, glm::vec3 & _cp);		
		int Type();	
		BranchPoint* GetBranchPointByID(int id);	

		int curr_tolerance;
		glm::vec3 lower_bounds;
		glm::vec3 upper_bounds;
		glm::vec3 voxel_scale;
		ColorObject * branch_point_marker;
		std::vector<BranchPoint*> branch_points;
		int current_bp_selection;
		

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
};
