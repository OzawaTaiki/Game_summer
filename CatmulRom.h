#pragma once
#include "ImGuiManager.h"
#include "JsonLoader.h"
#include "MatrixFunction.h"
#include "Model.h"
#include "PrimitiveDrawer.h"
#include "Structures.h"
#include "VectorFunction.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include <list>
#include <string>
#include <vector>

class RailCamera;

struct ControlPoint {
	WorldTransform worldTransform;
	float positionOnLine;
	uint16_t number;
};

class CatmulRom {
public:
	CatmulRom();
	~CatmulRom();

	void Initialize(const std::string& _filePath, RailCamera* _camera);
	void Update(const ViewProjection& _viewProjection);
	void Draw(const ViewProjection& _viewProjection);
	void Drawpline();

	bool GetIsMove() { return isMove_; };

private:
	std::list<std::unique_ptr<ControlPoint>>::iterator selectPointPosIterator_;
	std::list<std::unique_ptr<ControlPoint>>::iterator selectPointRotateIterator_;
	std::list<std::unique_ptr<ControlPoint>> currentCPPosition_; // 編集中保存用
	std::list<std::unique_ptr<ControlPoint>> currentCPRotation_; // 編集中保存用
	std::vector<Vector3> finalControlPointPosition_;             // 確定保存用
	std::vector<RotatePoint> finalControlPointRotation_;         // 確定保存用
	std::vector<Vector3> DrawPoint_;

	Vector3 addPosition_;       // 追加するpos
	float addPositionByLength_; // 追加するpos
	uint16_t insertNumber_;
	float hitRadius_;

	bool drawSphere_;
	bool isMove_;
	bool drawMoveObj_;

	bool selectRotation_;

	Vector3 defaultScale_;

	Vector3 positionByT_;
	float positionOnLine_;
	float speed_;
	float targetT_;
	float deltaTime_;

	bool isChangePoint_;

	const size_t maxSize_ = 128;
	size_t areaSegmentNum_;

	float totalLength_;
	std::vector<float> areaLength_;
	std::vector<float> cumulativeDistances_;

	std::unique_ptr<Model> positionModel_;
	std::unique_ptr<Model> rotationModel_;
	std::unique_ptr<ObjectColor> moveObjColor_;
	std::unique_ptr<ObjectColor> white_;
	std::unique_ptr<ObjectColor> red_;

	std::string filePath_;

	RailCamera* cameraPtr_ = nullptr;

	std::unique_ptr<JsonLoader> jsonLoader_;

	void CalculateCatmulRom();

	/// <summary>
	/// Catmull-Rom コントロールポイントの計算
	/// </summary>
	Vector3 CalculateCatmulRomPoint(float _t);

	Vector3 CalculateCatmulRomAreaLine(size_t _index0, size_t _index1, size_t _index2, size_t _index3, float _t);

	void CalculatePositinByT();
	float GetPositionParameterForDistance(float _targetDistance);
	float GetRotationParameterForDistance(float _targetDistance);

	Vector3 GetPositionByLength(float _length);

	void CumulativeDistances();

	/// <summary>
	/// ポイントの選択
	/// 球とマウスポインタの判定
	/// </summary>
	/// <param name="_viewProjection">viewProjection行列</param>
	void SelectPoint(const ViewProjection& _viewProjection);

	/// <summary>
	/// コントロールポイントの追加
	/// </summary>
	void AddPoint(bool _position = true);

	/// <summary>
	/// コントロールポイントの削除
	/// </summary>
	void DeletePoint(bool _position = true);

	/// <summary>
	/// コントロールポイントの挿入
	/// </summary>
	void InsertPoint();

	/// <summary>
	/// コントロールポイントlistをnumberの昇順に並べ替え
	/// </summary>
	void SortPointListByNumber();

	void SortRotatePointListByPos();

	/// <summary>
	/// numberの書き換え
	/// </summary>
	void Renumber();

	void RegisterithVector3();
	void RegisterithRotate();

	void RegisterithWorldTransform();
	void RegisterRotateData();

	void Rotate(float _t);

	void DrawCatmulRomSpline();

	void ShowImgui();
};