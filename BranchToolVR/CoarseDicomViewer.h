#pragma once

#include "Render.h"
#include "ColorObject.h"
#include "TextureObject.h"
#include "DicomReader.h"

class CoarseDicomViewer
{
	public:
		CoarseDicomViewer();
		~CoarseDicomViewer();
		ColorObject * static_mesh;
		TextureObject * base_handle;
		TextureObject * orthoslice_handle;
		ColorObject * point_cloud_selector;
		TextureObject * orthoslice;
		Texture * orthoslice_texture;
		float point_cloud_selector_scale;
		glm::vec3 GetCollisionPointWithOrthosliceHandlePlane(glm::vec3 _pos, glm::vec3 _ray, glm::vec3 & _cg);
		void Load(DicomSet & _dSet);
		void SetAppendPose(glm::mat4 _append);
		void SetAppendScale(float _scale);
		void AddObjects(Render * _r);
		void Translate(glm::vec3 & _inWorldPos);
		void Update(DicomSet & _dSet);
		bool selector_changed;
		glm::vec3 selector_lower_bounds;
		glm::vec3 selector_upper_bounds;


		ColorObject * debug_cube;

};
