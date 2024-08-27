#pragma once

#include "Model.h"
#include "VectorFunction.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class TargetSphere {
public:
	void Initialize(Model* _model, const Vector3& _position);
	void Update(void);
	void Draw(const ViewProjection& _vp, ObjectColor* _color);
	Vector3 GetPosition();

private:
	WorldTransform worldTransform_;
	Model* model_;
};