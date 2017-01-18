#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

struct VrMotionController 
{
	glm::mat4 pose;
	glm::vec3 position;
	glm::vec3 orientation;
	glm::vec3 ray;
	bool trigger_is_pressed;
	bool trigger_first_press;
	bool alt_is_pressed;
	bool alt_first_press;
	int id;

	VrMotionController() 
	{
		trigger_is_pressed = false;
		trigger_first_press = false;
		alt_is_pressed = false;
		alt_first_press = false;
		pose = glm::mat4(1.0f);
		position = glm::vec3(0.0f);
		orientation = glm::vec3(0.0f);
		ray = glm::vec3(0.0f, 0.0f, -1.0f);
		id = 0;
	}

	void Press(bool _is_pressed)
	{
		trigger_first_press = _is_pressed && !trigger_is_pressed;
		trigger_is_pressed = _is_pressed;
	}

	void SetPose(glm::mat4 & _inPose)
	{
		pose = _inPose;
		ray = glm::vec3(pose * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
		//position = glm::vec3(pose[0][3], pose[1][3], pose[2][3]);
		position = glm::vec3(pose * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	//  spoof functions used to fake controller input for testing
	void SetPosSpoof(glm::vec3 _pos) 
	{
		position = _pos;
		CalcSpoofPose();
	}

	void SetOrientationSpoof(glm::vec3 _orientation) 
	{
		orientation = _orientation;
		CalcSpoofPose();
	}

	void CalcSpoofPose() 
	{
		pose = glm::translate(glm::mat4(), position) * glm::yawPitchRoll(orientation.x, orientation.y, orientation.z);
		ray = glm::vec3(pose * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	}
};

struct VrData 
{
	// controllers
	VrMotionController controller1;
	VrMotionController controller2;

	// HMD
	glm::mat4 head_pose_inv;
	glm::mat4 left_eye_proj;
	glm::mat4 right_eye_proj;
	glm::mat4 left_eye_transform_inv;
	glm::mat4 right_eye_transform_inv;
	glm::vec3 head_position;
	bool hmd_connected;

	VrData() 
	{
		head_pose_inv = glm::mat4(1.0f);
		left_eye_proj = glm::mat4(1.0f);
		right_eye_proj = glm::mat4(1.0f);
		left_eye_transform_inv = glm::mat4(1.0f);
		right_eye_transform_inv = glm::mat4(1.0f);
		head_position = glm::vec3(0.0f);
		hmd_connected = false;
	}
};

struct CursorData 
{
	glm::vec2 normalized_cursor_position;
	bool is_pressed;
	bool first_press;

	CursorData() 
	{
		normalized_cursor_position = glm::vec2(0.0f, 0.0f);
		is_pressed = false;
		first_press = true;
	}
};
