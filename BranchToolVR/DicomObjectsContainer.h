#pragma once

#include <string>

#include "DicomReader.h"
#include "CoarseDicomViewer.h"
#include "DicomPointCloudObject.h"
#include "Render.h"

class DicomObjectsContainer{

	public:

		DicomObjectsContainer();
		~DicomObjectsContainer();
		void Load(std::string _dicomDir);
		void UpdateDicomPointCloud(int _isovalue);
		void Interact(glm::mat4 & _controllerPose, glm::vec3& _ray, glm::vec3& _pos, bool _pressed);
		void AddObjects(Render * _r);
		void Update(glm::mat4 & _controllerPose, glm::vec3& _ray, glm::vec3& _pos, bool _pressed);

	//private:

		DicomSet imaging_data;
		DicomPointCloudObject * points;
		CoarseDicomViewer * viewer;
		ColorObject * debug;

};
