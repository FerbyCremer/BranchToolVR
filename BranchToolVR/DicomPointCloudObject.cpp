#include "DicomPointCloudObject.h"

DicomPointCloudObject::DicomPointCloudObject()
{
	// default values
	is_selectable = true;
	scale = Constants::DEFAULT_POINT_CLOUD_SCALE;
	curr_tolerance = DEFAULT_ISOVALUE_TOLERANCE;
	first_load = false;
	current_bp_selection = -1;
	lower_bounds = glm::vec3(0.5f, 0.5f, 0.5f);
	upper_bounds = lower_bounds + glm::vec3(Constants::DEFAULT_SELECTOR_SCALE);

	// init objects
	handle = new TextureObject;
	handle->is_selectable = true;
	handle->is_double_selectable = true;
	handle->readObjFromFile(DirectoryInfo::POINT_CLOUD_HANDLE_MODEL, Constants::DEFAULT_POINT_CLOUD_SCALE, 0.5f*glm::vec3(Constants::DEFAULT_POINT_CLOUD_SCALE, 0.0f, Constants::DEFAULT_POINT_CLOUD_SCALE));

	branch_point_marker = new ColorObject;
	branch_point_marker->SetDisplayColor(glm::vec4(1.0f, 0.1f, 0.2f, 1.0f));

	bounding_cube = new ColorObject;
	bounding_cube->GenerateXYPrism(glm::vec3(Constants::DEFAULT_POINT_CLOUD_SCALE), glm::vec2(0.0f), glm::vec3());

	// set default transform
	//Set_scale(Constants::DEFAULT_POINT_CLOUD_SCALE);
}

DicomPointCloudObject::~DicomPointCloudObject()
{
}

void DicomPointCloudObject::Clear() 
{
	num_instances = 0;
}

int DicomPointCloudObject::Type() 
{
	return 2;
}

void DicomPointCloudObject::Load()
{
	/* dicom point cloud shader layouts
	"layout(location = 0) in vec3 position;\n"
	"layout(location = 1) in vec3 normal;\n"
	"layout(location = 2) in vec3 instanced_position;\n"
	"layout(location = 3) in float state;\n"
	"layout(location = 4) in float iso_diff;\n"
	*/		

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
		glGenBuffers(1, &states_buffer);
		glGenBuffers(1, &isovalue_differences_buffer);

		if (instanced_positions.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, instanced_positions_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*instanced_positions.size(), &instanced_positions[0], GL_STATIC_DRAW);
		}

		if (states.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, states_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*states.size(), &states[0], GL_STATIC_DRAW);
		}

		if (isovalue_differences.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, isovalue_differences_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*isovalue_differences.size(), &isovalue_differences[0], GL_STATIC_DRAW);
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
		glBindBuffer(GL_ARRAY_BUFFER, states_buffer);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, isovalue_differences_buffer);
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

		if (states.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, states_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*states.size(), &states[0], GL_STATIC_DRAW);
		}

		if (isovalue_differences.size() > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, isovalue_differences_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*isovalue_differences.size(), &isovalue_differences[0], GL_STATIC_DRAW);
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

void DicomPointCloudObject::GenerateDicomPointCloud(DicomSet & _ds, int _isovalue, int max_tolerance) 
{
	if (&_ds == NULL || _ds.data.size() < 1)
	{
		return;
	}

	float box_diag_len = glm::length(upper_bounds - lower_bounds);
	Set_scale(Constants::SQRT3 / box_diag_len);
	
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
		isovalue_differences.clear();
		states.clear();
	}
	
	// loop through dicom data and add points that are within the current isovalue tolerance
	for (int i = 0; i < _ds.data.size(); ++i) 
	{
		for (int j = 0; j < _ds.data[i].isovalues.size(); ++j) 
		{
			short iso_abs_check = abs(_ds.data[i].isovalues[j] - _isovalue);

			if (iso_abs_check <= max_tolerance) 
			{
				glm::vec3 instanced_position = glm::vec3((float)(j % _ds.data[0].width), float(j / _ds.data[0].width), (float)i) * voxel_scale;

				if (instanced_position.x > lower_bounds.x && instanced_position.y > lower_bounds.y && instanced_position.z > lower_bounds.z
				&& instanced_position.x < upper_bounds.x && instanced_position.y < upper_bounds.y && instanced_position.z < upper_bounds.z) 
				{
					instanced_positions.push_back(instanced_position - lower_bounds);
					isovalue_differences.push_back(iso_abs_check);
					states.push_back(i*_ds.data[i].isovalues.size()+j);
				}
			}
		}
	}

	Load();	
	first_load = true;
}

bool DicomPointCloudObject::TestCollision(glm::vec3 _ray, glm::vec3 _pos, glm::vec3 & _cp)
{
	// test if there is a collision with the bounding cube
	if (!bounding_cube->TestCollision(_ray, _pos, _cp))
	{
		return false;
	}
	
	glm::mat4 mm = GetModelMatrix();
	glm::vec3 cg;
	glm::vec3 b = glm::normalize(_ray);
	float dist_threshold = voxel_scale.x * 5.5f;
	float curr_min_dist = 0.0f;
	int curr_min_index = -1;
	bool first_collision = true;

	// check existing branch points
	for (int i = 0; i < branch_points.size(); i++) 
	{
		glm::vec3 real_ipos = glm::vec3(mm*glm::vec4((branch_points[i].position - lower_bounds), 1.0f));
		glm::vec3 a = real_ipos - _pos;
		float x = glm::dot(b, a);
		glm::vec3 y = a - (x*b);
		float dist1 = glm::length(y);

		if (x > 0 && dist1 <= (voxel_scale.x*3.5f*scale) && (x < curr_min_dist || first_collision)) 
		{
			curr_min_dist = x;
			curr_min_index = i;
			_cp = real_ipos;
			first_collision = false;
		}
	}
	
	// found a branch point collision
	if (!first_collision) 
	{
		for (int i = 0; i < branch_points.size(); i++) 
		{
			branch_points[i].is_selected = false;
		}

		current_bp_selection = branch_points[curr_min_index].id;
		branch_points[curr_min_index].is_selected = true;
		return true;
	}

	// no branch point collisions, check the point cloud
	glm::vec3 voxel_midpoint = voxel_scale * 0.5f;
	for (int i = 0; i < instanced_positions.size(); i++) 
	{
		if (isovalue_differences[i] < curr_tolerance) 
		{
			glm::vec3 real_ipos = glm::vec3(mm*glm::vec4((instanced_positions[i] + voxel_midpoint), 1.0f));
			glm::vec3 a = real_ipos - _pos;
			float x = glm::dot(b, a);
			glm::vec3 y = a - (x*b);
			float dist1 = glm::length(y);

			if (x > 0 && dist1 <= dist_threshold && (x < curr_min_dist || first_collision)) 
			{
				curr_min_dist = x;
				curr_min_index = i;
				_cp = real_ipos;
				first_collision = false;
			}
		}
	}

	// new isovalue point selected, connect to branch point graph
	if (!first_collision) 
	{
		// new branch point
		BranchPoint newBP(instanced_positions[curr_min_index] + lower_bounds, states[curr_min_index]);

		for (int i = 0; i < branch_points.size(); ++i) 
		{
			if (branch_points[i].id == current_bp_selection)
			{
				branch_points[i].neighbors.push_back(newBP.id);
				newBP.neighbors.push_back(branch_points[i].id);
			}
		}

		for (int i = 0; i < branch_points.size(); i++) 
		{
			branch_points[i].is_selected = false;
		}

		newBP.is_selected = true;
		current_bp_selection = states[curr_min_index];
		branch_points.push_back(newBP);
		return true;
	}

	return false;
}

BranchPoint* DicomPointCloudObject::GetBranchPointByID(int _id) 
{
	for (int i = 0; i < branch_points.size(); ++i) 
	{
		if (branch_points[i].id == _id) 
		{
			return &branch_points[i];
		}
	}

	return NULL;
}