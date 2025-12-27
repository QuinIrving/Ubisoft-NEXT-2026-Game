#pragma once
#include "MathConstants.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Mat4.h"
#include "Quaternion.h"

template <typename T>
Vec4<T> Vec4<T>::operator*(const Mat4<T>& rhs) const {
	Mat4<T> rhsT = rhs.GetTranspose();

	return Vec4<T>(this->DotProduct(rhsT[0]), this->DotProduct(rhsT[1]), this->DotProduct(rhsT[2]), this->DotProduct(rhsT[3]));
}

template <typename T>
Vec4<T>& Vec4<T>::operator*=(const Mat4<T>& rhs) {
	Mat4<T> rhsT = rhs.GetTranspose();

	T newX = this->DotProduct(rhsT[0]);
	T newY = this->DotProduct(rhsT[1]);
	T newZ = this->DotProduct(rhsT[2]);
	T newW = this->DotProduct(rhsT[3]);

	x = newX;
	y = newY;
	z = newZ;
	w = newW;

	return *this;
}

template <typename T>
Mat4<T> Mat4<T>::operator*(const Mat4<T>& rhs) const {
	Mat4<T> rhsT = rhs.GetTranspose();

	Mat4<T> result;

	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			result[r][c] = Vec4<T>::DotProduct(this->m_data[r], rhsT[c]);
		}
	}

	return result;
}

template <typename T>
Mat4<T>& Mat4<T>::operator*=(const Mat4<T>& rhs) {
	Mat4<T> rhsT = rhs.GetTranspose();

	Mat4<T> result;

	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			result[r][c] = Vec4<T>::DotProduct(this->m_data[r], rhsT[c]);
		}
	}

	this->m_data = result.m_data;
	return *this;
}

template <typename T>
inline Mat4<T> Mat4<T>::Translate(Vec4<T> v) {
	Mat4<T> m = Mat4<T>();
	m[3][0] = v.x;
	m[3][1] = v.y;
	m[3][2] = v.z;

	return m;
}

template <typename T>
inline Mat4<T> Mat4<T>::Rotate(Vec4<T> v) {
	return Quaternion(v.x, v.y, v.z).GetRotationMatrix();
}

template <typename T>
inline Mat4<T> Mat4<T>::Scale(Vec4<T> v) {
	Mat4<T> m = Mat4<T>();
	m[0][0] = v.x;
	m[1][1] = v.y;
	m[2][2] = v.z;

	return m;
}


template <typename T>
inline Vec3<T> Vec3<T>::operator*(const Quaternion& q) const {
	Quaternion qNormalized = Quaternion(q);
	qNormalized.Normalize();
	Vec4<T> v = *this;
	v.w = 0;
	Quaternion vQuat = Quaternion(v);
	Quaternion result = qNormalized * vQuat * (~qNormalized);
	const Vec4<T>& delta = result.GetDelta();
	return Vec3<T>(delta.x, delta.y, delta.z);
}

template <typename T>
inline Vec3<T>& Vec3<T>::operator*=(const Quaternion& q) {
	Quaternion qNormalized = Quaternion(q);
	qNormalized.Normalize();
	Vec4<T> v = *this;
	v.w = 0;
	Quaternion vQuat = Quaternion(v);
	Quaternion result = qNormalized * vQuat * (~qNormalized);
	Vec4<T> d = result.GetDelta();

	this->x = (T)d.x;
	this->y = (T)d.y;
	this->z = (T)d.z;

	return *this;
}

template <typename T>
inline Vec4<T> Vec4<T>::operator*(const Quaternion& q) const {
	Quaternion qNormalized = Quaternion(q);
	qNormalized.Normalize();
	Quaternion vQuat = Quaternion(Vec4<T>(this->x, this->y, this->z, (T)0));
	Quaternion result = qNormalized * vQuat * (~qNormalized);
	const Vec4<T>& delta = result.GetDelta();
	return Vec4<T>(delta.x, delta.y, delta.z, 1.f);
}

template <typename T>
inline Vec4<T>& Vec4<T>::operator*=(const Quaternion& q) {
	Quaternion qNormalized = Quaternion(q);
	qNormalized.Normalize();
	Quaternion vQuat = Quaternion(Vec4<T>(this->x, this->y, this->z, (T)0));

	Quaternion result = qNormalized * vQuat * (~qNormalized);
	Vec4<T> d = result.GetDelta();
	this->x = (T)d.x;
	this->y = (T)d.y;
	this->z = (T)d.z;
	this->w = (T)1.f;

	return *this;
}
