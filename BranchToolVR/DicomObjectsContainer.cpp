#include "DicomObjectsContainer.h"

DicomObjectsContainer::DicomObjectsContainer()
{
	points = new DicomPointCloudObject;
	viewer = new CoarseDicomViewer;
	imgui_panel = new TextureObject;

	imgui_panel->GenerateXYPlane(1.0f, 1.0f, 0.0f, glm::vec3(0.0f));
	imgui_panel->is_selectable = true;
	imgui_panel->texture_id = 11;

	imgui_panel_handle = new TextureObject;
	imgui_panel_handle->readObjFromFile(DirectoryInfo::IMGUI_FRAME_MODEL,1.0f,glm::vec3(0.5,0.5f,0.0f));
	imgui_panel_handle->texture_id = IMGUI_HANDLE_TEXTURE;
	imgui_panel_handle->is_selectable = true;

	//// default values
	//imaging_data.isovalue = 1340;
	//imaging_data.isovalue_tolerance = DEFAULT_ISOVALUE_TOLERANCE;
	//imaging_data.window_center = 0;
	//imaging_data.window_width = 1000;



	glm::mat4 tmp_viewer_start = glm::translate(glm::mat4(1.0f), glm::vec3( 0.5f, 0.25f, 0.5f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	glm::mat4 tmp_points_start = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.25f, 0.5f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));

	viewer->SetAppendPose(tmp_viewer_start);

	points->Set_append_pose(tmp_points_start);
	points->handle->Set_append_pose(tmp_points_start);
	points->Set_append_pose(tmp_points_start);
	points->branch_point_marker->Set_append_pose(tmp_points_start);
	points->bounding_cube->Set_append_pose(tmp_points_start);


}

DicomObjectsContainer::~DicomObjectsContainer()
{
	delete points;
	delete viewer;
}


void DicomObjectsContainer::UpdateSliders() 
{
	// tab one
	if (isovalue_slider->has_changed)
	{
		UpdateDicomPointCloud(isovalue_slider->curr);
		isovalue_slider->has_changed = false;
	}
	if (isovalue_tol_slider->has_changed) 
	{
		points->curr_tolerance = isovalue_tol_slider->curr;
		isovalue_tol_slider->has_changed = false;
	}
	if (clear_branching_slider->has_changed)
	{
		points->branch_points.clear();
		clear_branching_slider->has_changed = false;
	}

	// tab two
	if (scaleX_slider->has_changed) 
	{
		viewer->point_cloud_selector->Set_scale(scaleX_slider->curr);
		viewer->point_cloud_selector_scale = scaleX_slider->curr;
		scaleX_slider->has_changed = false;
	}

	// tab three
	if (window_width_slider->has_changed)
	{
		imaging_data.window_width = window_width_slider->curr;
		viewer->orthoslice_texture->Load(imaging_data.data[imaging_data.current_index], window_width_slider->curr, window_center_slider->curr);
		window_width_slider->has_changed = false;
	}
	
	if (window_center_slider->has_changed) 
	{
		imaging_data.window_center = window_center_slider->curr;
		viewer->orthoslice_texture->Load(imaging_data.data[imaging_data.current_index], window_width_slider->curr, window_center_slider->curr);
		window_center_slider->has_changed = false;
	}

}

void DicomObjectsContainer::SetImguiPanelCoords()
{

}

void DicomObjectsContainer::Update(float h_asp, VrData & _vr, CursorData & _crsr)
{
	//UpdateSliders();

	glm::mat4 curr_pose;

	if (viewer->base_handle->is_double_selected)
	{
		viewer->SetAppendPose(viewer->base_handle->getDoubleSelectionTransform());
	}	
	else if(viewer->base_handle->is_selected)
	{
		curr_pose = viewer->base_handle->cache.controller_pose_updated * viewer->base_handle->cache.to_controller_space_initial;
		viewer->SetAppendPose(curr_pose);
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

			viewer->point_cloud_selector->Set_scale(intersection_ray_ratio *viewer->point_cloud_selector->cache.initial_scale);
			viewer->point_cloud_selector_scale = viewer->point_cloud_selector->scale;
			once = true;
		}
		else
		{
			glm::vec3 pointer_model_space = glm::vec3(glm::inverse(viewer->base_handle->GetModelMatrix()) 
			* glm::vec4(viewer->point_cloud_selector->cache.primary_collision_point_world_current, 1.0f));

			if (once)
			{
				offset = viewer->point_cloud_selector->model_position - pointer_model_space;
				once = false;
			}

			viewer->point_cloud_selector->Set_model_position(pointer_model_space + offset);
		}
	}
	else
	{
		if (!once)
		{
			// point cloud selector was moved and released
			viewer->selector_lower_bounds = viewer->point_cloud_selector->model_position - glm::vec3(0.5f*viewer->point_cloud_selector_scale);
			viewer->selector_upper_bounds = viewer->point_cloud_selector->model_position + glm::vec3(0.5f*viewer->point_cloud_selector_scale);
			points->lower_bounds = viewer->selector_lower_bounds;
			points->upper_bounds = viewer->selector_upper_bounds;
			points->curr_tolerance = 30;
			points->GenerateDicomPointCloud(imaging_data, imaging_data.isovalue, 30);
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

		viewer->orthoslice_handle->Set_model_positionZ(colp_to_model_space.z);
		viewer->orthoslice->Set_model_positionZ(colp_to_model_space.z);
	}


	if (points->handle->is_double_selected)
	{
		curr_pose = points->handle->getDoubleSelectionTransform();
		points->Set_append_pose(curr_pose);
		points->handle->Set_append_pose(curr_pose);
		points->Set_append_pose(curr_pose);
		points->branch_point_marker->Set_append_pose(curr_pose);
		points->bounding_cube->Set_append_pose(curr_pose);
	}
	else if (points->handle->is_selected) 
	{
		curr_pose = points->handle->cache.controller_pose_updated * points->handle->cache.to_controller_space_initial;
		points->handle->Set_append_pose(curr_pose);
		points->Set_append_pose(curr_pose);
		points->branch_point_marker->Set_append_pose(curr_pose);
		points->bounding_cube->Set_append_pose(curr_pose);
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

		points->GenerateDicomPointCloud(imaging_data, imaging_data.isovalue, 30);
	}

	if (imgui_panel_handle->is_selected)
	{
		curr_pose = imgui_panel_handle->cache.controller_pose_updated * imgui_panel_handle->cache.to_controller_space_initial;
		imgui_panel_handle->Set_append_pose(curr_pose);
		imgui_panel->Set_append_pose(curr_pose);

	}

	if (imgui_panel->is_selected)
	{
		glm::vec4 colp_to_model_space = glm::inverse(imgui_panel->GetModelMatrix()) * glm::vec4(imgui_panel->cache.primary_collision_point_world_current, 1.0f);
		imgui_panel_coords = glm::vec2(colp_to_model_space);

	}



}

void DicomObjectsContainer::AddObjects(Render * _r) 
{
	// dicom panel
	//vr_ui->GenerateDicomPanel(_r);
	//vr_ui->SetWorldPosition(glm::vec3(0.0f, 0.0f, -1.0f));
	//vr_ui->SetModelOrientation(glm::vec3(0.8f, 0.2f, 0.2f));

	//isovalue_slider = vr_ui->GetSliderByName("isovalue");
	//isovalue_tol_slider = vr_ui->GetSliderByName("isovalue tolerance");
	//scaleX_slider = vr_ui->GetSliderByName("selector scale");
	//window_width_slider = vr_ui->GetSliderByName("window width");
	//window_center_slider = vr_ui->GetSliderByName("window center");
	//clear_branching_slider = vr_ui->GetSliderByName("reset selection");
	//ortho_level = vr_ui->GetSliderByName("level");
	
	_r->AddObjectToScene(points);
	_r->AddObjectToScene(imgui_panel);
	_r->AddObjectToScene(imgui_panel_handle);
	//_r->AddObjectToScene(points->bounding_cube);

	viewer->AddObjects(_r);



}

void DicomObjectsContainer::Load(std::string _dicomDir)
{
	// need to reset positions
	imaging_data = DicomReader::ReadSet(_dicomDir);
	viewer->Load(imaging_data);
	UpdateDicomPointCloud(DEFAULT_ISOVALUE);
}

void DicomObjectsContainer::UpdateDicomPointCloud(int _isovalue)
{
	imaging_data.isovalue = _isovalue;
	points->GenerateDicomPointCloud(imaging_data, _isovalue, MAX_ISOVALUE_TOLERANCE);
}