#pragma once

#include <Model.h>
#include <WorldTransform.h>

class Skydome {
public:
	~Skydome();

	void Initialze(Model* _model);
	void Update();
	void Draw(ViewProjection& _viewProjection);

private:
	void ImGui();

	WorldTransform worldTransform;
	Model* model = nullptr;
};
