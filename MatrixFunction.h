#pragma once
#include <Matrix4x4.h>
#include <Vector3.h>

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

Matrix4x4 Add(const Matrix4x4& _m1, const Matrix4x4& _m2);
Matrix4x4 Subtract(const Matrix4x4& _m1, const Matrix4x4& _m2);
Matrix4x4 Multiply(const Matrix4x4& _m1, const Matrix4x4& _m2);
Matrix4x4 Inverse(const Matrix4x4& _m);
Matrix4x4 Transpose(const Matrix4x4& _m);
Matrix4x4 MakeIdentity4x4();
Matrix4x4 MakeScaleMatrix(const Vector3& _scale);
Matrix4x4 MakeTranslateMatrix(const Vector3& _translate);
Matrix4x4 MakeRotateXMatrix(float _radian);
Matrix4x4 MakeRotateYMatrix(float _radian);
Matrix4x4 MakeRotateZMatrix(float _radian);
Matrix4x4 MakeRotateMatrix(const Vector3& _rotate);
Matrix4x4 MakeAffineMatrix(const Vector3& _scale, const Vector3& _rotate, const Vector3& _translate);
Matrix4x4 MakePerspectiveFovMatrix(float _fovY, float _aspectRatio, float _nearClip, float _farClip);
Matrix4x4 MakeOrthographicMatrix(float _left, float _top, float _right, float _bottom, float _nearClip, float _farClip);
Matrix4x4 MakeViewportMatrix(float _left, float _top, float _width, float _height, float _minDepth, float _maxDepth);

Matrix4x4 operator+(const Matrix4x4& _m1, const Matrix4x4& _m2);
Matrix4x4 operator-(const Matrix4x4& _m1, const Matrix4x4& _m2);
Matrix4x4 operator*(const Matrix4x4& _m1, const Matrix4x4& _m2);
