#include "BeatsCircle.h"
#include "Input.h"

void BeatsCircle::Initilaize(Model* _targetModel, Model* _circleModel, const Vector3& _position, float _deleteTime) {

	targetSphere_ = std::make_unique<TargetSphere>();
	targetSphere_->Initialize(_targetModel, _position);

	decisionCircle_ = std::make_unique<DecisionCircle>();
	decisionCircle_->Initialize(_circleModel, _position, _deleteTime);

	decisionTime_[static_cast<size_t>(Decision::parfect)] = 1.0f;
	decisionTime_[static_cast<size_t>(Decision::good)] = 2.0f;
	decisionTime_[static_cast<size_t>(Decision::miss)] = 3.0f;

	damageMagnification_[static_cast<size_t>(Decision::parfect)] = 1.0f;
	damageMagnification_[static_cast<size_t>(Decision::good)] = 0.75f;
	damageMagnification_[static_cast<size_t>(Decision::miss)] = 0.5f;
	damageMagnification_[static_cast<size_t>(Decision::none)] = 0.0f;

	color_[static_cast<size_t>(Decision::parfect)] = {1.0f, 0.45f, 0.6f, 1.0f};
	color_[static_cast<size_t>(Decision::good)] = {0.55f, 0.9f, 0.55f, 1.0f};
	color_[static_cast<size_t>(Decision::miss)] = {0.3f, 0.3f, 0.3f, 1.0f};
	color_[static_cast<size_t>(Decision::none)] = {1.0f, 1.0f, 1.0f, 1.0f};

	objColor_ = std::make_unique<ObjectColor>();
	objColor_->Initialize();

	isLockOn_ = false;

	isDead_ = false;
}

bool BeatsCircle::Update() {

	targetSphere_->Update();
	decisionCircle_->Update(isLockOn_);

	return IsDestroy();
}

void BeatsCircle::Draw(const ViewProjection& _vp) {
	targetSphere_->Draw(_vp, objColor_.get());
	decisionCircle_->Draw(_vp);
}

bool BeatsCircle::IsDestroy() {

	if (decisionCircle_->GetRemainingTime() <= 0 || isDead_) {
		targetSphere_.reset();
		decisionCircle_.reset();
		return true;
	}
	return false;
}

bool BeatsCircle::IsCollision(const Vector3& _pos) {

	Vector3 sPos = targetSphere_->GetPosition();

	float kRadius = 1.0f;

	float distance = Length(sPos - _pos);

	if (distance <= kRadius)
		return true;
	return false;
}

float BeatsCircle::JudgeTiming() {

	Decision decision;

	float time = GetRemainingTime();
	if (time <= decisionTime_[static_cast<size_t>(Decision::parfect)])
		decision = Decision::parfect;
	else if (time <= decisionTime_[static_cast<size_t>(Decision::good)])
		decision = Decision::good;
	else if (time <= decisionTime_[static_cast<size_t>(Decision::miss)])
		decision = Decision::miss;
	else
		decision = Decision::none;

	objColor_->SetColor(color_[static_cast<size_t>(decision)]);
	objColor_->TransferMatrix();

	return damageMagnification_[static_cast<size_t>(decision)];
}

bool BeatsCircle::LockOn() {
	if (JudgeTiming() != damageMagnification_[static_cast<size_t>(Decision::none)]) {
		// JudgeTiming();
		isLockOn_ = true;
		return true;
	}
	return false;
}
