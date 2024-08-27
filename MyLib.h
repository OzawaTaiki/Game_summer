#pragma once

#include "MatrixFunction.h"
#include "VectorFunction.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

struct Sphere {
	Vector3 center; // 中心点
	float radius;   // 半径
};
// 直線
struct Line {
	Vector3 origin; // 始点
	Vector3 diff;   // 終点への差分ベクトル
};
// 半直線
struct Ray {
	Vector3 origin; // 始点
	Vector3 diff;   // 終点への差分ベクトル
};
// 線分
struct Segment {
	Vector3 origin; // 始点
	Vector3 diff;   // 終点への差分ベクトル
};
// 平面
struct Plane {
	Vector3 normal;   // 法線
	float distance;   // 距離
	float scalar = 2; //
};

// 三角形
struct Triangle {
	Vector3 vertices[3]; // 頂点
};
// 矩形？
struct AABB {
	Vector3 min;
	Vector3 max;

public:
	void Update();
};

struct OBB {
	Vector3 center;          // 中心座標
	Vector3 orientations[3]; // 座標軸 正規化・直行必須
	Vector3 size;            // サイズ
	Vector3 rotate;          // 回転角

public:
	void CalculateOrientations();
	/// <summary>
	/// 頂点の計算
	/// </summary>
	/// <param name="vertices">計算結果を格納するVecter3配列</param>
	void CaluculateVertices(Vector3* vertices) const;
};

struct Bezier {
	Vector3 point[3];
};

// グリッドの描画
void DrawGrid(const Matrix4x4& _viewProjectionMatrix, const Matrix4x4& _viewportMatrix);
// 球体の描画
void DrawSphere(const Sphere& _sphere, const Matrix4x4& _viewProjectionMatrix, const Matrix4x4& _viewportMatrix, uint32_t _color);
// 直線の描画
void Drawline(const Vector3& _origin, const Vector3& _diff, const Matrix4x4& _viewProjectionMatrix, const Matrix4x4& _viewportMatrix, uint32_t _color);

void Drawline_se(const Vector3& _start, const Vector3& _end, const Matrix4x4& _viewProjectionMatrix, const Matrix4x4& _viewportMatrix, uint32_t _color);

void DrawPlane(const Plane& _plane, const Matrix4x4& _viewProjectionMatrix, const Matrix4x4& _viewportMatrix, uint32_t _color);

void DrawTriangle(const Triangle& _triangle, const Matrix4x4& _viewProjectionMatrix, const Matrix4x4& _viewportMatrix, uint32_t _color);

void DrawAABB(const AABB& _aabb, const Matrix4x4& _viewProjectionMatrix, const Matrix4x4& _viewportMatrix, uint32_t _color);

void DrawOBB(const OBB& _obb, const Matrix4x4& _viewProjectionMatrix, const Matrix4x4& _viewportMatrix, uint32_t _color);

void DrawBezier(const Bezier& _bezier, const Matrix4x4& _viewProjectionMatrix, const Matrix4x4& _viewportMatrix, uint32_t _color, bool _isDrawPoint);

void DrawCatmullRom(
    const Vector3& _cPoint0, const Vector3& _cPoint1, const Vector3& _cPoint2, const Vector3& _cPoint3, const Matrix4x4& _viewProjectionMatrix, const Matrix4x4& _viewportMatrix, uint32_t _color,
    bool _isDrawPoint);

float LerpShortAngle(float _a, float _b, float _t);

Vector3 Project(const Vector3& _v1, const Vector3& _v2);

Vector3 ClosestPoint(const Vector3& _point, const Segment& _segment);

// 法線に垂直なVectorを求める
Vector3 Perpendicular(const Vector3& _v);

/// <summary>
/// 平面の四点を求める
/// </summary>
/// <param name="_plane">４点を求める平面</param>
/// <param name="_vertex">計算結果を格納するvevtor3[4] 0-2-1-3</param>
void GetPlaneVertex(const Plane& _plane, Vector3* _vertex);

// 球体同士の衝突判定
bool IsCollision(const Sphere& _s1, const Sphere& _s2);

bool IsCollision(const Sphere& _s, const Plane& _p);

// 平面と線分の衝突判定
bool IsCollision(const Plane& _plane, const Segment& _segment);

// 三角形と線分の衝突判定
bool IsCollision(const Triangle& _triangle, const Segment& _segment);

// AABB同士の衝突判定
bool IsCollision(const AABB& _a, const AABB& _b);

// AABBと球の衝突判定
bool IsCollision(const AABB& _a, const Sphere& _s);

// AABBと線分の衝突判定
bool IsCollision(const AABB& _aabb, const Segment& _segment);

// obbと球の衝突判定
bool IsCollision(const OBB& _obb, const Sphere& _sphere);

// obbと線分の衝突判定
bool IsCollision(const OBB& _obb, const Segment& _segment);

// obbとobbの衝突判定
bool IsCollision(const OBB& _obb1, const OBB& _obb2);

// 三角形のある平面を計算
Plane CalculatePlane(const Triangle& _triangle);

/// <summary>
/// 射影ベクトルのminとmaxを返す
/// </summary>
/// <param name="_obb">OBB</param>
/// <param name="_axis">分離軸候補</param>
/// <returns>最小と最大</returns>
void CalculateProjectionRange(const OBB& _obb, const Vector3& _axis, float& _min, float& _max);

Vector3 CalculatePointBezier(const Bezier& _bezier, float _t);

Vector3 CalculatePointCatmullRom(const Vector3& _cPoint0, const Vector3& _cPoint1, const Vector3& _cPoint2, const Vector3& _cPoint3, float _t);
Vector3 CalculatePointCatmullRom(const std::vector<Vector3>& _points, float _t);