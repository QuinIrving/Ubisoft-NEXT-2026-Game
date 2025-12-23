#pragma once
#include "Vec3.h"

template <typename T>
class Vec4;

template <typename T>
class Mat4 {
public:
	Vec4<T>& operator[](int row);
	const Vec4<T>& operator[](int row) const;

public:
	constexpr Mat4() : data(Mat4<T>::GetIdentity()) {};
	constexpr Mat4(T matrix[16]);
	constexpr Mat4(std::array<T, 16> matrix);
	constexpr Mat4(std::array<Vec4<T>, 4> matrix) : data(matrix) {};

	static constexpr Mat4<T> GetIdentity();
	static constexpr Mat4<T> GetZero();

	T& operator()(int row, int col) { return data[row][col]; }

	// matrix addition and multiplication
	Mat4<T> operator+(const Mat4<T>& rhs) const;
	Mat4<T>& operator+=(const Mat4<T>& rhs);
	Mat4<T> operator*(const Mat4<T>& rhs) const;
	Mat4<T>& operator*=(const Mat4<T>& rhs);

	// Scalar multiply
	Mat4<T> operator*(T factor) const;
	Mat4<T>& operator*=(T factor);

	Mat4<T> GetTranspose() const;
	Mat4<T> GetNormalMatrix() const; // only requires getting the Mat3 top left corner inverse, as the translations don't affect normals
	std::array<Vec4<T>, 4> GetData() const;

	static Mat4<T> Translate(Vec4<T> v);
	static Mat4<T> Rotate(Vec4<T> v);
	static Mat4<T> Scale(Vec4<T> v);

private:
	std::array<Vec4<T>, 4> data;
};

template <typename T>
Vec4<T>& Mat4<T>::operator[](int row) {
	if (row < 0 || row >= 4) {
		throw std::out_of_range("Attempted to access a Mat4 row outside of range");
	}

	return data[row];
}

template <typename T>
const Vec4<T>& Mat4<T>::operator[](int row) const {
	if (row < 0 || row >= 4) {
		throw std::out_of_range("Attempted to access a Mat4 row outside of range");
	}

	return data[row];
}

template <typename T>
constexpr Mat4<T>::Mat4(T matrix[16]) {
	for (int i = 0; i < 16; i += 4) {
		data[(int)(i / 4)] = Vec4<T>(matrix[i], matrix[i + 1], matrix[i + 2], matrix[i + 3]);
	}
}

template <typename T>
constexpr Mat4<T>::Mat4(std::array<T, 16> matrix) {
	for (int i = 0; i < 16; i += 4) {
		data[(int)(i / 4)] = Vec4<T>(matrix[i], matrix[i + 1], matrix[i + 2], matrix[i + 3]);
	}
}

template <typename T>
static constexpr Mat4<T>::Mat4<T> GetIdentity() {
	std::array<Vec4<T>, 4> arr;

	arr[0] = Vec4<T>(T(1), T(0), T(0), T(0));
	arr[1] = Vec4<T>(T(0), T(1), T(0), T(0));
	arr[2] = Vec4<T>(T(0), T(0), T(1), T(0));
	arr[3] = Vec4<T>(T(0), T(0), T(0), T(1));

	return Mat4<T>(arr);
}

template <typename T>
static constexpr Mat4<T>::Mat4<T> GetZero() {
	std::array<Vec4<T>, 4> arr;

	arr[0] = Vec4<T>(T(0), T(0), T(0), T(0));
	arr[1] = Vec4<T>(T(0), T(0), T(0), T(0));
	arr[2] = Vec4<T>(T(0), T(0), T(0), T(0));
	arr[3] = Vec4<T>(T(0), T(0), T(0), T(0));

	return Mat4<T>(arr);
}

template <typename T>
Mat4<T> Mat4<T>::operator+(const Mat4<T>& rhs) const {
	T addedData[16];
	for (int i = 0; i < 16; ++i) {
		addedData[i] = data[i] + rhs.data[i];
	}

	return Mat4(addedData);
}

template <typename T>
Mat4<T>& Mat4<T>::operator+=(const Mat4<T>& rhs) {
	for (int i = 0; i < 16; ++i) {
		data[i] += rhs.data[i];
	}

	return *this;
}

template <typename T>
Mat4<T> Mat4<T>::operator*(const Mat4<T>& rhs) const {

}

template <typename T>
Mat4<T>& Mat4<T>::operator*=(const Mat4<T>& rhs) {
	return *this;
}

template <typename T>
Mat4<T> Mat4<T>::operator*(T factor) const {
	T multipliedData[16];
	for (int i = 0; i < 16; ++i) {
		multipliedData[i] = data[i] * factor;
	}

	return Mat4(multipliedData);
}

template <typename T>
Mat4<T>& Mat4<T>::operator*=(T factor) {
	for (int i = 0; i < 16; ++i) {
		data[i] *= factor;
	}

	return *this;
}

template <typename T>
Mat4<T> Mat4<T>::GetTranspose() const {
	Mat4<T> m;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			m[i][j] = (*this)[j][i];
		}
	}

	return m;
}

template <typename T>
Mat4<T> Mat4<T>::GetNormalMatrix() const { // only requires getting the Mat3 top left corner inverse, as the translations don't affect normals
	// only the upper 3x3 matters as for a normal translation doesn't come into play, only the rotation and scaling.

	T a = (*this)[0][0], b = (*this)[0][1], c = (*this)[0][2];
	T d = (*this)[1][0], e = (*this)[1][1], f = (*this)[1][2];
	T g = (*this)[2][0], h = (*this)[2][1], i = (*this)[2][2];

	// Determinant:
	T det = a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
	T invDet = T(1) / det;

	// Cofactor matrix (inverse)
	Mat4<T> inv;
	inv[0][0] = (e * i - f * h) * invDet;
	inv[0][1] = -(b * i - c * h) * invDet;
	inv[0][2] = (b * f - c * e) * invDet;

	inv[1][0] = -(d * i - f * g) * invDet;
	inv[1][1] = (a * i - c * g) * invDet;
	inv[1][2] = -(a * f - c * d) * invDet;

	inv[2][0] = (d * h - e * g) * invDet;
	inv[2][1] = -(a * h - b * g) * invDet;
	inv[2][2] = (a * e - b * d) * invDet;

	// Transpose, only the 3x3 not the translation parts.
	Mat4<T> normal;
	for (int r = 0; r < 3; r++) {
		for (int c = 0; c < 3; c++) {
			normal[r][c] = inv[c][r];
		}
	}

	// not sure if we even need this but will keep the factor there for consistency.
	normal[3][3] = 1;

	return normal;
}

template <typename T>
std::array<Vec4<T>, 4> Mat4<T>::GetData() const {
	return data;
}

//static Mat4<T> Translate(Vec4<T> v);
//static Mat4<T> Rotate(Vec4<T> v);
//static Mat4<T> Scale(Vec4<T> v);