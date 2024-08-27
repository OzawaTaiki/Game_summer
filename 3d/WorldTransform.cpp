#include "WorldTransform.h"
#include "MatrixFunction.h"

void WorldTransform::UpdateMatrix() {
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

	if (parent_) {
		matWorld_ = matWorld_ * parent_->matWorld_;
	}

	TransferMatrix();
}

Vector3 WorldTransform::GetWorldPositoin() {
	Vector3 wPos;
	wPos.x = matWorld_.m[3][0];
	wPos.y = matWorld_.m[3][1];
	wPos.z = matWorld_.m[3][2];
	return wPos;
}
