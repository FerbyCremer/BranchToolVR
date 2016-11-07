#include "AbstractBaseObject.h"

int AbstractBaseObject::id_counter = 0;

AbstractBaseObject::AbstractBaseObject(){
	world_position = glm::vec3(0.0, 0.0, 0.0f);
	model_orientation = glm::vec3(0.0f, 0.0f, 0.0f);
	model_position = glm::vec3(0.0f, 0.0f, 0.0f);
	append_pose = glm::mat4(1.0f);
	ui_model_matrix = glm::mat4(1.0f);
	model_matrix = glm::mat4(1.0f);
	base_model_matrix = glm::mat4(1.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	is_loaded = false;
	is_hidden = false;
	is_selectable = false;
	is_selected = false;
	ui_quadrant = 0;
	ui_transform = glm::mat4(1.0f);
	id = id_counter++;
	controllerSelectorId = -1;
	level = 0;
}

AbstractBaseObject::~AbstractBaseObject(){

}

glm::mat4 AbstractBaseObject::GetModelMatrix() {
	return model_matrix;
}

void AbstractBaseObject::CalcModelMatrix() {
	base_model_matrix = glm::translate(glm::mat4(), model_position) * glm::scale(glm::mat4(1.0f),scale);
	model_matrix = append_pose
				 * glm::translate(glm::mat4(), world_position)
				 * glm::yawPitchRoll(model_orientation.x, model_orientation.y, model_orientation.z)
				 * base_model_matrix;
	ui_model_matrix = ui_transform * base_model_matrix;
}

void AbstractBaseObject::Set_world_position(const glm::vec3& v) {
	world_position = v;
	CalcModelMatrix();
}
void AbstractBaseObject::Set_model_orientation(const glm::vec3& v) {
	model_orientation = v;
	CalcModelMatrix();
}
void AbstractBaseObject::Set_model_position(const glm::vec3& v) {
	model_position = v;
	CalcModelMatrix();
}

void AbstractBaseObject::Set_model_positionX(const float _x) {
	model_position.x = _x;
	CalcModelMatrix();
}

void AbstractBaseObject::Set_model_positionY(const float _y) {
	model_position.y = _y;
	CalcModelMatrix();
}

void AbstractBaseObject::Set_model_positionZ(const float _z) {
	model_position.z = _z;
	CalcModelMatrix();
}

void AbstractBaseObject::Set_scale(const glm::vec3& v) {
	scale = v;
	CalcModelMatrix();
}
void AbstractBaseObject::Set_append_pose(const glm::mat4& m) {
	append_pose = m;
	CalcModelMatrix();
}
void AbstractBaseObject::Set_cache_pose(const glm::mat4& m, const int index) {
	cache_pose[index] = m;
	CalcModelMatrix();
}
