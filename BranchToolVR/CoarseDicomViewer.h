#pragma once

#include "Render.h"
#include "ColorObject.h"
#include "TextureObject.h"
#include "DicomReader.h"

class CoarseDicomViewer{

	public:

		CoarseDicomViewer();
		~CoarseDicomViewer();
		ColorObject * static_mesh;
		ColorObject * base_handle;
		ColorObject * orthoslice_handle;
		ColorObject * point_cloud_selector;
		TextureObject * orthoslice;
		Texture * orthoslice_texture;
		glm::vec3 point_cloud_selector_scale;
		glm::vec3 GetCollisionPointWithOrthosliceHandlePlane(glm::vec3 _pos, glm::vec3 _ray, glm::vec3 & _cg);
		void Load(DicomSet & _dSet);
		void Interact(glm::mat4 & _controllerPose, glm::vec3 _ray, glm::vec3 _pos, bool _pressed);
		void AddObjects(Render * _r);
		void Translate(glm::vec3 & _inWorldPos);
		void Update(DicomSet & _dSet);
		bool selector_changed;

};

