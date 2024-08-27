#include "Reticle.h"
#include "BeatsCircle.h"
#include "Enemy.h"
#include "Input.h"
#include "MatrixFunction.h"
#include "VectorFunction.h"
#include "WinApp.h"
#include "imgui.h"
#include <algorithm>

void Reticle::Initialize(uint32_t _textureHandle) {
	position_ = {static_cast<float>(WinApp::kWindowWidth) / 2.0f, static_cast<float>(WinApp::kWindowHeight) / 2.0f};
	distance_ = 30.0f;

	textureHandle_ = _textureHandle;
	sprite_ = std::make_unique<Sprite>();
	sprite_.reset(Sprite::Create(textureHandle_, {0, 0}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	moveSpeed_ = 0.5f;

	worldTransform_.Initialize();
	worldTransform_.translation_.z = distance_;
	worldTransform_.UpdateMatrix();

	isLockOn_ = false;
	canLockOn_ = true;
}

void Reticle::Update(const ViewProjection& _vp, const std::list<std::unique_ptr<BeatsCircle>>& _beats) {

	canLockOn_ = true;

	Vector3 move = {0, 0, 0};

	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * moveSpeed_;
		move.y += (float)joyState.Gamepad.sThumbRY / SHRT_MAX * moveSpeed_;
	}
	worldTransform_.translation_ += move;

	worldTransform_.UpdateMatrix();
	if (enemyLockOnPtr_ && !enemyLockOnPtr_->IsAlive()) {
		enemyLockOnPtr_ = nullptr;
		isLockOn_ = false;
	}
	WorldToScreen(_vp);

	TargetLockOn(_beats);

	ImGui::Begin("debug");
	ImGui::Text("ReticlePos3d :%.3f,%.3f,%.3f", worldTransform_.GetWorldPositoin().x, worldTransform_.GetWorldPositoin().y, worldTransform_.GetWorldPositoin().z);
	ImGui::Text("ReticlePos2d :%.3f,%.3f", position_.x, position_.y);
	ImGui::Text("lockOnSize : %d", lockOnTarget_.size());
	ImGui::Text("enemylock :%s", enemyLockOnPtr_ ? "true" : "false");
	ImGui::End();
}

void Reticle::Draw() { sprite_->Draw(); }

float Reticle::ReleseLockOnTarget(float _damege) {
	if (!canLockOn_)
		return 0;

	float sum = 0;
	for (auto& lockon : lockOnTarget_) {
		sum = lockon.second * _damege;
		lockon.first->Dead();
	}
	const float kMagnification = 1.1f;

	float result = kMagnification * static_cast<float>(lockOnTarget_.size()) * sum;
	return result;
}

void Reticle::EnemyLockOn(Enemy* _enemy) { enemyLockOnPtr_ = _enemy; }

Enemy* Reticle::GetLockOnEnemyPtr() {
	if (isLockOn_)
		return enemyLockOnPtr_;
	return nullptr;
}

void Reticle::OnCollision(Enemy* _enemy) {
	enemyLockOnPtr_ = _enemy;
	isLockOn_ = true;
}

void Reticle::WorldToScreen(const ViewProjection& _vp) {

	Vector3 positionReticle = worldTransform_.GetWorldPositoin();

	// ビューポート行列
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport = _vp.matView * _vp.matProjection * matViewport;

	// ワールド→スクリーン座標変換(ここで3Dから2Dになる)
	positionReticle = Transform(positionReticle, matViewProjectionViewport);
	position_ = Vector2(positionReticle.x, positionReticle.y);
	// スプライトのレティクルに座標設定
	sprite_->SetPosition(position_);
}

void Reticle::TargetLockOn(const std::list<std::unique_ptr<BeatsCircle>>& _beats) {

	auto it = std::remove_if(lockOnTarget_.begin(), lockOnTarget_.end(), [&_beats](const auto& pair) {
		return std::none_of(_beats.begin(), _beats.end(), [ptr = pair.first](const auto& beat) { return beat.get() == ptr; });
	});
	lockOnTarget_.erase(it, lockOnTarget_.end());

	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState) && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER))
		for (auto it1 = _beats.begin(); it1 != _beats.end(); ++it1) {
			if ((*it1)->IsCollision(GetPosition())) {

				// 同じiteratorを探す
				auto it2 = std::find_if(lockOnTarget_.begin(), lockOnTarget_.end(), [it1](const auto& _pair) { return _pair.first == it1->get(); });

				// なかったときにpushBack
				if (it2 == lockOnTarget_.end()) {
					if ((*it1)->LockOn()) {
						lockOnTarget_.push_back(std::make_pair(it1->get(), (*it1)->JudgeTiming()));
						canLockOn_ = false;
					}
				}
			}
		}

	lockOnPosition_.clear();
	for (auto& lockon : lockOnTarget_) {
		lockOnPosition_.push_back(lockon.first->getPosition());
	}
}
