#include "DicomObjectsContainer.h"

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
	selector2D->GenerateXYPlane(0.5f, 0.5f, 0.1f, glm::vec3(-0.5f, -0.5f, 0.1f));
	selector2D->ui_quadrant = 1;

}

DicomObjectsContainer::~DicomObjectsContainer(){

}

void DicomObjectsContainer::Update(VrData & _vr){//(glm::mat4 & _controllerPose, glm::vec3& _ray, glm::vec3& _pos, bool _pressed) {

	viewer->Update(imaging_data);

	if (viewer->selector_changed) {	
		points->lower_bounds = viewer->point_cloud_selector->model_position;
		points->upper_bounds = viewer->point_cloud_selector->model_position + viewer->point_cloud_selector_scale;
		points->box_scale = viewer->point_cloud_selector_scale;
		points->GenerateDicomPointCloud(imaging_data, imaging_data.isovalue, MAX_ISOVALUE_TOLERANCE);
		viewer->selector_changed = false;
	}

	if (points->handle->is_selected) {
		glm::mat4 curr_pose = points->handle->cache_pose[1] * points->handle->cache_pose[0];
		points->handle->Set_append_pose(curr_pose);
		points->Set_append_pose(curr_pose);
		points->branch_point_marker->Set_append_pose(curr_pose);
	}

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
	viewer->AddObjects(_r);
	_r->AddObjectToScene(points);
	_r->AddObjectToUi(orthoslice);
	_r->AddObjectToUi(selector2D);
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