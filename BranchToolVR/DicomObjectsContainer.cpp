#include "DicomObjectsContainer.h"

DicomObjectsContainer::DicomObjectsContainer()
{
	points = new DicomPointCloudObject;
	viewer = new CoarseDicomViewer;
	
	// set starting locations
	float initial_scale = 0.5f;
	glm::vec3 initial_position = glm::vec3(0.5f, 0.25f, 0.5f);
	glm::mat4 tmp_initial_model_matrix = glm::translate(glm::mat4(1.0f), initial_position) * glm::scale(glm::mat4(1.0f), glm::vec3(initial_scale));
	viewer->SetMasterAppendPose(tmp_initial_model_matrix);
	
	initial_scale = 0.5f;
	initial_position = glm::vec3(-0.5f, 0.25f, 0.5f);
	tmp_initial_model_matrix = glm::translate(glm::mat4(1.0f), initial_position) * glm::scale(glm::mat4(1.0f), glm::vec3(initial_scale));
	points->SetMasterAppendPose(tmp_initial_model_matrix);
}

DicomObjectsContainer::~DicomObjectsContainer()
{
	delete points;
	delete viewer;
}

void DicomObjectsContainer::Update(const VrData& _vr, const CursorData& _crsr)
{
	glm::mat4 curr_pose;

	if (viewer->base_handle->is_double_selected)
	{
		viewer->SetMasterAppendPose(viewer->base_handle->GetDoubleSelectionTransform());
	}	
	else if(viewer->base_handle->is_selected)
	{
		curr_pose = viewer->base_handle->cache.controller_pose_updated * viewer->base_handle->cache.to_controller_space_initial;
		viewer->SetMasterAppendPose(curr_pose);
	}

	static bool once = true;
	static glm::vec3 offset;

	if (viewer->point_cloud_selector->is_selected)
	{
		if (viewer->point_cloud_selector->is_double_selected)
		{
			// TODO: fix position jump when secondary controller becomes primary
			float start_interesection_ray_len = glm::length(viewer->point_cloud_selector->cache.primary_to_secondary_collision_point_initial);
			float curr_intersection_ray_len = glm::length(viewer->point_cloud_selector->cache.primary_to_secondary_collision_point_current);
			float intersection_ray_ratio = curr_intersection_ray_len / start_interesection_ray_len;

			viewer->point_cloud_selector->SetScale(intersection_ray_ratio *viewer->point_cloud_selector->cache.initial_scale);
			viewer->point_cloud_selector_scale = viewer->point_cloud_selector->GetScale();
			once = true;
		}
		else
		{
			glm::vec3 pointer_model_space = glm::vec3(glm::inverse(viewer->base_handle->GetModelMatrix()) 
			* glm::vec4(viewer->point_cloud_selector->cache.primary_collision_point_world_current, 1.0f));

			if (once)
			{
				offset = viewer->point_cloud_selector->GetModelPosition() - pointer_model_space;
				once = false;
			}

			viewer->point_cloud_selector->SetModelPosition(pointer_model_space + offset);
		}
	}
	else
	{
		if (!once)
		{
			// point cloud selector was moved and released
			viewer->selector_lower_bounds = viewer->point_cloud_selector->GetModelPosition() - glm::vec3(0.5f*viewer->point_cloud_selector_scale);
			viewer->selector_upper_bounds = viewer->point_cloud_selector->GetModelPosition() + glm::vec3(0.5f*viewer->point_cloud_selector_scale);
			points->lower_bounds = viewer->selector_lower_bounds;
			points->upper_bounds = viewer->selector_upper_bounds;
			points->curr_tolerance = 30;
			points->Generate(imaging_data, imaging_data.isovalue, 30);
		}

		once = true;
	}

	// TODO: round down model position to nearest slice
	if (viewer->orthoslice_handle->is_selected)
	{
		glm::vec4 colp_to_model_space = glm::inverse(viewer->base_handle->GetModelMatrix()) * glm::vec4(viewer->orthoslice_handle->cache.primary_collision_point_world_current, 1.0f);
		colp_to_model_space.z = glm::clamp(colp_to_model_space.z, 0.0f, imaging_data.scale.z);

		imaging_data.current_index = (float)(imaging_data.data.size() - 1) * (colp_to_model_space.z / imaging_data.scale.z);
		viewer->orthoslice_texture->Load(imaging_data.data[imaging_data.current_index], imaging_data.window_width, imaging_data.window_center);

		viewer->orthoslice_handle->SetModelPositionZ(colp_to_model_space.z);
		viewer->orthoslice->SetModelPositionZ(colp_to_model_space.z);
	}

	if (points->handle->is_double_selected)
	{
		curr_pose = points->handle->GetDoubleSelectionTransform();
		points->SetMasterAppendPose(curr_pose);
	}
	else if (points->handle->is_selected) 
	{
		curr_pose = points->handle->cache.controller_pose_updated * points->handle->cache.to_controller_space_initial;
		points->SetMasterAppendPose(curr_pose);
	}

	if (viewer->orthoslice->WasClicked())
	{
		glm::vec4 colp_to_model_space = glm::inverse(viewer->base_handle->GetModelMatrix()) * glm::vec4(viewer->orthoslice->cache.primary_collision_point_world_current, 1.0f);

		// should be 1by1 in model space
		colp_to_model_space.x = glm::clamp(colp_to_model_space.x, 0.0f, imaging_data.scale.x);
		colp_to_model_space.y = glm::clamp(colp_to_model_space.y, 0.0f, imaging_data.scale.y);

		int index_x = (float)imaging_data.data[imaging_data.current_index].width * colp_to_model_space.x;
		int index_y = (float)imaging_data.data[imaging_data.current_index].height * colp_to_model_space.y;
		imaging_data.isovalue = imaging_data.data[imaging_data.current_index].isovalues.at(imaging_data.data[imaging_data.current_index].width * index_y + index_x);

		AddIsovaluePointCloudSlider(imaging_data.isovalue);
		points->Generate(imaging_data, imaging_data.isovalue, MAX_ISOVALUE_TOLERANCE);
	}

	// drawing branches in VR
	static BranchPoint* prev = NULL;
	static const float dist_threshold_to_existing = 0.1f;

	// test if controller is close to old branch point
	if (_vr.controller1.alt_first_press)
	{
		// find  the closest branch point		
		
		int closest_index = -1;
		float curr_min = -1.0f;
		bool found = false;

		for (int i = 0; i < points->branch_points.size(); ++i)
		{
			float len = glm::length(points->branch_points[i]->position - _vr.controller1.position);

			if (len <= dist_threshold_to_existing && len <= (curr_min || !found))
			{
				curr_min = len;
				closest_index = i;
				found = true;
			}
		}

		// if so, start a new branch from close branch point
		if (found)
		{
			prev = points->branch_points[closest_index];
		}
		// else new disconnected branch
		else
		{
			prev = NULL;
		}
	}
	else if (_vr.controller1.alt_is_pressed)
	{
		if (prev != NULL)
		{
			static const float new_bp_dist_threshold = 0.1f;

			if (glm::length(prev->position - _vr.controller1.position) >= new_bp_dist_threshold)
			{
				glm::vec4 controller_pos_in_point_space = glm::inverse(points->GetModelMatrix()) * glm::vec4(_vr.controller1.position, 1.0f);
				glm::vec4 tmp = points->GetModelMatrix() * glm::inverse(points->GetModelMatrix()) * glm::vec4(_vr.controller1.position, 1.0f);
				BranchPoint* newBP = new BranchPoint(glm::vec3(controller_pos_in_point_space) - points->lower_bounds);
				points->branch_points.push_back(newBP);
				prev->neighbors.push_back(newBP->id);
				prev = newBP;
			}
		}
		// first point of disconnected branch
		else
		{
			glm::vec4 controller_pos_in_point_space = glm::inverse(points->GetModelMatrix()) * glm::vec4(_vr.controller1.position, 1.0f);
			std::cout << controller_pos_in_point_space.x << " " << controller_pos_in_point_space.y << " " << controller_pos_in_point_space.z << std::endl;
			BranchPoint* newBP = new BranchPoint(glm::vec3(controller_pos_in_point_space) - points->lower_bounds);
			points->branch_points.push_back(newBP);
			prev = newBP;
		}
	}
}

void DicomObjectsContainer::AddObjects(Render * _r) 
{
	_r->AddObjectToScene(points);
	viewer->AddObjects(_r);
}

void DicomObjectsContainer::AddIsovaluePointCloudSlider(const int _isovalue)
{
	DicomPointCloudObject::isovalue_point_cloud_sliders.push_back(new IsovaluePointCloudSlider(_isovalue));
}

void DicomObjectsContainer::Load(std::string _dicomDir)
{
	imaging_data = DicomReader::ReadSet(_dicomDir);
	viewer->Load(imaging_data);
	UpdateDicomPointCloud(DEFAULT_ISOVALUE);
}

void DicomObjectsContainer::UpdateDicomPointCloud(int _isovalue)
{
	imaging_data.isovalue = _isovalue;
	points->Generate(imaging_data, _isovalue, MAX_ISOVALUE_TOLERANCE);
}