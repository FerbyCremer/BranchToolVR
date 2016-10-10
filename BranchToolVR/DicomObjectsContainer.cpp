#include "DicomObjectsContainer.h"

ColorObject * debug1 = new ColorObject;

DicomObjectsContainer::DicomObjectsContainer(){
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
	float half_scale = viewer->point_cloud_selector_scale.x * 0.5f;
	selector2D->Set_scale(glm::vec3(viewer->point_cloud_selector_scale.x, viewer->point_cloud_selector_scale.x, 1.0f));
	selector2D->GenerateXYPlane(1.0f, 1.0f, 0.0f, glm::vec3(-0.5f, -0.5f, 0.1f));
	selector2D->ui_quadrant = 1;
	selector2D->SetDisplayColor(Constants::UI_SELECTOR_COLOR);
	selector2D->level = 1;

	debug1->GenerateXYPlane(1.0f, 1.0f, 0.0f, glm::vec3(-0.5f, -0.5f, 0.1f));
	debug1->Set_scale(glm::vec3(0.2f, 0.2f, 1.0f));
	debug1->ui_quadrant = 0;
}

DicomObjectsContainer::~DicomObjectsContainer(){

}

void DicomObjectsContainer::Update(float h_asp, VrData & _vr, CursorData & _crsr){//(glm::mat4 & _controllerPose, glm::vec3& _ray, glm::vec3& _pos, bool _pressed) {

	viewer->Update(imaging_data);

	if (viewer->selector_changed) {	
		points->lower_bounds = viewer->point_cloud_selector->model_position;
		points->upper_bounds = viewer->point_cloud_selector->model_position + viewer->point_cloud_selector_scale;
		points->box_scale = viewer->point_cloud_selector_scale;
		points->GenerateDicomPointCloud(imaging_data, imaging_data.isovalue, MAX_ISOVALUE_TOLERANCE);
		viewer->selector_changed = false;
		std::cout << "cube " << viewer->point_cloud_selector->model_position.x << " " << viewer->point_cloud_selector->model_position.y << std::endl;
	}

	if (points->handle->is_selected) {
		glm::mat4 curr_pose = points->handle->cache_pose[1] * points->handle->cache_pose[0];
		points->handle->Set_append_pose(curr_pose);
		points->Set_append_pose(curr_pose);
		points->branch_point_marker->Set_append_pose(curr_pose);
	}

	static bool selector2D_ui_selection = false;

	// ui interactions
	if (_crsr.is_pressed) {
		if (_crsr.normalized_cursor_position.x > 0.0f){
			if (_crsr.normalized_cursor_position.y > 0.0f) {
				
				// quadrant 1
				glm::vec2 quad1coords = 1.0f*(_crsr.normalized_cursor_position) - 0.5f;

				if (h_asp > 1.0f) {
					quad1coords.x *= h_asp;
				}
				else {
					quad1coords.y /= h_asp;
				}
				
				selector2D->Set_model_positionX(quad1coords.x);
				selector2D->Set_model_positionY(quad1coords.y);

				selector2D_ui_selection = true;

				std::cout << "quad " << quad1coords.x << " " << quad1coords.y << std::endl;

			}
			else if (_crsr.normalized_cursor_position.y < 0.0f) {
				// quadrant 0
				glm::vec2 quad0coords = _crsr.normalized_cursor_position;
				quad0coords.y += 1.0f;
				quad0coords *= 2.0f;
				quad0coords -= 1.0f;
				quad0coords *= dicom_panel->base_panel.half_size.x;
	
				//std::cout << quad0coords.x << " " << quad0coords.y<< std::endl;

				if (h_asp > 1.0f) {
					quad0coords.y /= h_asp;
				}
				else {
					quad0coords.x *= h_asp;
				}

				glm::mat4 pose = glm::translate(glm::mat4(1.0f), glm::vec3(quad0coords.x, quad0coords.y,1.0f));
				glm::vec3 ray = glm::vec3(0.0f,0.0f,-1.0f);
				glm::vec3 pos = glm::vec3(quad0coords.x, quad0coords.y, 1.0f);

				debug1->Set_model_position(glm::vec3(quad0coords.x, quad0coords.y, 0.5f));

				dicom_panel->Interact(pose,ray,pos,_crsr.is_pressed,false);
			}
		}
	}
	else {
		// update selection box if 2d selector was moved

		if (selector2D_ui_selection) {
			
			glm::vec2 selectorTo3d = glm::vec2(selector2D->model_position) + glm::vec2(0.5f,0.5f) - viewer->point_cloud_selector_scale.x*0.5f;
			
			//points->lower_bounds = selector2D->model_position + -f;
			points->upper_bounds = points->lower_bounds + viewer->point_cloud_selector_scale;
			points->box_scale = viewer->point_cloud_selector_scale;
			viewer->point_cloud_selector->Set_model_positionX(selectorTo3d.x);
			viewer->point_cloud_selector->Set_model_positionY(selectorTo3d.y);
			points->GenerateDicomPointCloud(imaging_data, imaging_data.isovalue, MAX_ISOVALUE_TOLERANCE);
			selector2D_ui_selection = false;
		}

		// release any previously held selections
		dicom_panel->Interact(glm::mat4(), glm::vec3(), glm::vec3(), false, false);
	}

	// controller interactions
	dicom_panel->Interact(_vr.controller_pose, glm::vec3(_vr.controller_pose* glm::vec4(0.0f, 0.0f, -1.0f,0.0f)), glm::vec3(_vr.controller_pose* glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), _vr.controller_press, true);
	//glm::vec3 ray = glm::vec3(p2 - p1);
}

void DicomObjectsContainer::AddObjects(Render * _r) {

	// dicom panel
	dicom_panel->GenerateDicomPanel(_r);
	dicom_panel->SetWorldPosition(glm::vec3(0.0f, 0.0f, -1.0f));
	dicom_panel->SetModelOrientation(glm::vec3(0.8f, 0.2f, 0.2f));
	isovalue_slider = dicom_panel->GetSliderByName("isovalue");
	isovalue_tol_slider = dicom_panel->GetSliderByName("isovalue tolerance");
	scaleX_slider = dicom_panel->GetSliderByName("X");
	scaleY_slider = dicom_panel->GetSliderByName("Y");
	scaleZ_slider = dicom_panel->GetSliderByName("Z");
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