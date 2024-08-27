#include "DecisionCircle.h"
#include "TextureManager.h"

void DecisionCircle::Initialize(Model* _model, const Vector3& _position, float _time) {

	// textureHandle_ = TextureManager::Load("circle.png");
	currentTIme_ = 0.0f;
	deltaTime_ = 1.0f / 60.0f;
	deleteTime_ = _time;
	color_ = {1, 1, 1, 0.6f};
	defaultScale_ = {3.0f, 3.0f,0.1f};
	endScale_ = {1.0f, 1.0f, 0.1f};

	position_ = _position;
	model_ = _model;
	objColor_ = std::make_unique<ObjectColor>();
	objColor_->Initialize();
	objColor_->SetColor(color_);
	objColor_->TransferMatrix();

	worldTransform_.Initialize();
	worldTransform_.translation_ = position_;
	worldTransform_.scale_ = defaultScale_;
}

void DecisionCircle::Update(bool _lockOn) {

	if (!_lockOn)
		currentTIme_ += deltaTime_;
	float t = currentTIme_ / deleteTime_;
	worldTransform_.scale_ = Lerp(defaultScale_, endScale_, t);

	remainingTime_ = deleteTime_ - currentTIme_;

	worldTransform_.UpdateMatrix();
}

void DecisionCircle::Draw(const ViewProjection& _vp) {
	model_->Draw(worldTransform_, _vp, objColor_.get()); }
