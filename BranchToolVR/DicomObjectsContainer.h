#pragma once

#include <string>

#include "DicomReader.h"
#include "CoarseDicomViewer.h"
#include "DicomPointCloudObject.h"
#include "Render.h"

class DicomObjectsContainer
{
	public:
		DicomObjectsContainer();
		~DicomObjectsContainer();
		void Update(const VrData& _vr, const CursorData& _crsr);
		void Load(std::string _dicomDir);		
		void AddObjects(Render * _r);

	private:
		void UpdateDicomPointCloud(int _isovalue);
		DicomSet imaging_data;
		DicomPointCloudObject * points;
		CoarseDicomViewer * viewer;
};
