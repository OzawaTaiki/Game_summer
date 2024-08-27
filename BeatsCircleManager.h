#pragma once

#include "BeatsCircle.h"
#include "Model.h"
#include "ViewProjection.h"
#include <fstream>

class BeatsCircleManager {
public:
	void Initialize(Model* _targetModel, Model* _circleModel, float deltaTime_ = 1.0f / 60.0f);
	void Update();
	void Draw(const ViewProjection& _vp);

	const std::list<std::unique_ptr<BeatsCircle>>& GetBeatsList() { return beatsCircles_; }

private:
	void LoadFile();
	void PopBeats();
	void AddBeatsCircle(const Vector3& _position, float _deathTime);

	std::list<std::unique_ptr<BeatsCircle>> beatsCircles_;

	Model* targetModel_ = nullptr;
	Model* circleModel_ = nullptr;

	float deltaTime_;
	float damageMagnification_[sizeof(Decision)];
	float decisionTime_[sizeof(Decision)];

	std::ifstream file_;

	uint32_t waitTime_;
	bool isWaitig_;
};