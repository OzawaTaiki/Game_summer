#include "Enemy.h"
#include "MatrixFunction.h"
#include "VectorFunction.h"
#include "WinApp.h"
#include "imgui.h"

void Enemy::Initialize(Model* _model, uint32_t _textrueHandle, const Vector3& _pos,uint32_t _lanenNumber) {
	model_ = _model;
	textureHandle_ = _textrueHandle;
	worldTransform_.Initialize();
	worldTransform_.translation_ = _pos;

	MaxHp_ = 30.0f;
	hp_ = MaxHp_;
	isAlive_ = true;

	t_ = _pos.z;
	moveSpeed_ = 0.3f;
	laneNumber_ = _lanenNumber;

}

void Enemy::Update() {

	ImGui::Begin("debug");
	ImGui::Text("hp:%.2f", hp_);
	ImGui::End();
	Move();

	worldTransform_.UpdateMatrix();
}

void Enemy::Draw(const ViewProjection& _viewProjection) { model_->Draw(worldTransform_, _viewProjection); }

void Enemy::OnCollision(float _damege) { Damege(_damege); }

Vector2 Enemy::GetScreenPos(const ViewProjection& _vp) {

	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	Matrix4x4 matViewProjectionViewport = _vp.matView * _vp.matProjection * matViewport;

	Vector3 position = Transform(worldTransform_.GetWorldPositoin(), matViewProjectionViewport);
	Vector2 result = {position.x, position.y};

	return result;
}

void Enemy::Damege(float _damege) {

	hp_ -= _damege;
	if (hp_ <= 0.0f)
		isAlive_ = false;
}

void Enemy::Move() { worldTransform_.translation_.z -= moveSpeed_; }


