#include "DicomReader.h"

#include <future>

struct less_than_key
{
	inline bool operator() (const DicomSingle& struct1, const DicomSingle& struct2)
	{
		return (struct1.layer < struct2.layer);
	}
};


DicomSet DicomReader::ReadSet(std::string _dirPath, bool * finished) {

	DicomSet _dSet;

	//_dirPath = Path_MakeAbsolute(_dirPath, Path_GetWorkingDirectory(), '/');
	//std::cout << _dirPath << std::endl;

	tinydir_dir dir;
	tinydir_open(&dir, _dirPath.c_str()); 

	// single threaded version
	//while (dir.has_next){
	//
	//	tinydir_file file;
	//	tinydir_readfile(&dir, &file);
	//
	//	if (std::string(file.extension) == "dcm") {
	//		_dSet.data.push_back(ReadSingle(std::string(file.path)));
	//	}
	//
	//	tinydir_next(&dir);
	//}

	// multiple threads
	std::vector<std::future<DicomSingle>> threads;
	
	while (dir.has_next){
	
		tinydir_file file;
		tinydir_readfile(&dir, &file);
	
		if (std::string(file.extension) == "dcm") {
			threads.emplace_back(std::async(ReadSingle, std::string(file.path)));
		}
	
		tinydir_next(&dir);
	}
	
	tinydir_close(&dir);
	
	for (std::future<DicomSingle> & d : threads) {
		_dSet.data.push_back(d.get());
	}
	
	
	std::sort(_dSet.data.begin(), _dSet.data.end(), less_than_key());

	if(finished != NULL)
	*finished = true;

	if (_dSet.data.size() > 1) {

	_dSet.scale = glm::vec3(1.0f, 1.0f, (float) _dSet.data.size() / (float)_dSet.data[0].width);
	_dSet.scale.z = (1.0f/(float)_dSet.data.size()) * _dSet.data[0].spacing;
	_dSet.scale.z = 1.0f;
	std::cout << _dSet.data[0].spacing << std::endl;
	}


	return _dSet;
}

DicomSingle DicomReader::ReadSingle(std::string _filePath) {

	// return value
	DicomSingle _dSingle;

	using namespace puntoexe;

	ptr<stream> readStream(new stream);
	readStream->openFile(_filePath, std::ios::in);
	ptr<streamReader> reader(new streamReader(readStream));

	ptr<imebra::dataSet> testDataSet = imebra::codecs::codecFactory::getCodecFactory()->load(reader);

	_dSingle.layer = std::atoi(testDataSet->getString(0x0020, 0, 0x0013, 0).c_str());
	_dSingle.series_id = testDataSet->getString(0x0020, 0, 0x000E, 0);


	//double[] d = Attribute.getDoubleValues(list, TagFromName.SpacingBetweenSlices);
	//double[] e = Attribute.getDoubleValues(list, TagFromName.PixelSpacing);
	//
	//if (d.length > 0 && e.length > 0)
	//	spacing_ratio = (float)(d[0] / e[0]);



	float spacingBetweenSlices = std::atof(testDataSet->getString(0x0018, 0, 0x0088, 0).c_str());
	float pixelSpacing = std::atof(testDataSet->getString(0x0028, 0, 0x0030, 0).c_str());

	_dSingle.spacing = spacingBetweenSlices / pixelSpacing;

	ptr<imebra::image> firstImage = testDataSet->getImage(0);
	std::uint32_t rowSize, channelPixelSize, channelsNumber;
	ptr<imebra::handlers::dataHandlerNumericBase> myHandler = firstImage->getDataHandler(false, &rowSize, &channelPixelSize, &channelsNumber);
	
	std::uint32_t sizeX, sizeY;
	firstImage->getSize(&sizeX, &sizeY);

	//static int skip = 2;
	//_dSingle.width = sizeX / skip;
	//_dSingle.height = sizeY /skip;
	//_dSingle.isovalues.clear();
	//static short iso_prev;
	//std::uint32_t index(0);
	//for (std::uint32_t scanY = 0; scanY < sizeY; ++scanY){
	//	for (std::uint32_t scanX = 0; scanX < sizeX; ++scanX){
	//		for (std::uint32_t scanChannel = 0; scanChannel < 1; ++scanChannel){ // TEMP: loop condition set to 1 (was channelsNumber)
	//			if (!(scanX % skip == 0 && scanY % skip ==0))
	//				iso_prev = myHandler->getUnsignedLong(index++);
	//			else
	//				_dSingle.isovalues.push_back((myHandler->getUnsignedLong(index++) + iso_prev) / 2);
	//		}
	//	}
	//}

	_dSingle.width = sizeX;
	_dSingle.height = sizeY;
	_dSingle.isovalues.clear();

	std::uint32_t index(0);
	for (std::uint32_t scanY = 0; scanY < sizeY; ++scanY){
		for (std::uint32_t scanX = 0; scanX < sizeX; ++scanX){
			for (std::uint32_t scanChannel = 0; scanChannel < 1; ++scanChannel){ // TEMP: loop condition set to 1 (was channelsNumber)
				_dSingle.isovalues.push_back(myHandler->getUnsignedLong(index++));
			}
		}
	}

	return _dSingle;
}
