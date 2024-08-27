#define NOMINMAX
#include "MyLib.h"
#include <algorithm>
#define _USE_MATH_DEFINES
#include <cmath>
#include <numbers>

#include <imgui.h>

float LerpShortAngle(float _a, float _b, float _t) {
	// 角度差を計算
	float diff = _b - _a;
	// 差を [-pi, pi] の範囲に調整
	float rotate = std::remainder(diff, std::numbers::pi_v<float> * 2.0f);

	// 最短角度を使って補間
	return _a + rotate * _t;
}

Vector3 Project(const Vector3& _v1, const Vector3& _v2) {
	Vector3 normalize = Normalize(_v2);

	Vector3 result = Multiply(Dot(_v1, normalize), normalize);

	return result;
}

Vector3 ClosestPoint(const Vector3& _point, const Segment& _segment) {
	Vector3 cp = Add(_segment.origin, Project(Subtract(_point, _segment.origin), _segment.diff));

	return cp;
}

Vector3 Perpendicular(const Vector3& _v) {
	if (_v.x != 0.0f || _v.y != 0.0f) {
		return {-_v.y, _v.x, 0.0f};
	}
	return {0.0f, -_v.z, _v.y};
}

void GetPlaneVertex(const Plane& _plane, Vector3* _vertex) {
	Vector3 center = _plane.normal * _plane.distance;

	_vertex[0] = Normalize(Perpendicular(_plane.normal));
	_vertex[1] = {-_vertex[0].x, -_vertex[0].y, -_vertex[0].z};
	_vertex[2] = Cross(_plane.normal, _vertex[0]);
	_vertex[3] = {-_vertex[2].x, -_vertex[2].y, -_vertex[2].z};

	for (int i = 0; i < 4; i++)
		_vertex[i] = _vertex[i] * _plane.scalar + center;
}

bool IsCollision(const Sphere& _s1, const Sphere& _s2) {
	float distance = Length(Subtract(_s1.center, _s2.center));
	return distance <= _s1.radius + _s2.radius ? true : false;
}

bool IsCollision(const Sphere& _s, const Plane& _p) {
	float distance = Dot(_p.normal, _s.center) - _p.distance;
	distance = distance < 0 ? -distance : distance;

	if (distance <= _s.radius)
		return true;
	return false;
}

bool IsCollision(const Plane& _plane, const Segment& _segment) {
	float dot = Dot(_plane.normal, _segment.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (_plane.distance - Dot(_segment.origin, _plane.normal)) / dot;

	if (t < 0.0f || t > 1.0f)
		return false;

	return true;
}

bool IsCollision(const Triangle& _triangle, const Segment& _segment) {
	Plane lPlane = CalculatePlane(_triangle);

	float dot = Dot(lPlane.normal, _segment.diff);

	if (dot == 0.0f) {
		return false;
	}

	float t = (lPlane.distance - Dot(_segment.origin, lPlane.normal)) / dot;

	if (t < 0.0f || t > 1.0f)
		return false;

	Vector3 point = _segment.origin + _segment.diff * t;

	Vector3 v01 = _triangle.vertices[1] - _triangle.vertices[0];
	Vector3 v1p = point - _triangle.vertices[1];

	Vector3 v12 = _triangle.vertices[2] - _triangle.vertices[1];
	Vector3 v2p = point - _triangle.vertices[2];

	Vector3 v20 = _triangle.vertices[0] - _triangle.vertices[2];
	Vector3 v0p = point - _triangle.vertices[0];

	Vector3 cross01 = Cross(v01, v1p);
	Vector3 cross12 = Cross(v12, v2p);
	Vector3 cross20 = Cross(v20, v0p);

	if (Dot(cross01, lPlane.normal) >= 0.0f && Dot(cross12, lPlane.normal) >= 0.0f && Dot(cross20, lPlane.normal) >= 0.0f) {
		return true;
	}
	return false;
}

bool IsCollision(const AABB& _a, const AABB& _b) {
	if ((_a.min.x <= _b.max.x && _a.max.x >= _b.min.x) && // x
	    (_a.min.y <= _b.max.y && _a.max.y >= _b.min.y) && // y
	    (_a.min.z <= _b.max.z && _a.max.z >= _b.min.z)) { // z
		// 衝突
		return true;
	}

	return false;
}

bool IsCollision(const AABB& _a, const Sphere& _s) {

	Vector3 closestPoint;
	closestPoint.x = std::clamp(_s.center.x, _a.min.x, _a.max.x);
	closestPoint.y = std::clamp(_s.center.y, _a.min.y, _a.max.y);
	closestPoint.z = std::clamp(_s.center.z, _a.min.z, _a.max.z);

	float distance = Length(closestPoint - _s.center);

	if (distance <= _s.radius) {
		return true;
	}

	return false;
}

bool IsCollision(const AABB& _aabb, const Segment& _segment) {
	Vector3 tminVec;
	Vector3 tmaxVec;

	tminVec.y = (_aabb.min.y - _segment.origin.y) / _segment.diff.y;
	tminVec.x = (_aabb.min.x - _segment.origin.x) / _segment.diff.x;
	tminVec.z = (_aabb.min.z - _segment.origin.z) / _segment.diff.z;

	tmaxVec.x = (_aabb.max.x - _segment.origin.x) / _segment.diff.x;
	tmaxVec.y = (_aabb.max.y - _segment.origin.y) / _segment.diff.y;
	tmaxVec.z = (_aabb.max.z - _segment.origin.z) / _segment.diff.z;

	Vector3 tNear;
	tNear.x = std::min(tminVec.x, tmaxVec.x);
	tNear.y = std::min(tminVec.y, tmaxVec.y);
	tNear.z = std::min(tminVec.z, tmaxVec.z);

	Vector3 tFar;
	tFar.x = std::max(tminVec.x, tmaxVec.x);
	tFar.y = std::max(tminVec.y, tmaxVec.y);
	tFar.z = std::max(tminVec.z, tmaxVec.z);

	float tmin = std::max(std::max(tNear.x, tNear.y), tNear.z);
	float tmax = std::min(std::min(tFar.x, tFar.y), tFar.z);

#ifdef _DEBUG

	ImGui::Begin("aaa");
	ImGui::Text("tmin : %.3f", tmin);
	ImGui::Text("tmax ; %.3f", tmax);
	ImGui::End();

#endif // _DEBUG

	if (tmin <= tmax && tmax >= 0.0f && tmin <= 1.0f) {
		// 衝突
		return true;
	}

	return false;
}

bool IsCollision(const OBB& _obb, const Sphere& _sphere) {
	Matrix4x4 obbWolrdMat = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, _obb.rotate, _obb.center);
	Matrix4x4 obbWorldMatInv = Inverse(obbWolrdMat);

	Vector3 centerInOBBLocalSphere = Transform(_sphere.center, obbWorldMatInv);
	AABB aabbOBBLocal{.min = -_obb.size, .max = _obb.size};
	Sphere sphereOBBLocal{centerInOBBLocalSphere, _sphere.radius};

	return IsCollision(aabbOBBLocal, sphereOBBLocal);
}

bool IsCollision(const OBB& _obb, const Segment& _segment) {
	Matrix4x4 obbWolrdMat = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, _obb.rotate, _obb.center);
	Matrix4x4 obbWorldMatInv = Inverse(obbWolrdMat);
	Vector3 localOrigin = Transform(_segment.origin, obbWorldMatInv);
	Vector3 localEnd = Transform(_segment.origin + _segment.diff, obbWorldMatInv);

	AABB localAABB{
	    {-_obb.size.x, -_obb.size.y, -_obb.size.z},
	    {+_obb.size.x, +_obb.size.y, +_obb.size.z},
	};
	Segment localSegment;
	localSegment.origin = localOrigin;
	localSegment.diff = localEnd - localOrigin;

	return IsCollision(localAABB, localSegment);
}

bool IsCollision(const OBB& _obb1, const OBB& _obb2) {
	/// 分離軸候補の計算
	Vector3 axes[15];
	axes[0] = _obb1.orientations[0];
	axes[1] = _obb1.orientations[1];
	axes[2] = _obb1.orientations[2];

	axes[3] = _obb2.orientations[0];
	axes[4] = _obb2.orientations[1];
	axes[5] = _obb2.orientations[2];

	int index = 6;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			axes[index++] = Cross(_obb1.orientations[i], _obb2.orientations[j]);
		}
	}

	for (auto axis : axes) {
		float minObb1, maxObb1;
		float minObb2, maxObb2;

		// 軸に射影および点の最大と最小を求める
		CalculateProjectionRange(_obb1, axis, minObb1, maxObb1);
		CalculateProjectionRange(_obb2, axis, minObb2, maxObb2);

		float l1, l2;
		l1 = maxObb1 - minObb1;
		l2 = maxObb2 - minObb2;

		float sumSpan = l1 + l2;

		float longSpan = std::max(maxObb1, maxObb2) - std::min(minObb1, minObb2);
		if (sumSpan < longSpan) { // 分離している	//すなわち衝突していない
			return false;
		}
	}
	return true;
}

Plane CalculatePlane(const Triangle& _triangle) {
	Plane result{};

	// ab,bcを求める
	Vector3 edge1 = _triangle.vertices[1] - _triangle.vertices[0];
	Vector3 edge2 = _triangle.vertices[2] - _triangle.vertices[1];

	// 法線計算
	result.normal = Cross(edge1, edge2);
	result.normal = Normalize(result.normal);

	// 平面方程式を用いて距離を求める
	result.distance = Dot(_triangle.vertices[0], result.normal);

	return result;
}

void CalculateProjectionRange(const OBB& _obb, const Vector3& _axis, float& _min, float& _max) {
	Vector3 verties[8];
	_obb.CaluculateVertices(verties);

	_min = (float)10000;
	_max = -(float)-10000;

	for (auto vertex : verties) {
		float proj = Dot(_axis, vertex);
		_min = std::min(_min, proj);
		_max = std::max(_max, proj);
	}

	return;
}

Vector3 CalculatePointBezier(const Bezier& _bezier, float _t) {
	Vector3 point[3];
	point[0] = Lerp(_bezier.point[0], _bezier.point[1], _t);
	point[1] = Lerp(_bezier.point[1], _bezier.point[2], _t);
	point[2] = Lerp(point[0], point[1], _t);

	return point[2];
}

Vector3 CalculatePointCatmullRom(const Vector3& _cPoint0, const Vector3& _cPoint1, const Vector3& _cPoint2, const Vector3& _cPoint3, float _t) {
	Vector3 result;

	result.x = 1.0f / 2.0f *
	           ((-_cPoint0.x + 3 * _cPoint1.x - 3 * _cPoint2.x + _cPoint3.x) * _t * _t * _t + (2 * _cPoint0.x - 5 * _cPoint1.x + 4 * _cPoint2.x - _cPoint3.x) * _t * _t +
	            (-_cPoint0.x + _cPoint2.x) * _t + 2 * _cPoint1.x);

	result.y = 1.0f / 2.0f *
	           ((-_cPoint0.y + 3 * _cPoint1.y - 3 * _cPoint2.y + _cPoint3.y) * _t * _t * _t + (2 * _cPoint0.y - 5 * _cPoint1.y + 4 * _cPoint2.y - _cPoint3.y) * _t * _t +
	            (-_cPoint0.y + _cPoint2.y) * _t + 2 * _cPoint1.y);

	result.z = 1.0f / 2.0f *
	           ((-_cPoint0.z + 3 * _cPoint1.z - 3 * _cPoint2.z + _cPoint3.z) * _t * _t * _t + (2 * _cPoint0.z - 5 * _cPoint1.z + 4 * _cPoint2.z - _cPoint3.z) * _t * _t +
	            (-_cPoint0.z + _cPoint2.z) * _t + 2 * _cPoint1.z);

	return result;
}

Vector3 CalculatePointCatmullRom(const std::vector<Vector3>& _points, float _t) {

	assert(_points.size() >= 4 && "制御点は4点以上必要です");

	// 区間数は制御点の数-1
	size_t division = _points.size() - 1;
	// 1区間の長さ(全体を1.0とした割合)
	float areaWidth = 1.0f / division;

	// 区間内の始点を0.0f、終点を1.0fとしたときの現在位置
	float t_2 = std ::fmod(_t, areaWidth) * division;
	// 下限(0.of)と上限(1.0f)の範囲に収める
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	// 区間番号
	size_t index = static_cast<size_t>(_t / areaWidth);
	// 区間番号が上限を超えないように収める
	index = std::clamp(index, size_t(0), _points.size() - 2);

	// 4点分のインデックス
	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	// 最初の区間のpoはp1を重複使用する
	if (index == 0) {
		index0 = index1;
	}

	// 最後の区間のp3はp2を重複使用する
	if (index3 >= _points.size()) {
		index3 = index2;
	}

	// 4点の座標
	const Vector3& p0 = _points[index0];
	const Vector3& p1 = _points[index1];
	const Vector3& p2 = _points[index2];
	const Vector3& p3 = _points[index3];

	// 4点を指定してCatmull-Rom補間
	return CalculatePointCatmullRom(p0, p1, p2, p3, t_2);
}

void AABB::Update() {
	this->min.x = (std::min)(this->min.x, this->max.x);
	this->max.x = (std::max)(this->min.x, this->max.x);
	this->min.y = (std::min)(this->min.y, this->max.y);
	this->max.y = (std::max)(this->min.y, this->max.y);
	this->min.z = (std::min)(this->min.z, this->max.z);
	this->max.z = (std::max)(this->min.z, this->max.z);
}

void OBB::CalculateOrientations() {
	Matrix4x4 rotateMatrix = MakeRotateMatrix(this->rotate);

	this->orientations[0].x = rotateMatrix.m[0][0];
	this->orientations[0].y = rotateMatrix.m[0][1];
	this->orientations[0].z = rotateMatrix.m[0][2];

	this->orientations[1].x = rotateMatrix.m[1][0];
	this->orientations[1].y = rotateMatrix.m[1][1];
	this->orientations[1].z = rotateMatrix.m[1][2];

	this->orientations[2].x = rotateMatrix.m[2][0];
	this->orientations[2].y = rotateMatrix.m[2][1];
	this->orientations[2].z = rotateMatrix.m[2][2];

	this->orientations[0] = Normalize(this->orientations[0]);
	this->orientations[1] = Normalize(this->orientations[1]);
	this->orientations[2] = Normalize(this->orientations[2]);
}

void OBB::CaluculateVertices(Vector3* vertices) const {
	Vector3 rotateAxis[3];
	rotateAxis[0] = this->orientations[0] * this->size.x;
	rotateAxis[1] = this->orientations[1] * this->size.y;
	rotateAxis[2] = this->orientations[2] * this->size.z;

	vertices[0] = this->center + rotateAxis[0] + rotateAxis[1] + rotateAxis[2];
	vertices[1] = this->center + rotateAxis[0] + rotateAxis[1] - rotateAxis[2];
	vertices[2] = this->center + rotateAxis[0] - rotateAxis[1] + rotateAxis[2];
	vertices[3] = this->center + rotateAxis[0] - rotateAxis[1] - rotateAxis[2];
	vertices[4] = this->center - rotateAxis[0] + rotateAxis[1] + rotateAxis[2];
	vertices[5] = this->center - rotateAxis[0] + rotateAxis[1] - rotateAxis[2];
	vertices[6] = this->center - rotateAxis[0] - rotateAxis[1] + rotateAxis[2];
	vertices[7] = this->center - rotateAxis[0] - rotateAxis[1] - rotateAxis[2];
}
