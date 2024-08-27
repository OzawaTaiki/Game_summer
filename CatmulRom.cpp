#define NOMINMAX
#include "CatmulRom.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "MyLib.h"
#include "RailCamera.h"
#include "TextureManager.h"
#include "VectorFunction.h"
#include "WinApp.h"

#include <algorithm>
#include <cmath>
#include <fstream>

CatmulRom::CatmulRom() {
	finalControlPointPosition_.reserve(maxSize_);
	finalControlPointRotation_.reserve(maxSize_);
	addPosition_ = {0.0f, 0.0f, 0.0f};
	hitRadius_ = 20.0f;
	insertNumber_ = 0;
	areaSegmentNum_ = 16;
	drawSphere_ = true;
	positionOnLine_ = 0.0f;
	positionByT_ = {0.0f, 0.0f, 0.0f};
	speed_ = 1.0f;
	targetT_ = 0.005f;
	isMove_ = false;
	drawMoveObj_ = false;
	deltaTime_ = 1.0f / 60.0f;
	totalLength_ = 0.0f;
	isChangePoint_ = false;
	selectRotation_ = false;
	addPositionByLength_ = 0.0f;
	defaultScale_ = {0.3f, 0.3f, 0.3f};
}

CatmulRom::~CatmulRom() {
	SortPointListByNumber();
	RegisterithVector3();
	// データをフォルダに書きこむ
	jsonLoader_.get()->PrepareForSave();
	jsonLoader_.get()->RegisterData("ControlPointPosition", finalControlPointPosition_);
	jsonLoader_.get()->RegisterData("ControlPointRotation", finalControlPointRotation_);
}

void CatmulRom::Initialize(const std::string& _filePath, RailCamera* _camera) {
	filePath_ = _filePath;

	jsonLoader_ = std::make_unique<JsonLoader>();
	jsonLoader_->LoadJson(filePath_, true);
	jsonLoader_->GetPointPositoin("ControlPointPosition", finalControlPointPosition_, maxSize_);
	jsonLoader_->GetData("ControlPointRotation", finalControlPointRotation_, maxSize_);

	RegisterithWorldTransform();
	RegisterRotateData();

	positionModel_.reset(Model::CreateSphere());
	rotationModel_.reset(Model::CreateFromOBJ("axisCube"));

	moveObjColor_ = std::make_unique<ObjectColor>();
	moveObjColor_->Initialize();
	moveObjColor_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
	moveObjColor_->TransferMatrix();

	selectPointPosIterator_ = currentCPPosition_.end();
	selectPointRotateIterator_ = currentCPRotation_.end();

	white_ = std::make_unique<ObjectColor>();
	white_->Initialize();
	white_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	white_->TransferMatrix();

	red_ = std::make_unique<ObjectColor>();
	red_->Initialize();
	red_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	red_->TransferMatrix();

	cameraPtr_ = _camera;
	if (currentCPPosition_.empty())
		AddPoint();
	else
		CalculateCatmulRom();
	if (currentCPRotation_.empty())
		AddPoint(0);
}

void CatmulRom::Update(const ViewProjection& _viewProjection) {

	isChangePoint_ = false;
	ShowImgui();

	if (Input::GetInstance()->IsTriggerMouse(0))
		SelectPoint(_viewProjection);

	if (isChangePoint_)
		CalculateCatmulRom();
	if (isMove_)
		CalculatePositinByT();

	for (const auto& wt : currentCPPosition_) {
		wt->worldTransform.UpdateMatrix();
	}
	for (const auto& wt : currentCPRotation_) {
		wt->worldTransform.translation_ = GetPositionByLength(wt->positionOnLine);
		wt->worldTransform.UpdateMatrix();
	}
}

void CatmulRom::Draw(const ViewProjection& _viewProjection) {

	if (drawSphere_) {
		for (const auto& wt : currentCPPosition_) {
			if (selectPointPosIterator_ != currentCPPosition_.end() && selectPointPosIterator_->get() == wt.get()) {
				positionModel_->Draw(wt->worldTransform, _viewProjection, red_.get());
			} else {
				positionModel_->Draw(wt->worldTransform, _viewProjection, white_.get());
			}
		}
	}
	for (const auto& rcp : currentCPRotation_) {
		if (selectPointRotateIterator_ != currentCPRotation_.end() && selectPointRotateIterator_->get() == rcp.get()) {
			rotationModel_->Draw(rcp->worldTransform, _viewProjection, red_.get());
		} else {
			rotationModel_->Draw(rcp->worldTransform, _viewProjection, white_.get());
		}
	}

	if (drawMoveObj_)
		rotationModel_->Draw(cameraPtr_->GetWorldTransform(), _viewProjection, moveObjColor_.get());
}

void CatmulRom::Drawpline() { DrawCatmulRomSpline(); }

void CatmulRom::CalculateCatmulRom() {
	DrawPoint_.clear();

	if (currentCPPosition_.size() < 4) {
		return;
	}
	RegisterithVector3();

	CumulativeDistances();
}

Vector3 CatmulRom::CalculateCatmulRomPoint(float _t) {
	RegisterithVector3();
	// 区間（線）の数
	size_t divisoin = finalControlPointPosition_.size() - 1;

	// 全体の位置から区間のインデックスを計算
	size_t index = static_cast<size_t>(std::floor(_t * divisoin));
	index = std::clamp(index, size_t(0), divisoin - 1);

	// 現在の区間内での正規化された位置（0から1の間）を計算
	float t1 = _t * divisoin - std::floor(_t * divisoin);

	size_t index0 = (index == 0) ? 0 : index - 1;
	size_t index1 = index;
	size_t index2 = std::min(index + 1, divisoin);
	size_t index3 = std::min(index + 2, divisoin);

	return CalculateCatmulRomAreaLine(index0, index1, index2, index3, t1);
}

Vector3 CatmulRom::CalculateCatmulRomAreaLine(size_t _index0, size_t _index1, size_t _index2, size_t _index3, float _t) {
	Vector3 p0 = finalControlPointPosition_[_index0];
	Vector3 p1 = finalControlPointPosition_[_index1];
	Vector3 p2 = finalControlPointPosition_[_index2];
	Vector3 p3 = finalControlPointPosition_[_index3];

	float t2 = _t * _t;
	float t3 = t2 * _t;

	Vector3 result;
	result.x = 0.5f * ((2.0f * p1.x) + (-p0.x + p2.x) * _t + (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 + (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);

	result.y = 0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * _t + (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 + (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);

	result.z = 0.5f * ((2.0f * p1.z) + (-p0.z + p2.z) * _t + (2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t2 + (-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t3);

	return result;
}

void CatmulRom::CalculatePositinByT() {

	if (currentCPPosition_.size() < 4)
		return;

	Vector3 rotate = {0.0f, 0.0f, 0.0f};

	positionOnLine_ += speed_ * deltaTime_;
	if (positionOnLine_ > totalLength_)
		positionOnLine_ = 0.0f;

	float posT = GetPositionParameterForDistance(positionOnLine_);

	positionByT_ = CalculateCatmulRomPoint(posT);
	cameraPtr_->SetPosition(positionByT_);

	///*回転*///
	float t = GetRotationParameterForDistance(positionOnLine_);

	Rotate(t);
}

float CatmulRom::GetPositionParameterForDistance(float _targetDistance) {
	if (cumulativeDistances_.size() == 0)
		return 0.0f;

	for (size_t i = 0; i < cumulativeDistances_.size() - 1; ++i) {
		if (cumulativeDistances_[i] <= _targetDistance && cumulativeDistances_[i + 1] >= _targetDistance) {
			float t1 = static_cast<float>(i) / (cumulativeDistances_.size() - 1);
			float t2 = static_cast<float>(i + 1) / (cumulativeDistances_.size() - 1);
			float segmentLength = cumulativeDistances_[i + 1] - cumulativeDistances_[i];
			float localT = (_targetDistance - cumulativeDistances_[i]) / segmentLength;
			return t1 + localT * (t2 - t1);
		}
	}
	return 1.0f; // 最後のポイントに達した場合
}

float CatmulRom::GetRotationParameterForDistance(float _targetDistance) {
	int i = 0;
	for (auto it = currentCPRotation_.begin(); it != currentCPRotation_.end(); ++it) {
		float currentLength = it->get()->positionOnLine;
		if (std::next(it) == currentCPRotation_.end()) {
			if (currentCPRotation_.size() == 1)
				return 0.0f;
			else
				break;
		}
		float nextLength = std::next(it)->get()->positionOnLine;
		if (currentLength <= _targetDistance && nextLength >= _targetDistance) {
			float t1 = static_cast<float>(i) / (currentCPRotation_.size() - 1);
			float t2 = static_cast<float>(i + 1) / (currentCPRotation_.size() - 1);
			float segmentLength = nextLength - currentLength;
			float localT = (_targetDistance - currentLength) / segmentLength;
			return t1 + localT * (t2 - t1);
		}
		i++;
	}
	return 1.0f; // 最後のポイントに達した場合
}

Vector3 CatmulRom::GetPositionByLength(float _length) {
	float t = GetPositionParameterForDistance(_length);
	Vector3 addp = CalculateCatmulRomPoint(t);
	return addp;
}

void CatmulRom::CumulativeDistances() {

	totalLength_ = 0.0f;
	areaLength_.clear();
	areaLength_.push_back(0.0f);
	float divisoin = areaSegmentNum_ * static_cast<float>(currentCPPosition_.size() - 1);
	float t = 0;
	DrawPoint_.push_back(CalculateCatmulRomPoint(t));
	for (size_t i = 1; i < divisoin; i++) {
		t = 1.0f / divisoin * i;
		DrawPoint_.push_back(CalculateCatmulRomPoint(t));
		areaLength_.back() += Length(DrawPoint_.back() - DrawPoint_[DrawPoint_.size() - 2]);
		if (static_cast<int>(DrawPoint_.size()) % areaSegmentNum_ == 0) {
			totalLength_ += areaLength_.back();
			if (i != divisoin - 1)
				areaLength_.push_back(0.0f);
		}
	}
	cumulativeDistances_.clear();
	cumulativeDistances_.push_back(0);
	for (size_t index = 0; index < areaLength_.size(); index++) {
		cumulativeDistances_.push_back(cumulativeDistances_.back() + areaLength_[index]);
	}
}

void CatmulRom::SelectPoint(const ViewProjection& _viewProjection) {

	// マウス座標を取得
	POINT mousePosition;
	GetCursorPos(&mousePosition);
	// クライアント座標へ変換
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);
	Vector2 mPos((float)mousePosition.x, (float)mousePosition.y);

	// スクリーン変換用の行列計算
	Matrix4x4 matViewport = MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	Matrix4x4 matViewProjectionViewport = _viewProjection.matView * _viewProjection.matProjection * matViewport;

	if (!selectRotation_) {
		for (auto it = currentCPPosition_.begin(); it != currentCPPosition_.end(); ++it) {
			// スクリーン座標へ変換
			Vector3 pointPosition = Transform((*it)->worldTransform.GetWorldPositoin(), matViewProjectionViewport);
			Vector2 pointPositionOfscreen(pointPosition.x, pointPosition.y);

			// 範囲内にあったらイテレータを保存
			float distance = Length(mPos - pointPositionOfscreen);
			if (distance <= hitRadius_) {
				// 同じものを選んでいたらキャンセル
				// そうじゃなければ選択
				if (selectPointPosIterator_ != currentCPPosition_.end() && it == selectPointPosIterator_) {
					selectPointPosIterator_ = currentCPPosition_.end();
					insertNumber_ = static_cast<uint16_t>(currentCPPosition_.size());
				} else {
					selectPointPosIterator_ = it;
					selectPointRotateIterator_ = currentCPRotation_.end();
					addPosition_ = (*it)->worldTransform.GetWorldPositoin();
					insertNumber_ = static_cast<uint16_t>(it->get()->number);
				}
			}
		}
	} else {
		for (auto it = currentCPRotation_.begin(); it != currentCPRotation_.end(); ++it) {
			Vector3 pointPosition = Transform((*it)->worldTransform.GetWorldPositoin(), matViewProjectionViewport);
			Vector2 pointPositionOfscreen(pointPosition.x, pointPosition.y);

			// 範囲内にあったらイテレータを保存
			float distance = Length(mPos - pointPositionOfscreen);
			if (distance <= hitRadius_) {
				if (selectPointRotateIterator_ != currentCPRotation_.end() && it == selectPointRotateIterator_) {
					selectPointRotateIterator_ = currentCPRotation_.end();
					insertNumber_ = static_cast<uint16_t>(currentCPRotation_.size());
				} else {
					selectPointRotateIterator_ = it;
					selectPointPosIterator_ = currentCPPosition_.end();
					addPositionByLength_ = (*it)->positionOnLine;
					insertNumber_ = static_cast<uint16_t>(it->get()->number);
				}
			}
		}
	}
}
void CatmulRom::AddPoint(bool _position) {
	if (_position) {
		// 配列追加 初期化 座標を設定 転送
		// 選択されているならそれの後ろに
		// そうじゃないなら一番後ろに
		if (selectPointPosIterator_ != currentCPPosition_.end()) {

			// 追加前に並べ替える
			SortPointListByNumber();
			// 選択されている後ろに挿入
			auto it = currentCPPosition_.insert(std::next(selectPointPosIterator_), std::make_unique<ControlPoint>());
			// 初期化
			it->get()->worldTransform.Initialize();
			it->get()->worldTransform.translation_ = addPosition_;
			it->get()->worldTransform.scale_ = defaultScale_;
			it->get()->worldTransform.UpdateMatrix();
			it->get()->number = selectPointPosIterator_->get()->number + 1;
			// 以降の番号を＋１
			for (auto i = std::next(it); i != currentCPPosition_.end(); i++) {
				i->get()->number++;
			}
		} else {
			currentCPPosition_.emplace_back(std::make_unique<ControlPoint>());
			currentCPPosition_.back()->worldTransform.Initialize();
			currentCPPosition_.back()->worldTransform.translation_ = addPosition_;
			currentCPPosition_.back()->worldTransform.scale_ = defaultScale_;
			currentCPPosition_.back()->worldTransform.UpdateMatrix();
			currentCPPosition_.back()->number = static_cast<uint16_t>(currentCPPosition_.size());
		}

	} else {
		if (selectPointRotateIterator_ != currentCPRotation_.end()) {
			auto it = currentCPRotation_.insert(std::next(selectPointRotateIterator_), std::make_unique<ControlPoint>());
			// 初期化
			it->get()->worldTransform.Initialize();
			it->get()->worldTransform.translation_ = GetPositionByLength(addPositionByLength_);
			it->get()->positionOnLine = addPositionByLength_;
			it->get()->worldTransform.scale_ = defaultScale_;
			it->get()->worldTransform.UpdateMatrix();
			it->get()->number = selectPointRotateIterator_->get()->number + 1;
			// 以降の番号を＋１
			for (auto i = std::next(it); i != currentCPRotation_.end(); i++) {
				i->get()->number++;
			}
		} else {
			currentCPRotation_.emplace_back(std::make_unique<ControlPoint>());
			currentCPRotation_.back()->worldTransform.Initialize();
			currentCPRotation_.back()->worldTransform.translation_ = GetPositionByLength(addPositionByLength_);
			currentCPRotation_.back()->positionOnLine = addPositionByLength_;
			currentCPRotation_.back()->worldTransform.scale_ = defaultScale_;
			currentCPRotation_.back()->worldTransform.UpdateMatrix();
			currentCPRotation_.back()->number = static_cast<uint16_t>(currentCPRotation_.size());
		}
	}
}
void CatmulRom::DeletePoint(bool _position) {

	if (_position) {
		if (selectPointPosIterator_ != currentCPPosition_.end()) {
			uint16_t addNum = 0;
			for (auto it = std::next(selectPointPosIterator_); it != currentCPPosition_.end(); it++) {
				it->get()->number = selectPointPosIterator_->get()->number + addNum++;
			}
		}
		currentCPPosition_.erase(selectPointPosIterator_);
		selectPointPosIterator_ = currentCPPosition_.end();
	} else {
		if (selectPointRotateIterator_ != currentCPRotation_.end()) {
			uint16_t addNum = 0;
			for (auto it = std::next(selectPointRotateIterator_); it != currentCPRotation_.end(); it++) {
				it->get()->number = selectPointRotateIterator_->get()->number + addNum++;
			}
		}
		currentCPRotation_.erase(selectPointRotateIterator_);
		selectPointRotateIterator_ = currentCPRotation_.end();
	}
}

void CatmulRom::InsertPoint() {
	// 並べ替えて
	SortPointListByNumber();
	// 番号の要素探して
	// 移動して
	if (insertNumber_ > selectPointPosIterator_->get()->number) {
		auto insertIt = std::find_if(currentCPPosition_.begin(), currentCPPosition_.end(), [this](const std::unique_ptr<ControlPoint>& cp) { return cp->number == insertNumber_ + 1; });
		currentCPPosition_.splice(insertIt, currentCPPosition_, selectPointPosIterator_);
	} else {
		auto insertIt = std::find_if(currentCPPosition_.begin(), currentCPPosition_.end(), [this](const std::unique_ptr<ControlPoint>& cp) { return cp->number == insertNumber_; });
		currentCPPosition_.splice(insertIt, currentCPPosition_, selectPointPosIterator_);
	}
	// 番号書き換え
	Renumber();
}

void CatmulRom::SortPointListByNumber() {
	currentCPPosition_.sort([](const std::unique_ptr<ControlPoint>& a, const std::unique_ptr<ControlPoint>& b) { return a->number < b->number; });
}

void CatmulRom::SortRotatePointListByPos() {
	currentCPRotation_.sort([](const std::unique_ptr<ControlPoint>& a, const std::unique_ptr<ControlPoint>& b) { return a->positionOnLine < b->positionOnLine; });
}

void CatmulRom::Renumber() {
	uint16_t addNum = 0;
	if (insertNumber_ > selectPointPosIterator_->get()->number) {
		auto it =
		    std::find_if(currentCPPosition_.begin(), currentCPPosition_.end(), [this](const std::unique_ptr<ControlPoint>& cp) { return cp->number == selectPointPosIterator_->get()->number + 1; });
		for (; it != currentCPPosition_.end(); it++) {
			it->get()->number = selectPointPosIterator_->get()->number + addNum++;
		}
	} else {
		for (auto it = selectPointPosIterator_; it != currentCPPosition_.end(); it++) {
			it->get()->number = insertNumber_ + addNum++;
		}
	}
}

void CatmulRom::RegisterithVector3() {
	finalControlPointPosition_.clear();
	for (auto it = currentCPPosition_.begin(); it != currentCPPosition_.end(); it++) {
		finalControlPointPosition_.push_back(it->get()->worldTransform.GetWorldPositoin());
	}
}

void CatmulRom::RegisterithRotate() {
	finalControlPointRotation_.clear();
	for (auto it = currentCPRotation_.begin(); it != currentCPRotation_.end(); it++) {
		RotatePoint rp;
		rp.rotate = it->get()->worldTransform.rotation_;
		// rp.rotate = EulerAngles(it->get()->worldTransform.matWorld_, defaultScale_);
		rp.posLength = it->get()->positionOnLine;
		finalControlPointRotation_.push_back(rp);
	}
}

void CatmulRom::RegisterithWorldTransform() {

	if (finalControlPointPosition_.size() == 0)
		return;

	currentCPPosition_.clear();
	for (auto it = finalControlPointPosition_.begin(); it != finalControlPointPosition_.end(); it++) {
		currentCPPosition_.push_back(std::make_unique<ControlPoint>());
		currentCPPosition_.back()->worldTransform.Initialize();
		currentCPPosition_.back()->worldTransform.translation_ = *it;
		currentCPPosition_.back()->worldTransform.scale_ = {0.3f, 0.3f, 0.3f};
		currentCPPosition_.back()->worldTransform.UpdateMatrix();
		currentCPPosition_.back()->number = static_cast<uint16_t>(currentCPPosition_.size());
	}
	CumulativeDistances();
}

void CatmulRom::RegisterRotateData() {
	currentCPRotation_.clear();
	for (auto it = finalControlPointRotation_.begin(); it != finalControlPointRotation_.end(); it++) {
		currentCPRotation_.push_back(std::make_unique<ControlPoint>());
		currentCPRotation_.back()->worldTransform.Initialize();
		currentCPRotation_.back()->worldTransform.translation_ = GetPositionByLength(it->posLength);
		currentCPRotation_.back()->worldTransform.rotation_ = it->rotate;
		currentCPRotation_.back()->worldTransform.scale_ = defaultScale_;
		currentCPRotation_.back()->worldTransform.UpdateMatrix();
		currentCPRotation_.back()->positionOnLine = it->posLength;
		currentCPRotation_.back()->number = static_cast<uint16_t>(currentCPPosition_.size());
	}
}

void CatmulRom::Rotate(float _t) {
	// 前後のポイントを取得
	// その二つで0-1で回転を補間

	RegisterithRotate();

	size_t divistion = finalControlPointRotation_.size() - 1;
	size_t index = static_cast<size_t>(std::floor(_t * divistion));
	index = std::clamp(index, size_t(0), finalControlPointRotation_.size() - 1);

	float t = _t * divistion - std::floor(_t * divistion);

	size_t index1 = index + 1;
	if (index1 > finalControlPointRotation_.size() - 1) {
		index1 = index;
	}

	Vector3 rotate1 = finalControlPointRotation_[index].rotate;
	Vector3 rotate2 = finalControlPointRotation_[index1].rotate;

	Vector3 result /*= Lerp(rotate1, rotate2, t)*/;

	result.x = LerpShortAngle(rotate1.x, rotate2.x, t);
	result.y = LerpShortAngle(rotate1.y, rotate2.y, t);
	result.z = LerpShortAngle(rotate1.z, rotate2.z, t);

	ImGui::Begin("CatmulRom");
	ImGui::Text("index : %d", index);
	ImGui::Text("t : %f", t);
	ImGui::DragFloat3("beforeRotate", &rotate1.x);
	ImGui::DragFloat3("NextRotate", &rotate2.x);
	ImGui::DragFloat3("result", &result.x);
	ImGui::End();

	cameraPtr_->SetRotate(result);
}

void CatmulRom::DrawCatmulRomSpline() {
	if (DrawPoint_.size() < 4)
		return;
	for (size_t index = 1; index < DrawPoint_.size(); index++) {
		PrimitiveDrawer::GetInstance()->DrawLine3d(DrawPoint_[index - 1], DrawPoint_[index], Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	}
}

void CatmulRom::ShowImgui() {

	ImGui::Begin("CatmulRom");
	ImGui::Checkbox("drawSphere", &drawSphere_);
	ImGui::Checkbox("drawMoveObj", &drawMoveObj_);
	if (ImGui::Checkbox("Move", &isMove_)) {
		drawSphere_ = !isMove_;
	}
	ImGui::BeginTabBar("point");
	if (ImGui::BeginTabItem("positoin")) {
		selectRotation_ = false;
		ImGui::DragFloat3("AddPosition", &addPosition_.x, 0.01f);

		if (ImGui::Button("AddControlPoint") /*|| Input::GetInstance()->PushKey(DIK_LALT) && Input::GetInstance()->IsTriggerMouse(0)*/) {
			AddPoint();
			isChangePoint_ = true;
		}
		if (ImGui::Button("DeleteControlPoint")) {
			DeletePoint();
			isChangePoint_ = true;
		}
		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("rotation")) {
		selectRotation_ = true;
		ImGui::DragFloat("AddPosition", &addPositionByLength_, 0.01f);

		if (ImGui::Button("AddControlPoint") /*|| Input::GetInstance()->PushKey(DIK_LALT) && Input::GetInstance()->IsTriggerMouse(0)*/) {
			AddPoint(0);
			isChangePoint_ = true;
		}
		if (ImGui::Button("DeleteControlPoint")) {
			DeletePoint(0);
			isChangePoint_ = true;
		}
		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();
	if (selectPointPosIterator_ != currentCPPosition_.end()) {
		if (ImGui::DragFloat3("positoin", &(selectPointPosIterator_->get()->worldTransform.translation_.x), 0.01f)) {
			isChangePoint_ = true;
		}
		/*if (ImGui::DragFloat3("rotation", &(selectPointIterator_->get()->worldTransform.rotation_.x), 0.01f)) {
		    isChangePoint_ = true;
		}*/
		ImGui::Text("number : %d", (int)selectPointPosIterator_->get()->number);

		ImGui::Spacing();
		if (ImGui::Button("Insert")) {
			InsertPoint();
			isChangePoint_ = true;
		}
		ImGui::SameLine();
		int tempNum = static_cast<int>(insertNumber_);
		ImGui::SetNextItemWidth(100.0f);
		ImGui::InputInt("Number", &tempNum);
		insertNumber_ = static_cast<uint16_t>(tempNum);
	}
	if (selectPointRotateIterator_ != currentCPRotation_.end()) {
		if (ImGui::DragFloat("positoin", &(selectPointRotateIterator_->get()->positionOnLine), 0.01f)) {
			isChangePoint_ = true;
		}
		if (ImGui::DragFloat3("rotate", &selectPointRotateIterator_->get()->worldTransform.rotation_.x, 0.01f)) {
			isChangePoint_ = true;
		}
		ImGui::Text("number : %d", (int)selectPointRotateIterator_->get()->number);
	}

	if (drawMoveObj_) {
		if (ImGui::SliderFloat("positionOnLine", &positionOnLine_, 0.0f, totalLength_))
			CalculatePositinByT();
		ImGui::DragFloat("speed", &speed_, 0.1f);
	}

	/*for (float l : areaLength_) {
	    ImGui::Text("%f", l);
	}*/

	ImGui::End();
}
