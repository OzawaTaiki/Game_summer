#pragma once

#include "Model.h"
#include "PlayerBullet.h"
#include "Reticle.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class BeatsCircle;
class Enemy;

class Player {
public:
	void Initialize(Model* _model, Model* _bulletModel, uint32_t _textureHandle = 0, uint32_t _reticleTextureHandle = 0);
	void Update(const ViewProjection& _vp, const std::list<std::unique_ptr<BeatsCircle>>& _beats, float _deltaTime);
	void Draw(const ViewProjection& _vp);
	void DrawReticle();

	Vector3 GetReticlePosition() { return reticle_->GetPosition(); }
	Vector2 GetReticleScreenPosition() { return reticle_->GetScreenPosition(); }
	Vector3 GetPosition() { return worldTransform_.GetWorldPositoin(); }

	float GetBulletDamage_() { return bulletDamage_; }

	float ReleseLockOnTarget();

	void OnCollsion();
	void ReticleCollision(Enemy* _enemy);

	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

private:
	void Move();

	void Attack(float _deltaTime);

	Model* model_;
	WorldTransform worldTransform_;
	uint32_t textureHandle_;

	float kCharacterSpeed_;

	std::unique_ptr<Reticle> reticle_;

	float attackCoolTime_;
	float currentTime;
	Model* bulletModel_;
	std::list<std::unique_ptr<PlayerBullet>> bullets_;
	float bulletDamage_;
	float RBDamage_;

	float maxLife_;
	float life_;
	bool isAlive_;
};