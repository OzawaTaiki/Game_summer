#include "TargetSphere.h"

void TargetSphere::Initialize(Model* _model, const Vector3& _position) {
	//
	model_ = _model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = _position;
}

void TargetSphere::Update() { worldTransform_.UpdateMatrix(); }

void TargetSphere::Draw(const ViewProjection& _vp, ObjectColor* _color) { model_->Draw(worldTransform_, _vp, _color); }

Vector3 TargetSphere::GetPosition() { return worldTransform_.GetWorldPositoin(); }