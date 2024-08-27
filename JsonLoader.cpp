#include "JsonLoader.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>

JsonLoader::JsonLoader() { folderPath_ = "Data"; }

JsonLoader::~JsonLoader() { SaveJson(); }

void JsonLoader::LoadJson(const std::string& _filepath, bool _isMakeFile) {

	filePath_ = _filepath;
	std::ifstream inputFile(filePath_);
	if (!inputFile.is_open()) {
		if (_isMakeFile)
			MakeJsonFile();
		else
			assert(inputFile.is_open() && "Cant Open inputFile");
	} else {
		if (inputFile.peek() != std::ifstream::traits_type::eof())
			inputFile >> data_;
		inputFile.close();
	}
}

void JsonLoader::SaveJson() {
	std::ofstream outputFile(filePath_);
	assert(outputFile.is_open() && "Cant Open OutputFile");

	outputFile << data_.dump(4);
	outputFile.close();
}

void JsonLoader::MakeJsonFile() {

	std::filesystem::create_directories(folderPath_);
	std::ofstream outputFile(filePath_);
	outputFile.close();
}

void JsonLoader::RegisterData(const std::string& _key, const std::vector<Vector3>& _data) {

	for (const Vector3& vec : _data) {
		Vec3Json v(vec.x, vec.y, vec.z);
		data_[_key].push_back(v);
	}
}
void JsonLoader::RegisterData(const std::string& _key, const std::vector<RotatePoint>& _data) {

	for (const RotatePoint& vec : _data) {
		rotatePJson v;
		v.rotate = vec.rotate;
		v.posLength = vec.posLength;
		data_[_key].push_back(v);
	}
}
void JsonLoader::GetPointPositoin(const std::string& _key, std::vector<Vector3>& _array, size_t _arrayMaxSize) {
	if (data_.is_null() || data_[_key].is_null())
		return;

	std::vector<Vec3Json> vec = data_[_key];
	_array.clear();
	_array.reserve(_arrayMaxSize);

	size_t index = 0;
	for (const Vec3Json& v : vec) {
		_array.emplace_back();
		_array[index].x = v.x;
		_array[index].y = v.y;
		_array[index++].z = v.z;
	}
}

void JsonLoader::GetData(const std::string& _key, std::vector<RotatePoint>& _array, size_t _arrayMaxSize) {
	if (data_.is_null() || data_[_key].is_null())
		return;

	std::vector<rotatePJson> vec = data_[_key];
	_array.clear();
	_array.reserve(_arrayMaxSize);

	size_t index = 0;
	for (const rotatePJson& v : vec) {
		_array.emplace_back();
		_array[index].rotate.x = v.rotate.x;
		_array[index].rotate.y = v.rotate.y;
		_array[index].rotate.z = v.rotate.z;
		_array[index++].posLength = v.posLength;
	}
}

void JsonLoader::PrepareForSave() { data_.clear(); }
