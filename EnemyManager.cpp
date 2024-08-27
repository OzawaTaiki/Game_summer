#include "EnemyManager.h"
#include "PrimitiveDrawer.h"
#include <cassert>
#include <sstream>

void EnemyManager::Initialize(Model* _model, uint32_t _textrueHandle) {
	model_ = _model;
	textureHandle_ = _textrueHandle;

	laneNum_ = 5;
	laneMargin_ = 5.0f;
	centerLane_ = {0, 0, 0};
	CalculateLane();

	LoadPopData();
}

void EnemyManager::Update() {
	PopCommandUpdate();

	for (auto& enemy : enemies_) {
		enemy->Update();
	}
}

void EnemyManager::Draw(const ViewProjection& _viewProjection) {
	for (auto& enemy : enemies_) {
		enemy->Draw(_viewProjection);
	}
}

void EnemyManager::DrawMoveLanes() {
	for (auto ml : moveLanes_) {
		Vector3 endPos = ml.origin + ml.diff * 500;
		PrimitiveDrawer::GetInstance()->DrawLine3d(ml.origin, endPos, {0, 0, 0, 1});
	}
}

void EnemyManager::Damage(float _damage) {
	for (auto it = enemies_.begin(); it != enemies_.end();) {
		(*it)->Damege(_damage);
		if (!(*it)->IsAlive())
			it = enemies_.erase(it);
		else
			it++;
	}
}

void EnemyManager::LoadPopData() {
	// ｃｓｖの読みこみ
	std::string path = ("./Resources/data/enemyPop.csv");
	file_.open(path);
	assert(file_.is_open());
}

void EnemyManager::PopCommandUpdate() {
	if (isWaitig_) {
		--waitTime_;
		if (waitTime_ <= 0)
			isWaitig_ = false;
		return;
	}

	std::string line;

	while (getline(file_, line)) {
		std::istringstream ss(line);
		std::string word;
		getline(ss, word, ',');

		if (word.find("POP") == 0) {

			getline(ss, word, ',');
			uint32_t spawnLane = atoi(word.c_str());
			getline(ss, word, ',');
			float t = static_cast<float>(atof(word.c_str()));

			Pop(GetPositionOnLane(spawnLane, t),spawnLane);

		} else if (word.find("WAIT") == 0) {

			getline(ss, word, ',');
			uint32_t waitTime = atoi(word.c_str());

			waitTime_ = waitTime;
			isWaitig_ = true;

			break;
		}
	}
}

void EnemyManager::Pop(const Vector3& _pos, uint32_t _laneNum) {
	enemies_.push_back(std::make_unique<Enemy>());
	enemies_.back()->Initialize(model_, textureHandle_, _pos, _laneNum);
}

Vector3 EnemyManager::GetPositionOnLane(uint32_t _lane, float _t) {
	Vector3 diff = moveLanes_[_lane].diff * _t;
	Vector3 result = moveLanes_[_lane].origin + diff;
	return result;
}

void EnemyManager::CalculateLane() {

	moveLanes_.resize(laneNum_);
	Vector3 diff = {0, 0, 1};
	if (laneNum_ % 2 == 1) {
		int centerIndex = static_cast<int>(laneNum_ / 2);
		for (int index = 0; index < laneNum_; ++index) {
			moveLanes_[index].origin = centerLane_;
			moveLanes_[index].origin.x = (index - centerIndex) * laneMargin_;
			moveLanes_[index].diff = diff;
		}
	} else {
		float centerIndex = static_cast<float>(laneNum_ - 1) / 2.0f;
		for (int index = 0; index < laneNum_; ++index) {
			moveLanes_[index].origin = centerLane_;
			moveLanes_[index].origin.x = (static_cast<float>(index) - centerIndex) * laneMargin_;
			moveLanes_[index].diff = diff;
		}
	}
}
