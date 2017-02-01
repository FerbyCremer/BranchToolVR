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
	static int id_counter;

	BranchPoint(glm::vec3 _inPos)
	{
		position = _inPos;
		id = id_counter++;
		is_selected = false;
	}

	BranchPoint()
	{
		position = glm::vec3(0.0f);
		id = id_counter++;
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

struct IsovaluePointCloudSlider
{
	ColorObject* frame;
	ColorObject* knob;
	ColorObject* x_button;
	TextureObject* value_label;
	int isovalue;
	const int id;
	static int id_counter;

	IsovaluePointCloudSlider(int _isovalue) : isovalue(_isovalue), id(id_counter++)
	{
		frame = new ColorObject;
		knob = new ColorObject;
		x_button = new ColorObject;

		frame->GenerateIsovaluePointSliderFrame();
		knob->GenerateIsovaluePointSliderKnob();
		knob->SetDisplayColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		x_button->GenerateXYPlane(0.5f, 0.5f, 0.0f, glm::vec3(0.25f));
	}

	void SetModelPosition(const glm::vec3& _pos)
	{
		frame->SetModelPosition(_pos);
		knob->SetModelPosition(_pos);
		x_button->SetModelPosition(_pos);
	}

	~IsovaluePointCloudSlider()
	{
		delete frame;
		delete knob;
		delete x_button;
	}
};

class DicomPointCloudObject : public AbstractBaseObject
{
	public:
		DicomPointCloudObject();
		~DicomPointCloudObject();
		void Generate(DicomSet & _ds, int isovalue, int max_tolerance);
		void SetMasterAppendPose(glm::mat4 _in);
		void Clear();
		int Type();	
		const GLuint GetNumInstances() { return num_instances; }
		BranchPoint* GetBranchPointByID(int id);	
		
		bool first_load;	
		int curr_tolerance;
		glm::vec3 lower_bounds;
		glm::vec3 upper_bounds;
		glm::vec3 voxel_scale;		
		
		TextureObject * handle;		
		ColorObject* branch_point_marker;
		std::vector<BranchPoint*> branch_points;
		int current_bp_selection;		
		static std::vector<IsovaluePointCloudSlider*> isovalue_point_cloud_sliders;
		static const int max_nr_isovalue_point_cloud_sliders;

	private:		
		void GenerateCube(glm::vec3 _scale, glm::vec3 _offset);
		void GenerateSphere(float _scale);	
		void Load();

		unsigned int num_instances;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> instanced_positions;
		std::vector<int> states;
		std::vector<GLfloat> isovalue_differences;

		GLuint normals_buffer;
		GLuint texture_coords_buffer;
		GLuint instanced_positions_buffer;
		GLuint states_buffer;
		GLuint isovalue_differences_buffer;
};
