#pragma once

#include "Structures.h"
#include "Vector3.h"
#include "json.hpp"

#include <string>
#include <vector>

using json = nlohmann::json;

struct Vec3Json {
	float x, y, z;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Vec3Json, x, y, z)
};

struct rotatePJson : RotatePoint {
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(rotatePJson, rotate.x, rotate.y, rotate.z, posLength)
};

class JsonLoader {

public:
	JsonLoader();
	~JsonLoader();

	void LoadJson(const std::string& _filepath, bool _isMakeFile = false);
	void SaveJson();
	void MakeJsonFile();

	void RegisterData(const std::string& _key, const std::vector<Vector3>& _data);
	void RegisterData(const std::string& _key, const std::vector<RotatePoint>& _data);

	void GetPointPositoin(const std::string& _key, std::vector<Vector3>& _array, size_t _arrayMaxSize);
	void GetData(const std::string& _key, std::vector<RotatePoint>& _array, size_t _arrayMaxSize);
	void PrepareForSave();

private:
	json data_;
	std::string folderPath_;
	std::string filePath_;
};