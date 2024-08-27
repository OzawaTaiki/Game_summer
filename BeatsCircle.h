#pragma once

#include "DecisionCircle.h"
#include "Model.h"
#include "TargetSphere.h"
#include <list>

enum class Decision { parfect, good, miss, none };

class BeatsCircle {
public:
	void Initilaize(Model* _targetModel, Model* _circleModel, const Vector3& _position, float _deleteTime);
	bool Update();
	void Draw(const ViewProjection& _vp);

	bool IsCollision(const Vector3& _pos);
	float GetRemainingTime() { return decisionCircle_->GetRemainingTime(); };
	Vector3 getPosition() { return targetSphere_->GetPosition(); };

	float JudgeTiming();
	bool LockOn();

	void Dead() { isDead_ = true; }

private:
	bool IsDestroy();

	std::unique_ptr<DecisionCircle> decisionCircle_;
	std::unique_ptr<TargetSphere> targetSphere_;

	float decisionTime_[sizeof(Decision)];
	float damageMagnification_[sizeof(Decision)];

	Vector4 color_[sizeof(Decision)];
	std::unique_ptr<ObjectColor> objColor_;

	bool isLockOn_;

	bool isDead_;
};
