#include "DicomPointCloudObject.h"

DicomPointCloudObject::DicomPointCloudObject(){
	curr_tolerance = DEFAULT_ISOVALUE_TOLERANCE;
	is_loaded = false;
	handle = new ColorObject;
	handle->is_selectable = true;
	handle->SetDisplayColor(glm::vec4(0.2f, 0.2f, 0.2f,1.0f));
	handle->GenerateSphere(11, 0.1f, false);
	Set_scale(glm::vec3(2.0f, 2.0f, 2.0f));
	//scale = 1.0f;
	branch_point_marker = new ColorObject;
	branch_point_marker->SetDisplayColor(glm::vec4(1.0f, 0.1f, 0.2f, 1.0f));
	current_bp_selection = -1;
}

DicomPointCloudObject::~DicomPointCloudObject(){

}

void DicomPointCloudObject::Clear() {
	num_instances = 0;
}

int DicomPointCloudObject::Type() {
	return 2;
}

void DicomPointCloudObject::GenerateCube(glm::vec3 _scale,glm::vec3 _offset) {

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

	for (unsigned int i = 0; i < positions.size(); ++i) {
		positions[i].z += 1.0f;
		positions[i] += _offset;
		positions[i] *= _scale;
	}
}

void DicomPointCloudObject::SetWorldPosition(glm::vec3 _inWpos) {
	Set_world_position(_inWpos);
	handle->Set_world_position(_inWpos);
}

void DicomPointCloudObject::SetModelPosition(glm::vec3 _inMpos) {
	Set_model_position(_inMpos);
	handle->Set_model_position(_inMpos);
}

void DicomPointCloudObject::SetModelOrientation(glm::vec3 _inOri) {
	Set_model_orientation(_inOri);
	handle->Set_model_orientation (_inOri);
}

void DicomPointCloudObject::SetAppendPose(glm::mat4 _inPose) {
	Set_append_pose(_inPose);
	handle->Set_append_pose(_inPose);
}

void DicomPointCloudObject::Interact(glm::mat4& _controllerPose, glm::vec3& _ray, glm::vec3& _pos, bool _pressed) {

}

void DicomPointCloudObject::GenerateSphere(float _scale) {
	int resolution = 5;
	int num_vertices = resolution * resolution;
	float t_inc1 = 3.145 / (float)(resolution - 1);
	float t_inc2 = 6.28 / (float)(resolution);
	glm::vec3* vertices = new glm::vec3[num_vertices];

	for (int i = 0; i < num_vertices; ++i) {
		float layer = i / resolution;
		float curr_radius = sin(t_inc1*layer);
		float t = t_inc2*(float)((resolution - 1) - (i % resolution));
		vertices[i] = _scale*glm::vec3(curr_radius*cos(t), -1.0f*cos(t_inc1*layer), curr_radius*sin(t));
	}

	int num_patches = resolution * (resolution - 1);
	for (int i = 0; i < num_patches; ++i) {

		int x = i % resolution;
		int y = i / resolution;

		if (y != 0) {
			positions.push_back(vertices[i]);
			positions.push_back(vertices[(x + 1) % resolution + y*resolution]);
			positions.push_back(vertices[(x + 1) % resolution + (y + 1)*resolution]);

			normals.push_back(vertices[i]);
			normals.push_back(vertices[(x + 1) % resolution + y*resolution]);
			normals.push_back(vertices[(x + 1) % resolution + (y + 1)*resolution]);
		}
		
		if (y != resolution - 1) {
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

void DicomPointCloudObject::GenerateDicomPointCloud(DicomSet & _ds, int _isovalue, int max_tolerance) {

	if (&_ds == NULL || _ds.data.size() < 1) {
		return;
	}

	voxel_scale = glm::vec3(_ds.scale.x / (float)_ds.data[0].width, _ds.scale.y / (float)_ds.data[0].height, _ds.scale.z / (float)_ds.data.size());
	branch_point_marker->GenerateSphere(10, voxel_scale.x*3.0f, false);
	
	if (!is_loaded) {
		//GenerateCube(voxel_scale, glm::vec3(-0.5f,-0.5f,-0.5f));
		GenerateSphere(voxel_scale.x);
	}
	else {
		instanced_positions.clear();
		isovalue_differences.clear();
		states.clear();
	}
	
	for (int i = 0; i < _ds.data.size(); ++i) {
		for (int j = 0; j < _ds.data[i].isovalues.size(); ++j) {
			short iso_abs_check = abs(_ds.data[i].isovalues[j] - _isovalue);
			if (iso_abs_check <= max_tolerance) {
				glm::vec3 instanced_position = glm::vec3((float)(j % _ds.data[0].width)*voxel_scale.x, float(j / _ds.data[0].width)*voxel_scale.y, (float)i * voxel_scale.z);

				if (instanced_position.x > lower_bounds.x && instanced_position.y > lower_bounds.y && instanced_position.z > lower_bounds.z
				&& instanced_position.x < upper_bounds.x && instanced_position.y < upper_bounds.y && instanced_position.z < upper_bounds.z) {
					instanced_positions.push_back(instanced_position - lower_bounds);
					isovalue_differences.push_back(iso_abs_check);
					states.push_back(i*_ds.data[i].isovalues.size()+j);
				}
			}
		}
	}

	Load();
	is_loaded = true;
}

void DicomPointCloudObject::Load() {

	/* dicom point cloud shader layouts
	"layout(location = 0) in vec3 position;\n"
	"layout(location = 1) in vec3 normal;\n"
	"layout(location = 2) in vec3 instanced_position;\n"
	"layout(location = 3) in float state;\n"
	"layout(location = 4) in float iso_diff;\n"
	*/		

	num_vertices = positions.size();
	num_instances = instanced_positions.size();

	if (!is_loaded) {

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &positions_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*positions.size(), &positions[0], GL_STATIC_DRAW);

		glGenBuffers(1, &normals_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normals.size(), &normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &instanced_positions_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_positions_buffer);
		if (instanced_positions.size() > 0)
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*instanced_positions.size(), &instanced_positions[0], GL_STATIC_DRAW);

		glGenBuffers(1, &states_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, states_buffer);
		if (states.size() > 0)
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*states.size(), &states[0], GL_STATIC_DRAW);

		glGenBuffers(1, &isovalue_differences_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, isovalue_differences_buffer);
		if (isovalue_differences.size() > 0)
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*isovalue_differences.size(), &isovalue_differences[0], GL_STATIC_DRAW);

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
	else {

		glBindBuffer(GL_ARRAY_BUFFER, instanced_positions_buffer);
		if (instanced_positions.size() > 0)
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*instanced_positions.size(), &instanced_positions[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, states_buffer);
		if (states.size() > 0)
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*states.size(), &states[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, isovalue_differences_buffer);
		if (isovalue_differences.size() > 0)
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*isovalue_differences.size(), &isovalue_differences[0], GL_STATIC_DRAW);
	}
}

bool DicomPointCloudObject::TestCollision(glm::vec3 _ray, glm::vec3 _pos, glm::vec3 & _cp) {

	glm::mat4 mm = GetModelMatrix();
	glm::vec3 cg;
	glm::vec3 b = glm::normalize(_ray);
	float dist_threshold = voxel_scale.x * 5.5f;
	float curr_min_dist = 0.0f;
	int curr_min_index = -1;
	bool first_collision = true;

	for (int i = 0; i < branch_points.size(); i++) {
		//branch_points[i].color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		glm::vec3 real_ipos = glm::vec3(mm*glm::vec4(scale*(branch_points[i].position - lower_bounds), 1.0f));
		glm::vec3 a = real_ipos - _pos;
		float x = glm::dot(b, a);
		glm::vec3 y = a - (x*b);
		float dist1 = glm::length(y);
		if (x > 0 && dist1 <= (voxel_scale.x *3.5f*scale.x) && (x < curr_min_dist || first_collision)) {
			//branch_points[i].color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			curr_min_dist = x;
			curr_min_index = i;
			_cp = real_ipos;
			first_collision = false;
		}
	}
	
	if (!first_collision) {
		for (int i = 0; i < branch_points.size(); i++) {
			branch_points[i].is_selected = false;
		}
		// branch point collision
		current_bp_selection = branch_points[curr_min_index].id;
		branch_points[curr_min_index].is_selected = true;
		return true;
	}

	for (int i = 0; i < instanced_positions.size(); i++) {
		if (isovalue_differences[i] < curr_tolerance) {
			glm::vec3 real_ipos = glm::vec3(mm*glm::vec4(scale*(instanced_positions[i]),1.0f));
			glm::vec3 a = real_ipos - _pos;
			float x = glm::dot(b, a);
			glm::vec3 y = a - (x*b);
			float dist1 = glm::length(y);
			if (x > 0 && dist1 <= dist_threshold && (x < curr_min_dist || first_collision)) {
				curr_min_dist = x;
				curr_min_index = i;
				_cp = real_ipos;
				first_collision = false;
			}
		}
	}

	if (!first_collision) {
		// new branch point
		BranchPoint newBP(instanced_positions[curr_min_index] + lower_bounds, states[curr_min_index]);
		for (int i = 0; i < branch_points.size(); ++i) {
			if (branch_points[i].id == current_bp_selection) {
				branch_points[i].neighbors.push_back(newBP.id);
				newBP.neighbors.push_back(branch_points[i].id);
			}
		}

		for (int i = 0; i < branch_points.size(); i++) {
			branch_points[i].is_selected = false;
		}
		newBP.is_selected = true;
		current_bp_selection = states[curr_min_index];

		branch_points.push_back(newBP);
		return true;
	}

	return false;
}

BranchPoint* DicomPointCloudObject::GetBranchPointByID(int _id) {
	for (int i = 0; i < branch_points.size(); ++i) {
		if (branch_points[i].id == _id) {
			return &branch_points[i];
		}
	}

	return NULL;
}