#include "RailCamera.h"
#include "ImGuiManager.h"
#include "MatrixFunction.h"
#include "VectorFunction.h"

void RailCamera::Initialize(const Vector3& _worldPos, const Vector3& _rotare) {

	worldTransform.Initialize();
	worldTransform.translation_ = _worldPos;
	worldTransform.rotation_ = _rotare;
	isStop = false;
	viewProjection.farZ = 2000.0f;
	viewProjection.Initialize();
	startPosition = _worldPos;
	startRotate = _rotare;
}

void RailCamera::Update() {

	Imgui();

	worldTransform.UpdateMatrix();

	viewProjection.matView = Inverse(worldTransform.matWorld_);
}

void RailCamera::SetPosition(const Vector3& _pos) { worldTransform.translation_ = _pos; }

void RailCamera::SetRotate(const Vector3& _rotate) { worldTransform.rotation_ = _rotate; }

void RailCamera::SetTranslate(const Vector3& _move, const Vector3& _rotare) {
	if (!isStop) {
		worldTransform.translation_ += _move;
		worldTransform.rotation_ += _rotare;
	}
}

void RailCamera::Imgui() {
	ImGui::Begin("RailCamera");
	ImGui::DragFloat3("translate", &worldTransform.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &worldTransform.rotation_.x, 0.01f);
	if (ImGui::Button("reset"))
	{
		worldTransform.translation_ = startPosition;
		worldTransform.rotation_ = startRotate;
		worldTransform.UpdateMatrix();
	}
	// ImGui::Checkbox("isStop", &isStop);
	ImGui::End();
}
