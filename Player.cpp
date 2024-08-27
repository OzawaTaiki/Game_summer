#include "Player.h"
#include "Enemy.h"
#include "Input.h"
#include "VectorFunction.h"

void Player::Initialize(Model* _model, Model* _bulletModel, uint32_t _textureHandle, uint32_t _reticleTextureHandle) {
	model_ = _model;
	textureHandle_ = _textureHandle;

	worldTransform_.Initialize();

	kCharacterSpeed_ = 0.3f;

	attackCoolTime_ = 0.5f;
	bulletModel_ = _bulletModel;
	bulletDamage_ = 1.0f;
	RBDamage_ = 5.0f;

	reticle_ = std::make_unique<Reticle>();
	reticle_->Initialize(_reticleTextureHandle);

	maxLife_ = 3.0f;
	life_ = maxLife_;
	isAlive_ = true;
}

void Player::Update(const ViewProjection& _vp, const std::list<std::unique_ptr<BeatsCircle>>& _beats, float _deltaTime) {

	bullets_.remove_if([](const std::unique_ptr<PlayerBullet>& _bullet) { return _bullet->IsDead(); });

	Move();
	reticle_->Update(_vp, _beats);
	Attack(_deltaTime);

	for (auto& pb : bullets_)
		pb->Update();

	worldTransform_.UpdateMatrix();
}

void Player::Draw(const ViewProjection& _vp) {
	for (auto& pb : bullets_)
		pb->Draw(_vp);
	model_->Draw(worldTransform_, _vp, textureHandle_);
}

void Player::DrawReticle() { reticle_->Draw(); }

float Player::ReleseLockOnTarget() { return reticle_->ReleseLockOnTarget(RBDamage_); }

void Player::OnCollsion() {
	life_--;
	if (life_ <= 0.0f)
		isAlive_ = false;
}

void Player::ReticleCollision(Enemy* _enemy) {

	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState) && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)) {
		reticle_->OnCollision(_enemy);
	}
}

void Player::Move() {
	Vector3 move = {0, 0, 0};

	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed_;
		// move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed_;
	} else {
		if (Input::GetInstance()->PushKey(DIK_W))
			move.y += kCharacterSpeed_;
		if (Input::GetInstance()->PushKey(DIK_A))
			move.x -= kCharacterSpeed_;
		if (Input::GetInstance()->PushKey(DIK_S))
			move.y -= kCharacterSpeed_;
		if (Input::GetInstance()->PushKey(DIK_D))
			move.x += kCharacterSpeed_;
	}

	worldTransform_.translation_ += move;
}

void Player::Attack(float _deltaTime) {
	// 5
	currentTime += _deltaTime;
	if (currentTime >= attackCoolTime_) {
		currentTime = 0;
		bullets_.push_back(std::make_unique<PlayerBullet>());
		if (reticle_->GetLockOnEnemyPtr()) {
			// velocity計算
			Vector3 pPos = worldTransform_.GetWorldPositoin();
			Vector3 ePos = reticle_->GetLockOnEnemyPtr()->GetPosition();

			Vector3 directoin = ePos - pPos;
			directoin = Normalize(directoin);

			Vector3 velocity = directoin;

			bullets_.back()->initialize(bulletModel_, worldTransform_.GetWorldPositoin(), velocity);

		} else {
			bullets_.back()->initialize(bulletModel_, worldTransform_.GetWorldPositoin(), {0, 0, 1});
		}
	}
}
