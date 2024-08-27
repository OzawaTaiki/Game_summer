#pragma once

#include "Model.h"
#include "VectorFunction.h"
#include "WorldTransform.h"

class DecisionCircle {
public:
	void Initialize(Model* _model, const Vector3& _position, float _time);
	void Update(bool _lockOn);
	void Draw(const ViewProjection& _vp);

	/// <summary>
	/// 残り時間を取得する
	/// </summary>
	/// <returns>残り時間</returns>
	float GetRemainingTime() { return remainingTime_; }

private:
	WorldTransform worldTransform_;
	Model* model_;
	std::unique_ptr<ObjectColor> objColor_;

	Vector3 position_;
	uint32_t textureHandle_;
	Vector4 color_;

	Vector3 defaultScale_;
	Vector3 endScale_;

	float deltaTime_;
	float currentTIme_;
	float deleteTime_;
	float remainingTime_;
};