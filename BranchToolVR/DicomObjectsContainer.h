#pragma once

#include <string>

#include "DicomReader.h"
#include "CoarseDicomViewer.h"
#include "DicomPointCloudObject.h"
#include "Render.h"
#include "UiPanel.h"

class DicomObjectsContainer{

	public:

		DicomObjectsContainer();
		~DicomObjectsContainer();
		void Load(std::string _dicomDir);
		void UpdateDicomPointCloud(int _isovalue);
		void Interact(glm::mat4 & _controllerPose, glm::vec3& _ray, glm::vec3& _pos, bool _pressed);
		void AddObjects(Render * _r);
		void Update(float h_asp, VrData & _vr, CursorData & _crsr);// glm::mat4 & _controllerPose, glm::vec3& _ray, glm::vec3& _pos, bool _pressed);

	//private:

		DicomSet imaging_data;
		DicomPointCloudObject * points;
		UiPanel * dicom_panel;
		CoarseDicomViewer * viewer;
		ColorObject * debug;

		// 2D ui elements
		TextureObject * orthoslice;
		ColorObject * selector2D;

		// ui panel trigger elements
		Slider* isovalue_slider;
		Slider* isovalue_tol_slider;
		Slider* scaleX_slider;
		Slider* scaleY_slider;
		Slider* scaleZ_slider;
		Slider* window_width_slider;
		Slider* window_center_slider;
		Slider* clear_branching_slider;
		Slider* ortho_level;

};
