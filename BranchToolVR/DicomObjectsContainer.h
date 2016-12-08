#pragma once

#include <string>

#include "DicomReader.h"
#include "CoarseDicomViewer.h"
#include "DicomPointCloudObject.h"
#include "Render.h"
#include "UiPanel.h"

class DicomObjectsContainer
{
	public:
		DicomObjectsContainer();
		~DicomObjectsContainer();
		void Load(std::string _dicomDir);
		void UpdateDicomPointCloud(int _isovalue);
		void AddObjects(Render * _r);
		void Update(float h_asp, VrData & _vr, CursorData & _crsr);
		void UpdateSliders();

	//private:

		DicomSet imaging_data;
		DicomPointCloudObject * points;
		UiPanel * vr_ui;
		CoarseDicomViewer * viewer;
		ColorObject * debug;

		// 2D ui elements
		TextureObject * orthoslice;
		ColorObject * selector2D;

		// ui panel trigger elements
		Slider* isovalue_slider;
		Slider* isovalue_tol_slider;
		Slider* scaleX_slider;
		Slider* window_width_slider;
		Slider* window_center_slider;
		Slider* clear_branching_slider;
		Slider* ortho_level;
};
