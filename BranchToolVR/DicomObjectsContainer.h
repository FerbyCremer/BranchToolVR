#pragma once

#include <string>

#include "DicomReader.h"
#include "CoarseDicomViewer.h"
#include "DicomPointCloudObject.h"
#include "Render.h"
#include "Ui.h"

class DicomObjectsContainer
{
	public:
		DicomObjectsContainer();
		~DicomObjectsContainer();
		void Update(const VrData& _vr, const CursorData& _crsr);
		void Load(std::string _dicomDir);		
		void AddObjects(Render * _r);
		void AddIsovaluePointCloudSlider(const int _isovalue);

	private:
		void UpdateDicomPointCloud(int _isovalue);
		void SetCoarseViewerAppendPose(const glm::mat4 _m);
		DicomSet imaging_data;
		DicomPointCloudObject * points;
		CoarseDicomViewer * viewer;
};
