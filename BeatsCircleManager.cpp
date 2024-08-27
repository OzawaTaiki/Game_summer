#include "BeatsCircleManager.h"
#include "Input.h"
#include <cassert>
#include <sstream>

void BeatsCircleManager::Initialize(Model* _targetModel, Model* _circleModel, float _deltaTime) {
	targetModel_ = _targetModel;
	circleModel_ = _circleModel;
	deltaTime_ = _deltaTime;

	waitTime_ = 0;
	isWaitig_ = false;

	LoadFile();

	// AddBeatsCircle();
}
void BeatsCircleManager::Update() {
	// if (Input::GetInstance()->TriggerKey(DIK_SPACE))
	//	AddBeatsCircle();
	PopBeats();

	/// 判定円が球と重なったとき対象を消す
	for (auto it = beatsCircles_.begin(); it != beatsCircles_.end();) {
		if ((*it)->Update()) {
			it = beatsCircles_.erase(it);
		} else {
			it++;
		}
	}
}

void BeatsCircleManager::Draw(const ViewProjection& _vp) {
	for (auto& bc : beatsCircles_) {
		bc->Draw(_vp);
	}
}

void BeatsCircleManager::LoadFile() {

	// ｃｓｖの読みこみ
	std::string path = ("./Resources/Data/beatsPop.csv");
	file_.open(path);
	assert(file_.is_open());
}

void BeatsCircleManager::PopBeats() {

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

			Vector3 loadpos;

			getline(ss, word, ',');
			loadpos.x = static_cast<float>(atof(word.c_str()));

			getline(ss, word, ',');
			loadpos.y = static_cast<float>(atof(word.c_str()));

			loadpos.z = 30.0f;

			getline(ss, word, ',');
			float time = static_cast<float>(atof(word.c_str()));

			AddBeatsCircle(loadpos, time);
		} else if (word.find("WAIT") == 0) {

			getline(ss, word, ',');
			uint32_t waitTime = atoi(word.c_str());

			waitTime_ = waitTime;
			isWaitig_ = true;

			break;
		}
	}
}

void BeatsCircleManager::AddBeatsCircle(const Vector3& _position, float _deathTime) {
	beatsCircles_.push_back(std::make_unique<BeatsCircle>());
	beatsCircles_.back()->Initilaize(targetModel_, circleModel_, _position, _deathTime);
}
