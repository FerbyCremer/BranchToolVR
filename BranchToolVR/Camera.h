#pragma once

#include <glm/glm.hpp>

class Camera{

	public:
		Camera();
		Camera(float _near, float _far, float _aspect, float _fov);
		~Camera();
		glm::mat4 GetViewMatrix();

	private:
		//float near;
		//float far;
		//float aspect;
		//float fov;
		glm::vec3 position;
		glm::vec3 orientation;
};
