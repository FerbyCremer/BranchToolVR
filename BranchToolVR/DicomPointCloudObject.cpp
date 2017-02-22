#include "DicomPointCloudObject.h"

int BranchPoint::id_counter = 0;
int IsovaluePointCloudSlider::id_counter = 0;
const int DicomPointCloudObject::max_nr_isovalue_point_cloud_sliders = MAX_NR_POINT_CLOUD_SLIDERS;
glm::vec2 IsovaluePointCloudSlider::frame_scale = glm::vec2(1.0f, 0.25f);
glm::vec2 IsovaluePointCloudSlider::knob_scale = glm::vec2(IsovaluePointCloudSlider::frame_scale.y);
float IsovaluePointCloudSlider::knob_travel_dist = IsovaluePointCloudSlider::frame_scale.x - IsovaluePointCloudSlider::knob_scale.x;
float IsovaluePointCloudSlider::min_isovalue = TMP_MIN_ISOVALUE;
float IsovaluePointCloudSlider::max_isovalue = TMP_MAX_ISOVALUE;

DicomPointCloudObject::DicomPointCloudObject() : isovalue_point_cloud_sliders(MAX_NR_POINT_CLOUD_SLIDERS)
{
	// default values
	is_selectable = true;
	curr_tolerance = DEFAULT_ISOVALUE_TOLERANCE;
	first_load = false;
	current_bp_selection = -1;
	lower_bounds = glm::vec3(0.5f, 0.5f, 0.5f);
	upper_bounds = lower_bounds + glm::vec3(DEFAULT_SELECTOR_SCALE);

	// init objects
	handle = new TextureObject;
	handle->is_selectable = true;
	handle->is_double_selectable = true;
	handle->readObjFromFile(DirectoryInfo::POINT_CLOUD_HANDLE_MODEL, DEFAULT_POINT_CLOUD_SCALE, 0.5f*glm::vec3(DEFAULT_POINT_CLOUD_SCALE, 0.0f, DEFAULT_POINT_CLOUD_SCALE));
	handle->texture_level = 3; //TODO: temp set POINT_CLOUD_FRAME_TEXTURE

	branch_point_marker = new ColorObject;
	branch_point_marker->SetDisplayColor(glm::vec4(1.0f, 0.1f, 0.2f, 1.0f));

	for(int i = 0; i < isovalue_point_cloud_sliders.size(); ++i)
	{
		isovalue_point_cloud_sliders[i] = new IsovaluePointCloudSlider;
		isovalue_point_cloud_sliders[i]->SetModelPosition(glm::vec3(0.0f, 0.3f*(float)i, 0.0f));
	}
}

DicomPointCloudObject::~DicomPointCloudObject()
{
	delete handle;
	delete branch_point_marker;
}

void DicomPointCloudObject::Clear() 
{
	num_instances = 0;
}

void DicomPointCloudObject::AddNewIsovaluePointCloudSlider(int _isovalue)
{
	bool unused_slider_slot_found = false;

	for (int i = 0; i < isovalue_point_cloud_sliders.size(); ++i)
	{
		if (!isovalue_point_cloud_sliders[i]->in_use)
		{
			isovalue_point_cloud_sliders[i]->SetInUse(true);

			unused_slider_slot_found = true;
			break;
		}
	}
}

void DicomPointCloudObject::SetIsovaluePointCloudSlidersHeights()
{
}

int DicomPointCloudObject::Type() 
{
	return AbstractBaseObject::get_type_id<DicomPointCloudObject>();
}

void DicomPointCloudObject::Load()
{
	num_vertices = positions.size();
	num_instances = instanced_positions.size();

	if (!first_load)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &positions_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*positions.size(), &positions[0], GL_STATIC_DRAW);

		glGenBuffers(1, &normals_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normals.size(), &normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &instanced_positions_buffer);
		glGenBuffers(1, &instanced_colors_buffer);
		glGenBuffers(1, &instanced_isovalue_differences_buffer);

		if (instanced_positions.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanced_positions_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*instanced_positions.size(), &instanced_positions[0], GL_STATIC_DRAW);
		}

		if (instanced_colors.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanced_colors_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*instanced_colors.size(), &instanced_colors[0], GL_STATIC_DRAW);
		}

		if (instanced_isovalue_differences.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanced_isovalue_differences_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*instanced_isovalue_differences.size(), &instanced_isovalue_differences[0], GL_STATIC_DRAW);
		}

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_positions_buffer);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_colors_buffer);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_isovalue_differences_buffer);
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
	}
	else
	{
		if (instanced_positions.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanced_positions_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*instanced_positions.size(), &instanced_positions[0], GL_STATIC_DRAW);
		}

		if (instanced_colors.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanced_colors_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*instanced_colors.size(), &instanced_colors[0], GL_STATIC_DRAW);
		}

		if (instanced_isovalue_differences.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanced_isovalue_differences_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*instanced_isovalue_differences.size(), &instanced_isovalue_differences[0], GL_STATIC_DRAW);
		}
	}
}

void DicomPointCloudObject::GenerateCube(glm::vec3 _scale,glm::vec3 _offset) 
{
	glm::vec3 cube[8];
	cube[0] = glm::vec3(0.0f, 0.0f, 0.0f);
	cube[1] = glm::vec3(1.0f, 0.0f, 0.0f);
	cube[2] = glm::vec3(0.0f, 0.0f, -1.0f);
	cube[3] = glm::vec3(1.0f, 0.0f, -1.0f);
	cube[4] = glm::vec3(0.0f, 1.0f, 0.0f);
	cube[5] = glm::vec3(1.0f, 1.0f, 0.0f);
	cube[6] = glm::vec3(0.0f, 1.0f, -1.0f);
	cube[7] = glm::vec3(1.0f, 1.0f, -1.0f);

	// front
	positions.push_back(cube[0]);
	positions.push_back(cube[1]);
	positions.push_back(cube[5]);
	positions.push_back(cube[0]);
	positions.push_back(cube[5]);
	positions.push_back(cube[4]);

	// back
	positions.push_back(cube[3]);
	positions.push_back(cube[2]);
	positions.push_back(cube[6]);
	positions.push_back(cube[3]);
	positions.push_back(cube[6]);
	positions.push_back(cube[7]);

	// left side
	positions.push_back(cube[2]);
	positions.push_back(cube[0]);
	positions.push_back(cube[4]);
	positions.push_back(cube[2]);
	positions.push_back(cube[4]);
	positions.push_back(cube[6]);

	// right side
	positions.push_back(cube[1]);
	positions.push_back(cube[3]);
	positions.push_back(cube[7]);
	positions.push_back(cube[1]);
	positions.push_back(cube[7]);
	positions.push_back(cube[5]);

	// top
	positions.push_back(cube[4]);
	positions.push_back(cube[5]);
	positions.push_back(cube[7]);
	positions.push_back(cube[4]);
	positions.push_back(cube[7]);
	positions.push_back(cube[6]);

	// bottom
	positions.push_back(cube[1]);
	positions.push_back(cube[0]);
	positions.push_back(cube[2]);
	positions.push_back(cube[1]);
	positions.push_back(cube[2]);
	positions.push_back(cube[3]);

	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));

	normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));

	normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));

	for (unsigned int i = 0; i < positions.size(); ++i) 
	{
		positions[i].z += 1.0f;
		positions[i] += _offset;
		positions[i] *= _scale;
	}
}

void DicomPointCloudObject::GenerateSphere(float _scale) 
{
	int resolution = 5;
	int num_vertices = resolution * resolution;
	float t_inc1 = 3.145 / (float)(resolution - 1);
	float t_inc2 = 6.28 / (float)(resolution);
	glm::vec3* vertices = new glm::vec3[num_vertices];

	for (int i = 0; i < num_vertices; ++i) 
	{
		float layer = i / resolution;
		float curr_radius = sin(t_inc1*layer);
		float t = t_inc2*(float)((resolution - 1) - (i % resolution));
		vertices[i] = _scale*glm::vec3(curr_radius*cos(t), -1.0f*cos(t_inc1*layer), curr_radius*sin(t));
	}

	int num_patches = resolution * (resolution - 1);
	for (int i = 0; i < num_patches; ++i) 
	{
		int x = i % resolution;
		int y = i / resolution;

		if (y != 0) 
		{
			positions.push_back(vertices[i]);
			positions.push_back(vertices[(x + 1) % resolution + y*resolution]);
			positions.push_back(vertices[(x + 1) % resolution + (y + 1)*resolution]);

			normals.push_back(vertices[i]);
			normals.push_back(vertices[(x + 1) % resolution + y*resolution]);
			normals.push_back(vertices[(x + 1) % resolution + (y + 1)*resolution]);
		}
		
		if (y != resolution - 1) 
		{
			positions.push_back(vertices[i]);
			positions.push_back(vertices[(x + 1) % resolution + (y + 1)*resolution]);
			positions.push_back(vertices[x % resolution + (y + 1)*resolution]);

			normals.push_back(vertices[i]);
			normals.push_back(vertices[(x + 1) % resolution + (y + 1)*resolution]);
			normals.push_back(vertices[x % resolution + (y + 1)*resolution]);
		}
	}

	Load();
}

void DicomPointCloudObject::Generate(DicomSet & _ds, int _isovalue, int max_tolerance) 
{
	if (&_ds == NULL || _ds.data.size() < 1)
	{
		return;
	}

	float box_diag_len = glm::length(upper_bounds - lower_bounds);
	SetScale(SQRT3 / box_diag_len);
	
	int f = 0;
	voxel_scale =  glm::vec3(_ds.scale.x / (float)(_ds.data[0].width+f), _ds.scale.y / (float)(_ds.data[0].height+f), _ds.scale.z / ((float)_ds.data.size()+f));
	branch_point_marker->GenerateSphere(10, voxel_scale.x*3.0f, false);
	
	if (!first_load)
	{
		GenerateCube(voxel_scale, glm::vec3(0.0f));
		// old generate sphere for picking points
		//GenerateSphere(voxel_scale.x);
	}
	else
	{
		instanced_positions.clear();
		instanced_isovalue_differences.clear();
		instanced_colors.clear();
	}
	
	// loop through dicom data and add points that are within the current isovalue tolerance, needs optimization

	for (int i = 0; i < _ds.data.size(); ++i) 
	{
		for (int j = 0; j < _ds.data[i].isovalues.size(); ++j) 
		{
			glm::vec3 col = glm::vec3(1.0f,0.0f,1.0f);
			short iso_abs_check;
			bool found = false;

			for (int k = 0; k < isovalue_point_cloud_sliders.size(); ++k)
			{
				if (!isovalue_point_cloud_sliders[k]->in_use) continue;

				iso_abs_check = abs(_ds.data[i].isovalues[j] - (short)isovalue_point_cloud_sliders[k]->curr_isovalue);
				if (iso_abs_check <= max_tolerance)
				{
					col = isovalue_point_cloud_sliders[k]->color;
					col = glm::vec3(0.25f, 1.0f, 0.1f);
					found = true;
					break;
				}
			}

			if (found)
			{
				glm::vec3 instanced_position = glm::vec3((float)(j % _ds.data[0].width), float(j / _ds.data[0].width), (float)i) * voxel_scale;

				if (instanced_position.x > lower_bounds.x && instanced_position.y > lower_bounds.y && instanced_position.z > lower_bounds.z
				&& instanced_position.x < upper_bounds.x && instanced_position.y < upper_bounds.y && instanced_position.z < upper_bounds.z) 
				{
					instanced_positions.push_back(instanced_position - lower_bounds);
					instanced_isovalue_differences.push_back(iso_abs_check);
					instanced_colors.push_back(col);
				}
			}
		}
	}

	Load();	
	first_load = true;
}

void DicomPointCloudObject::SetMasterAppendPose(glm::mat4 _in)
{
	SetAppendPose(_in);
	handle->SetAppendPose(_in);
	branch_point_marker->SetAppendPose(_in);

	for (auto*& slider : isovalue_point_cloud_sliders)
	{
		slider->SetAppendPose(_in);
	}
}

void DicomPointCloudObject::SetIsovalueSliderTagAppendPose(const glm::mat4 _in)
{
	for (auto*& slider : isovalue_point_cloud_sliders)
	{
		slider->SetTagsAppendPose(_in);
	}
}

BranchPoint* DicomPointCloudObject::GetBranchPointByID(int _id)  // TODO: switch to id to pointer map
{
	for (int i = 0; i < branch_points.size(); ++i) 
	{
		if (branch_points[i]->id == _id) 
		{
			return branch_points[i];
		}
	}

	return NULL;
}
