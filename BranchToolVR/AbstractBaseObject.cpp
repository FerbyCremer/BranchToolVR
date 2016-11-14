#include "AbstractBaseObject.h"

int AbstractBaseObject::id_counter = 0;

AbstractBaseObject::AbstractBaseObject()
{
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

AbstractBaseObject::~AbstractBaseObject()
{

}

glm::mat4 AbstractBaseObject::GetModelMatrix() 
{
	return model_matrix;
}

void AbstractBaseObject::CalcModelMatrix()
{
	base_model_matrix = glm::translate(glm::mat4(), model_position) * glm::scale(glm::mat4(1.0f),scale);
	model_matrix = append_pose
				 * glm::translate(glm::mat4(), world_position)
				 * glm::yawPitchRoll(model_orientation.x, model_orientation.y, model_orientation.z)
				 * base_model_matrix;
	ui_model_matrix = ui_transform * base_model_matrix;

	center = glm::vec3(model_matrix[0][3], model_matrix[1][3], model_matrix[2][3]);
}

void AbstractBaseObject::Set_world_position(const glm::vec3& v) 
{
	world_position = v;
	CalcModelMatrix();
}

void AbstractBaseObject::Set_model_orientation(const glm::vec3& v) 
{
	model_orientation = v;
	CalcModelMatrix();
}

void AbstractBaseObject::Set_model_position(const glm::vec3& v) 
{
	model_position = v;
	CalcModelMatrix();
}

void AbstractBaseObject::Set_model_positionX(const float _x) 
{
	model_position.x = _x;
	CalcModelMatrix();
}

void AbstractBaseObject::Set_model_positionY(const float _y)
{
	model_position.y = _y;
	CalcModelMatrix();
}

void AbstractBaseObject::Set_model_positionZ(const float _z) 
{
	model_position.z = _z;
	CalcModelMatrix();
}

void AbstractBaseObject::Set_scale(const glm::vec3& v)
{
	scale = v;
	
	CalcModelMatrix();
}

void AbstractBaseObject::Set_append_pose(const glm::mat4& m)
{
	append_pose = m;
	CalcModelMatrix();
}

void AbstractBaseObject::Set_cache_pose(const glm::mat4& m, const int index) 
{
	cache_pose[index] = m;
	CalcModelMatrix();
}

bool AbstractBaseObject::TestCollision(glm::vec3 _ray, glm::vec3 _pos, glm::vec3 & _collisionPoint)
{
	if (TestBoundingSphere(_ray, _pos) || true) // TODO FIX BOUNDING SPHERE CHECK
	{
		glm::mat4 mm = GetModelMatrix();
		glm::vec3 cg;
		
		// loop over mesh triangles, does not account for multiple collisions
		for (int i = 0; i < positions.size(); i += 3) 
		{
			glm::vec4 tri[3];
			tri[0] = mm * glm::vec4(positions[i + 0], 1.0f);
			tri[1] = mm * glm::vec4(positions[i + 1], 1.0f);
			tri[2] = mm * glm::vec4(positions[i + 2], 1.0f);

			MiscFunctions::ray_triangle_intersection(glm::value_ptr(tri[0]), glm::value_ptr(tri[1]), glm::value_ptr(tri[2]), glm::value_ptr(_pos), glm::value_ptr(_ray), glm::value_ptr(cg));
			_collisionPoint = glm::vec3(cg.x * tri[0] + cg.y * tri[1] + cg.z * tri[2]);
			
			if (cg.x >= 0 && cg.y >= 0 && cg.z > 0 && glm::dot(_ray, _collisionPoint - _pos) > 0)
			{
				return true;
			}
		}
	}

	_collisionPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	return false;
}

bool AbstractBaseObject::TestBoundingSphere(glm::vec3 _ray, glm::vec3 _pos) 
{
	if (_ray == Constants::ZERO_VEC3)
		return false;

	_ray = glm::normalize(_ray);
	glm::vec3 to_center = center - _pos;
	float factor1 = glm::dot(_ray, to_center);

	glm::vec3 perpendicular_vec = factor1*_ray + _pos - center;

	return glm::length(perpendicular_vec) < glm::length(model_max*scale.x);

}

void AbstractBaseObject::SetBoundingSphere() 
{
	float curr_max = 0.0f;
	for (int i = 0; i < positions.size(); ++i) 
	{
		// find length
		float curr_length;
		if (positions[i] != Constants::ZERO_VEC3) 
		{
			curr_length = glm::length(positions[i]);
		}
		else 
		{
			curr_length = 0.0f;
		}

		// test length
		if (curr_length > curr_max || i == 0)
		{
			curr_max = curr_length;
			model_max = positions[i];
		}
	}
}