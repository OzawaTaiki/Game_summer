#pragma once

#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "MyLib.h"


class Enemy {
public:
	void Initialize(Model* _model, uint32_t _textrueHandle, const Vector3& _pos, uint32_t _lanenNumber);
	void Update();
	void Draw(const ViewProjection& _viewProjection);

	void SetPosition(const Vector3& _pos) { worldTransform_.translation_ = _pos; }

	void OnCollision(float _damege);

	Vector2 GetScreenPos(const ViewProjection& _vp);

	Vector3 GetPosition() { return worldTransform_.GetWorldPositoin(); }

	void Damege(float _damege);
	bool IsAlive() { return isAlive_; };


	uint32_t laneNumber_;

private:
	void Move();
	Model* model_;
	uint32_t textureHandle_;
	WorldTransform worldTransform_;

	float t_;
	float moveSpeed_;

	float MaxHp_;
	float hp_;

	bool isAlive_;
};
