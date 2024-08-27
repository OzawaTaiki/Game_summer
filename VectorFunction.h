#pragma once
#include "Vector2.h"
#include <Matrix4x4.h>
#include <Vector3.h>

Vector3 Add(const Vector3& v1, const Vector3& v2);
Vector3 Subtract(const Vector3& v1, const Vector3& v2);
Vector3 Multiply(float scalar, const Vector3& v);
Vector3 Multiply(const Vector3& v1, const Vector3& v2);
float Dot(const Vector3& v1, const Vector3& v2);
Vector3 Cross(const Vector3& _v1, const Vector3& _v2);
float Length(const Vector3& v);
Vector3 Normalize(const Vector3& v);
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
Vector3 Lerp(const Vector3& _vector1, const Vector3& _vector2, float _t);

void Clamp(Vector3& _value, const Vector3& _min, const Vector3& _max);
Vector3 TransformNormal(const Vector3& _v, const Matrix4x4& _m);

float Length(const Vector2& _vec);

Vector3& operator+=(Vector3& _v1, const Vector3& _v2);
Vector3& operator-=(Vector3& _v1, const Vector3& _v2);
Vector3 operator+(const Vector3& _v1, const Vector3& _v2);
Vector3 operator-(const Vector3& _v1, const Vector3& _v2);
Vector3 operator*(const Vector3& _v1, const Vector3& _v2);
Vector3 operator/(const Vector3& _v1, const Vector3& _v2);
Vector3 operator*(const Vector3& _v, float _s);
Vector3 operator/(const Vector3& _v, float _s);
Vector3 operator*(float _s, const Vector3& _v);
Vector3 operator/(float _s, const Vector3& _v);
Vector3 operator-(const Vector3& _v);

Vector2& operator+=(Vector2& _v1, const Vector2& _v2);
Vector2 operator+(const Vector2& _vec1, const Vector2& _vec2);
Vector2 operator-(const Vector2& _vec1, const Vector2& _vec2);