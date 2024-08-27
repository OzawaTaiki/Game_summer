#include "skydome.h"
#include "ImGuiManager.h"
#include "MatrixFunction.h"
#include <cassert>

Skydome::~Skydome() {}

void Skydome::Initialze(Model* _model) {
	assert(_model);
	model = _model;
	worldTransform.Initialize();
	worldTransform.scale_ = {1.0f, 1.0f, 1.0f};
}

void Skydome::Update() {
	ImGui();
	worldTransform.matWorld_ =  MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
	worldTransform.TransferMatrix();
}

void Skydome::Draw(ViewProjection& _viewProjection) { model->Draw(worldTransform, _viewProjection); }

void Skydome::ImGui() {
	ImGui::Begin("skydoom");
	ImGui::DragFloat3("Scale", &worldTransform.scale_.x, 0.1f);
	ImGui::DragFloat3("Rotation", &worldTransform.rotation_.x, 0.1f);
	ImGui::DragFloat3("Translation", &worldTransform.translation_.x, 0.1f);
	ImGui::End();
}
