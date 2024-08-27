#pragma once

#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <list>
#include <memory>
#include <vector>

class BeatsCircle;
class Enemy;

class Reticle {
public:
	void Initialize(uint32_t _textureHandle);
	void Update(const ViewProjection& _vp, const std::list<std::unique_ptr<BeatsCircle>>& _beats);
	void Draw();

	Vector3 GetPosition() { return worldTransform_.GetWorldPositoin(); }
	Vector2 GetScreenPosition() { return sprite_->GetPosition(); }
	std::vector<Vector3> GetLockOnPosition() { return lockOnPosition_; }

	float ReleseLockOnTarget(float _damege);
	void EnemyLockOn(Enemy* _enemy);
	Enemy* GetLockOnEnemyPtr();
	void OnCollision(Enemy* _enemy);

private:
	void WorldToScreen(const ViewProjection& _vp);

	void TargetLockOn(const std::list<std::unique_ptr<BeatsCircle>>& _beats);

	Vector2 position_;
	float distance_;
	uint32_t textureHandle_;

	WorldTransform worldTransform_;
	float moveSpeed_;

	// 対象イテレータ，ダメージ倍率
	std::list<std::pair<BeatsCircle*, float>> lockOnTarget_;
	std::vector<Vector3> lockOnPosition_;

	Enemy* enemyLockOnPtr_ = nullptr;
	bool isLockOn_;

	bool canLockOn_;

	std::unique_ptr<Sprite> sprite_;
};