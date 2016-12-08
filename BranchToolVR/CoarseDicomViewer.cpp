#include "CoarseDicomViewer.h"

CoarseDicomViewer::CoarseDicomViewer()
{
	static_mesh = new ColorObject;
	
	float base_handle_scale = 1.0f;
	base_handle = new TextureObject;
	base_handle->readObjFromFile(DirectoryInfo::POINT_CLOUD_HANDLE_MODEL, base_handle_scale, glm::vec3(base_handle_scale * 0.5f, 0.0f, base_handle_scale * 0.5f));
	base_handle->is_selectable = true;
	base_handle->is_double_selectable = true;

	orthoslice_handle = new TextureObject;
	orthoslice_handle->readObjFromFile(DirectoryInfo::COARSE_VIEWER_SLICE_HANDLE,1.0f, glm::vec3(0.5f, 0.5f, 0.0f));
	orthoslice_handle->is_selectable = true;
	orthoslice_handle->texture_id = COARSE_VIEWER_SLICE_HANDLE_TEXTURE;

	orthoslice = new TextureObject;
	orthoslice->is_clickable = true;
	orthoslice->GenerateXYPlane(1.0f, 1.0f, 0.0f, glm::vec3(0.0f));
	orthoslice->texture_id = CURR_ORTHOSLICE_TEXTURE;
	orthoslice_texture = new Texture;
	selector_changed = false;

	point_cloud_selector = new ColorObject;
	point_cloud_selector->is_selectable = true;
	point_cloud_selector->is_double_selectable = true;
	point_cloud_selector->SetDisplayColor(glm::vec4(0.0f, 0.5f, 0.5f, 0.5f));
	point_cloud_selector->GenerateXYPrism(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f), glm::vec3(-0.5f));
	point_cloud_selector->Set_scale(Constants::DEFAULT_SELECTOR_SCALE);
	point_cloud_selector->selection_modifier = -0.25f;
	point_cloud_selector_scale = Constants::DEFAULT_SELECTOR_SCALE;

	//SetAppendScale(0.5f);
	//Translate(glm::vec3(0.5f, 0.05f, 0.5f));
}

CoarseDicomViewer::~CoarseDicomViewer()
{
	delete static_mesh;
	delete base_handle;
	delete orthoslice_handle;
	delete orthoslice;
	delete point_cloud_selector;
}

void CoarseDicomViewer::AddObjects(Render * _r) 
{
	_r->AddObjectToScene(static_mesh);
	_r->AddObjectToScene(base_handle);
	_r->AddObjectToScene(orthoslice);
	_r->AddObjectToScene(orthoslice_handle);
	_r->AddObjectToScene(point_cloud_selector);

	// set texture reference
	_r->textures[CURR_ORTHOSLICE_TEXTURE] = orthoslice_texture;
}

glm::vec3 CoarseDicomViewer::GetCollisionPointWithOrthosliceHandlePlane(glm::vec3 _pos, glm::vec3 _ray, glm::vec3 & _cg) 
{
	glm::mat4 mm = base_handle->GetModelMatrix();
	glm::vec4 tri[3];
	tri[0] = mm * glm::vec4(glm::vec3(0.0f,0.0f,0.0f),1.0f);
	tri[1] = mm * glm::vec4(glm::vec3(1.0f,0.0f,0.0f),1.0f);
	tri[2] = mm * glm::vec4(glm::vec3(1.0f,0.0f,1.0f),1.0f);
	MiscFunctions::ray_triangle_intersection(glm::value_ptr(tri[0]), glm::value_ptr(tri[1]), glm::value_ptr(tri[2]), glm::value_ptr(_pos), glm::value_ptr(_ray), glm::value_ptr(_cg));
	_cg = glm::vec3(_cg.x * tri[0] + _cg.y * tri[1] + _cg.z * tri[2]);
	
	return glm::vec3(glm::inverse(mm) * glm::vec4(_cg, 1.0f));
}

void CoarseDicomViewer::Translate(glm::vec3 & _inWorldPos) 
{
	glm::mat4 curr_pose = glm::translate(glm::mat4(1.0f), _inWorldPos);// *base_handle->append_pose;
	base_handle->Set_append_pose(curr_pose);
	static_mesh->Set_append_pose(curr_pose);
	orthoslice->Set_append_pose(curr_pose);
	orthoslice_handle->Set_append_pose(curr_pose);
	point_cloud_selector->Set_append_pose(curr_pose);
	//debug_cube->Set_append_pose(curr_pose);
}

void CoarseDicomViewer::Update(DicomSet & _dSet) 
{

	//static glm::vec3 curr_selector_delta = glm::vec3(0.0f, 0.0f, 0.0f);
	//static glm::mat4 old_selector_pose = glm::mat4(1.0f);
	//
	//if (base_handle->is_selected) {
	//	//glm::mat4 curr_pose = base_handle->cache_pose[1] * base_handle->cache_pose[0];
	//	//orthoslice_handle->append_pose = curr_pose;
	//	//base_handle->append_pose = curr_pose;
	//	//static_mesh->append_pose = curr_pose;
	//	//orthoslice->append_pose = curr_pose;
	//	//orthoslice_handle->append_pose = curr_pose;
	//	//point_cloud_selector->append_pose = curr_pose;
	//	glm::mat4 curr_pose = base_handle->cache_pose[1] * base_handle->cache_pose[0];
	//	orthoslice_handle->Set_append_pose(curr_pose);
	//	base_handle->Set_append_pose(curr_pose);
	//	static_mesh->Set_append_pose(curr_pose);
	//	orthoslice->Set_append_pose(curr_pose);
	//	point_cloud_selector->Set_append_pose(curr_pose);
	//}
	//static bool b = true;
	//static glm::vec3 st(0.0f, 0.0f, 0.0f);
	//
	//if (point_cloud_selector->is_selected) {
	//
	//	if (b) {
	//		st = point_cloud_selector->model_position;
	//		b = false;
	//	}
	//
	//	glm::mat4 curr_pose = point_cloud_selector->cache_pose[1] * point_cloud_selector->cache_pose[0];
	//	glm::vec3 l1 = glm::vec3(curr_pose * glm::vec4(glm::vec3(st),1.0f)); // was 0s
	//	glm::vec3 l2 = glm::vec3(point_cloud_selector->cache_pose[1] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	//	glm::vec4 pos1 = point_cloud_selector->cache_pose[1] * glm::vec4(0.0f, 0.0f,glm::length(l1-l2)*-1.0f, 1.0f);
	//	glm::vec4 pos2 = point_cloud_selector->cache_pose[2] * glm::vec4(0.0f, 0.0f,glm::length(l1-l2)*-1.0f, 1.0f);
	//	glm::vec3 pos31 =  glm::vec3(point_cloud_selector->append_pose * glm::vec4(glm::vec3(pos1 - pos2),0.0f));
	//	glm::vec3 pos32 = glm::vec3(pos1 - pos2);
	//
	//	
	//	//debug[1].world_position = pos32;
	//	//point_cloud_selector->append_pose = curr_pose;
	//	curr_selector_delta = pos32;
	//	//debug[0].append_pose = curr_pose;
	//
	//	//point_cloud_selector->append_pose = glm::translate(glm::mat4(1.0f),pos32) * point_cloud_selector->cache_pose[3];
	//	glm::mat4 inv = glm::inverse(base_handle->append_pose) * glm::translate(glm::mat4(1.0f), pos32) * point_cloud_selector->cache_pose[3];
	//	point_cloud_selector->Set_model_position(glm::vec3(inv * glm::vec4(glm::vec3(st), 1.0f)));
	//	old_selector_pose = point_cloud_selector->append_pose;
	//	old_selector_pose = glm::inverse(point_cloud_selector->cache_pose[3]);
	//	//cObj->cache_pose[0] = glm::inverse(_controllerPose1) * cObj->append_pose;
	//	//glm::vec3 pos31 = glm::vec3(curr_pose * glm::vec4(0.0f,0.0f,0.0f,1.0f));
	//	//glm::vec3 pos32 = glm::vec3(base_handle->append_pose * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	//	//point_cloud_selector->model_position = pos31;
	//
	//}
	//else if(!b) {
	//	// selected and released
	//	selector_changed = true;
	//	b = true;
	//}
	//else {
	//	// not previously selected
	//	//std::cout << "not prev selected" << std::endl;
	//	b = true;
	//}
	//
	//if (orthoslice_handle->is_selected) {
	//	glm::vec3 _pos = glm::vec3(orthoslice_handle->cache_pose[1] * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	//	glm::vec3 _ray = glm::vec3(orthoslice_handle->cache_pose[1] * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));		
	//	glm::vec3 cg;
	//	glm::vec3 p = GetCollisionPointWithOrthosliceHandlePlane(_pos,_ray,cg);
	//
	//	if (glm::dot((cg - _pos), _ray) > 0) {
	//			float fac1 = glm::clamp(p.z, 0.0f, _dSet.scale.z);
	//			float fac2 = fac1 / _dSet.scale.z;
	//			int orthoslice_index = (float)(_dSet.data.size()-1) * fac2;
	//			_dSet.current_index = orthoslice_index;
	//			orthoslice_texture->Load(_dSet.data[orthoslice_index], _dSet.window_width, _dSet.window_center);
	//			orthoslice->Set_model_position(glm::vec3(0.0f,0.0f,fac1));
	//			orthoslice_handle->Set_model_position(glm::vec3(_dSet.scale.x / 2.0f, 0.0f, fac1));
	//	}
	//
	//}
}

void CoarseDicomViewer::Load(DicomSet & _dSet) 
{
	static_mesh->GenerateIsosurfaceFromDicomSet(_dSet, _dSet.isovalue);
	static_mesh->SetDisplayColor(Constants::STATIC_DICOM_MESH_COLOR);
	orthoslice_texture->Load(_dSet.data[0], _dSet.window_width, _dSet.window_center);
	orthoslice->texture_id = CURR_ORTHOSLICE_TEXTURE;
}

void CoarseDicomViewer::SetAppendPose(glm::mat4 _append)
{
	base_handle->Set_append_pose(_append);
	static_mesh->Set_append_pose(_append);
	orthoslice_handle->Set_append_pose(_append);
	orthoslice->Set_append_pose(_append);
	point_cloud_selector->Set_append_pose(_append);
}

void CoarseDicomViewer::SetAppendScale(float _scale)
{
	glm::mat4 new_append = glm::scale(glm::mat4(1.0f),glm::vec3(_scale))* base_handle->append_pose;
	SetAppendPose(new_append);
}
