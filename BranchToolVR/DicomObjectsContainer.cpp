#include "DicomObjectsContainer.h"

DicomObjectsContainer::DicomObjectsContainer()
{
	points = new DicomPointCloudObject;
	viewer = new CoarseDicomViewer;
	dicom_panel = new UiPanel;

	// default values
	imaging_data.isovalue = 1340;
	imaging_data.isovalue_tolerance = DEFAULT_ISOVALUE_TOLERANCE;
	imaging_data.window_center = 0;
	imaging_data.window_width = 1000;

	// default positions
	points->SetWorldPosition(glm::vec3(1.0f, 0.1f, 0.1f));
	viewer->Translate(glm::vec3(0.5f, 0.5f, 0.5f));

	orthoslice = new TextureObject;
	orthoslice->GenerateXYPlane(1.0f, 1.0f, 0.0f, glm::vec3(-0.5f, -0.5f, 0.0f));
	orthoslice->texture_id = CURR_ORTHOSLICE_TEXTURE;
	orthoslice->ui_quadrant = 1;

	selector2D = new ColorObject;
	selector2D->Set_scale(Constants::DEFAULT_SELECTOR_SCALE);
	selector2D->GenerateXYPlane(1.0f, 1.0f, 0.0f, glm::vec3(-0.5f, -0.5f, 0.1f));
	selector2D->ui_quadrant = 1;
	selector2D->SetDisplayColor(Constants::UI_SELECTOR_COLOR);
	selector2D->Set_model_position(glm::vec3(viewer->point_cloud_selector_scale.x*0.5f - 0.5f, viewer->point_cloud_selector_scale.x*0.5f - 0.5f,0.0f));
	selector2D->level = 1;
}

DicomObjectsContainer::~DicomObjectsContainer()
{

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
		viewer->point_cloud_selector->Set_scale(glm::vec3(scaleX_slider->curr));
		viewer->point_cloud_selector_scale = glm::vec3(scaleX_slider->curr);
		selector2D->Set_scale(glm::vec3(scaleX_slider->curr));
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


void DicomObjectsContainer::Update(float h_asp, VrData & _vr, CursorData & _crsr){

	UpdateSliders();

	viewer->Update(imaging_data);

	if (viewer->selector_changed) 
	{	
		points->lower_bounds = viewer->point_cloud_selector->model_position;
		points->upper_bounds = viewer->point_cloud_selector->model_position + viewer->point_cloud_selector_scale;
		points->box_scale = viewer->point_cloud_selector_scale;
		points->GenerateDicomPointCloud(imaging_data, imaging_data.isovalue, MAX_ISOVALUE_TOLERANCE);
		viewer->selector_changed = false;
		selector2D->Set_model_positionX(viewer->point_cloud_selector->model_position.x + viewer->point_cloud_selector_scale.x*0.5f-0.5f);
		selector2D->Set_model_positionY(viewer->point_cloud_selector->model_position.y + viewer->point_cloud_selector_scale.x*0.5f-0.5f);
	}

	if (points->handle->is_selected) 
	{
		glm::mat4 curr_pose = points->handle->cache_pose[1] * points->handle->cache_pose[0];
		points->handle->Set_append_pose(curr_pose);
		points->Set_append_pose(curr_pose);
		points->branch_point_marker->Set_append_pose(curr_pose);
	}		

	if (viewer->orthoslice->model_position.z >= points->lower_bounds.z && viewer->orthoslice->model_position.z <= points->upper_bounds.z) 
	{
		selector2D->SetDisplayColor(glm::vec4(0.2f, 0.2f, 1.0f, 0.5f));
	}
	else 
	{
		selector2D->SetDisplayColor(glm::vec4(0.8f,0.8f,0.8f,1.0f));
	}

	static bool selector2D_ui_selection = false;

	// ui interactions
	if (_crsr.is_pressed) 
	{
		if (_crsr.normalized_cursor_position.x > 0.0f)
		{
			if (_crsr.normalized_cursor_position.y > 0.0f) 
			{
				
				// quadrant 1
				glm::vec2 quad1coords = 1.0f*(_crsr.normalized_cursor_position) - 0.5f;

				if (h_asp > 1.0f) 
				{
					quad1coords.x *= h_asp;
				}
				else 
				{
					quad1coords.y /= h_asp;
				}
				
				selector2D->Set_model_positionX(quad1coords.x);
				selector2D->Set_model_positionY(quad1coords.y);

				selector2D_ui_selection = true;

			}
			else if (_crsr.normalized_cursor_position.y < 0.0f) 
			{
				// quadrant 0
				glm::vec2 quad0coords = _crsr.normalized_cursor_position;
				quad0coords.y += 1.0f;
				quad0coords *= 2.0f;
				quad0coords -= 1.0f;
				quad0coords *= dicom_panel->base_panel.half_size.x;
	
				if (h_asp > 1.0f) 
				{
					quad0coords.y /= h_asp;
				}
				else 
				{
					quad0coords.x *= h_asp;
				}

				glm::mat4 pose = glm::translate(glm::mat4(1.0f), glm::vec3(quad0coords.x, quad0coords.y,1.0f));
				glm::vec3 ray = glm::vec3(0.0f,0.0f,-1.0f);
				glm::vec3 pos = glm::vec3(quad0coords.x, quad0coords.y, 1.0f);

				dicom_panel->Interact(pose,ray,pos,_crsr.is_pressed,false);
			}
		}
	}
	else 
	{
		// update selection box if 2d selector was moved

		if (selector2D_ui_selection)
		{
			
			glm::vec2 selectorTo3dlower = glm::vec2(selector2D->model_position) + glm::vec2(0.5f,0.5f) - viewer->point_cloud_selector_scale.x*0.5f;
			glm::vec2 selectorTo3dupper = glm::vec2(selector2D->model_position) + glm::vec2(0.5f, 0.5f) + viewer->point_cloud_selector_scale.x*0.5f;


			points->lower_bounds = glm::vec3(selectorTo3dlower, viewer->orthoslice->model_position.z - viewer->point_cloud_selector_scale.x*0.5f);
			points->upper_bounds = glm::vec3(selectorTo3dupper, viewer->orthoslice->model_position.z + viewer->point_cloud_selector_scale.x*0.5f);

			points->box_scale = viewer->point_cloud_selector_scale;
			viewer->point_cloud_selector->Set_model_positionX(selectorTo3dlower.x);
			viewer->point_cloud_selector->Set_model_positionY(selectorTo3dlower.y);
			viewer->point_cloud_selector->Set_model_positionZ(viewer->orthoslice->model_position.z - viewer->point_cloud_selector_scale.x*0.5f);
			points->GenerateDicomPointCloud(imaging_data, imaging_data.isovalue, MAX_ISOVALUE_TOLERANCE);
			selector2D_ui_selection = false;
			selector2D->SetDisplayColor(glm::vec4(0.2f, 0.2f, 1.0f, 0.5f));
		}


		// release any previously held selections
		//dicom_panel->Interact(glm::mat4(), glm::vec3(), glm::vec3(), false, false);
	}

	// controller interactions
	//dicom_panel->Interact(_vr.controller_pose1, glm::vec3(_vr.controller_pose1* glm::vec4(0.0f, 0.0f, -1.0f,0.0f)), glm::vec3(_vr.controller_pose1* glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), _vr.controller_press1, true);
	//glm::vec3 ray = glm::vec3(p2 - p1);
}

void DicomObjectsContainer::AddObjects(Render * _r) {

	// dicom panel
	dicom_panel->GenerateDicomPanel(_r);
	dicom_panel->SetWorldPosition(glm::vec3(0.0f, 0.0f, -1.0f));
	dicom_panel->SetModelOrientation(glm::vec3(0.8f, 0.2f, 0.2f));
	isovalue_slider = dicom_panel->GetSliderByName("isovalue");
	isovalue_tol_slider = dicom_panel->GetSliderByName("isovalue tolerance");
	scaleX_slider = dicom_panel->GetSliderByName("selector scale");
	window_width_slider = dicom_panel->GetSliderByName("window width");
	window_center_slider = dicom_panel->GetSliderByName("window center");
	clear_branching_slider = dicom_panel->GetSliderByName("reset selection");
	ortho_level = dicom_panel->GetSliderByName("level");
	
	
	viewer->AddObjects(_r);
	_r->AddObjectToScene(points);
	_r->AddObjectToUi(orthoslice);
	_r->AddObjectToUi(selector2D);
	//_r->AddObjectToUi(debug1);
}

void DicomObjectsContainer::Load(std::string _dicomDir) {
	// need to reset positions
	imaging_data = DicomReader::ReadSet(_dicomDir, NULL);

	viewer->Load(imaging_data);
	UpdateDicomPointCloud(DEFAULT_ISOVALUE);
}

void DicomObjectsContainer::UpdateDicomPointCloud(int _isovalue) {
	imaging_data.isovalue = _isovalue;
	points->GenerateDicomPointCloud(imaging_data, _isovalue, MAX_ISOVALUE_TOLERANCE);
}

void DicomObjectsContainer::Interact(glm::mat4 & _controllerPose, glm::vec3& _ray, glm::vec3& _pos, bool _pressed) {
	//viewer->Interact(_controllerPose, _ray, _pos, _pressed);
	points->Interact( _controllerPose, _ray, _pos, _pressed);
}