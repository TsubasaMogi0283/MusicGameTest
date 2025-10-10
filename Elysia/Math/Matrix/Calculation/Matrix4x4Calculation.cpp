#include "Matrix4x4Calculation.h"
#include <VectorCalculation.h>
#include <SingleCalculation.h>


Matrix4x4 Matrix4x4Calculation::MakeIdentity4x4(){
	Matrix4x4 result = {
		result.m[0][0] = 1.0f,
		result.m[0][1] = 0.0f,
		result.m[0][2] = 0.0f,
		result.m[0][3] = 0.0f,

		result.m[1][0] = 0.0f,
		result.m[1][1] = 1.0f,
		result.m[1][2] = 0.0f,
		result.m[1][3] = 0.0f,

		result.m[2][0] = 0.0f,
		result.m[2][1] = 0.0f,
		result.m[2][2] = 1.0f,
		result.m[2][3] = 0.0f,

		result.m[3][0] = 0.0f,
		result.m[3][1] = 0.0f,
		result.m[3][2] = 0.0f,
		result.m[3][3] = 1.0f,
	};

	return result;
}

Matrix4x4 Matrix4x4Calculation::Multiply(const Matrix4x4& m1, const Matrix4x4& m2){
	Matrix4x4 result = {
		result.m[0][0] = (m1.m[0][0] * m2.m[0][0]) + (m1.m[0][1] * m2.m[1][0]) + (m1.m[0][2] * m2.m[2][0]) + (m1.m[0][3] * m2.m[3][0]),
		result.m[0][1] = (m1.m[0][0] * m2.m[0][1]) + (m1.m[0][1] * m2.m[1][1]) + (m1.m[0][2] * m2.m[2][1]) + (m1.m[0][3] * m2.m[3][1]),
		result.m[0][2] = (m1.m[0][0] * m2.m[0][2]) + (m1.m[0][1] * m2.m[1][2]) + (m1.m[0][2] * m2.m[2][2]) + (m1.m[0][3] * m2.m[3][2]),
		result.m[0][3] = (m1.m[0][0] * m2.m[0][3]) + (m1.m[0][1] * m2.m[1][3]) + (m1.m[0][2] * m2.m[2][3]) + (m1.m[0][3] * m2.m[3][3]),

		result.m[1][0] = (m1.m[1][0] * m2.m[0][0]) + (m1.m[1][1] * m2.m[1][0]) + (m1.m[1][2] * m2.m[2][0]) + (m1.m[1][3] * m2.m[3][0]),
		result.m[1][1] = (m1.m[1][0] * m2.m[0][1]) + (m1.m[1][1] * m2.m[1][1]) + (m1.m[1][2] * m2.m[2][1]) + (m1.m[1][3] * m2.m[3][1]),
		result.m[1][2] = (m1.m[1][0] * m2.m[0][2]) + (m1.m[1][1] * m2.m[1][2]) + (m1.m[1][2] * m2.m[2][2]) + (m1.m[1][3] * m2.m[3][2]),
		result.m[1][3] = (m1.m[1][0] * m2.m[0][3]) + (m1.m[1][1] * m2.m[1][3]) + (m1.m[1][2] * m2.m[2][3]) + (m1.m[1][3] * m2.m[3][3]),

		result.m[2][0] = (m1.m[2][0] * m2.m[0][0]) + (m1.m[2][1] * m2.m[1][0]) + (m1.m[2][2] * m2.m[2][0]) + (m1.m[2][3] * m2.m[3][0]),
		result.m[2][1] = (m1.m[2][0] * m2.m[0][1]) + (m1.m[2][1] * m2.m[1][1]) + (m1.m[2][2] * m2.m[2][1]) + (m1.m[2][3] * m2.m[3][1]),
		result.m[2][2] = (m1.m[2][0] * m2.m[0][2]) + (m1.m[2][1] * m2.m[1][2]) + (m1.m[2][2] * m2.m[2][2]) + (m1.m[2][3] * m2.m[3][2]),
		result.m[2][3] = (m1.m[2][0] * m2.m[0][3]) + (m1.m[2][1] * m2.m[1][3]) + (m1.m[2][2] * m2.m[2][3]) + (m1.m[2][3] * m2.m[3][3]),

		result.m[3][0] = (m1.m[3][0] * m2.m[0][0]) + (m1.m[3][1] * m2.m[1][0]) + (m1.m[3][2] * m2.m[2][0]) + (m1.m[3][3] * m2.m[3][0]),
		result.m[3][1] = (m1.m[3][0] * m2.m[0][1]) + (m1.m[3][1] * m2.m[1][1]) + (m1.m[3][2] * m2.m[2][1]) + (m1.m[3][3] * m2.m[3][1]),
		result.m[3][2] = (m1.m[3][0] * m2.m[0][2]) + (m1.m[3][1] * m2.m[1][2]) + (m1.m[3][2] * m2.m[2][2]) + (m1.m[3][3] * m2.m[3][2]),
		result.m[3][3] = (m1.m[3][0] * m2.m[0][3]) + (m1.m[3][1] * m2.m[1][3]) + (m1.m[3][2] * m2.m[2][3]) + (m1.m[3][3] * m2.m[3][3]),
	};

	return result;

}

Matrix4x4 Matrix4x4Calculation::MakeScaleMatrix(const Vector3& scale){
	Matrix4x4 result = {
		result.m[0][0] = scale.x,
		result.m[0][1] = 0.0f,
		result.m[0][2] = 0.0f,
		result.m[0][3] = 0.0f,

		result.m[1][0] = 0.0f,
		result.m[1][1] = scale.y,
		result.m[1][2] = 0.0f,
		result.m[1][3] = 0.0f,

		result.m[2][0] = 0.0f,
		result.m[2][1] = 0.0f,
		result.m[2][2] = scale.z,
		result.m[2][3] = 0.0f,

		result.m[3][0] = 0.0f,
		result.m[3][1] = 0.0f,
		result.m[3][2] = 0.0f,
		result.m[3][3] = 1.0f,

	};
	
	return result;
}

Matrix4x4 Matrix4x4Calculation::MakeRotateXMatrix(const float_t& radian){
	Matrix4x4 result = {

		result.m[0][0] = 1.0f,
		result.m[0][1] = 0.0f,
		result.m[0][2] = 0.0f,
		result.m[0][3] = 0.0f,

		result.m[1][0] = 0.0f,
		result.m[1][1] = std::cos(radian),
		result.m[1][2] = std::sin(radian),
		result.m[1][3] = 0.0f,

		result.m[2][0] = 0.0f,
		result.m[2][1] = -(std::sin(radian)),
		result.m[2][2] = std::cos(radian),
		result.m[2][3] = 0.0f,

		result.m[3][0] = 0.0f,
		result.m[3][1] = 0.0f,
		result.m[3][2] = 0.0f,
		result.m[3][3] = 1.0f,
	};

	return result;
	
}

Matrix4x4 Matrix4x4Calculation::MakeRotateYMatrix(const float_t& radian){
	Matrix4x4 result = {
		result.m[0][0] = std::cos(radian),
		result.m[0][1] = 0.0f,
		result.m[0][2] = -(std::sin(radian)),
		result.m[0][3] = 0.0f,

		result.m[1][0] = 0.0f,
		result.m[1][1] = 1.0f,
		result.m[1][2] = 0.0f,
		result.m[1][3] = 0.0f,

		result.m[2][0] = std::sin(radian),
		result.m[2][1] = 0.0f,
		result.m[2][2] = std::cos(radian),
		result.m[2][3] = 0.0f,

		result.m[3][0] = 0.0f,
		result.m[3][1] = 0.0f,
		result.m[3][2] = 0.0f,
		result.m[3][3] = 1.0f,

	};

	
	return result;
}

Matrix4x4 Matrix4x4Calculation::MakeRotateZMatrix(const float_t& radian){
	Matrix4x4 result = {
		result.m[0][0] = std::cos(radian),
		result.m[0][1] = std::sin(radian),
		result.m[0][2] = 0.0f,
		result.m[0][3] = 0.0f,

		result.m[1][0] = -(std::sin(radian)),
		result.m[1][1] = std::cos(radian),
		result.m[1][2] = 0.0f,
		result.m[1][3] = 0.0f,

		result.m[2][0] = 0.0f,
		result.m[2][1] = 0.0f,
		result.m[2][2] = 1.0f,
		result.m[2][3] = 0.0f,

		result.m[3][0] = 0.0f,
		result.m[3][1] = 0.0f,
		result.m[3][2] = 0.0f,
		result.m[3][3] = 1.0f,

	};

	
	return result;
}

Matrix4x4 Matrix4x4Calculation::MakeRotateXYZMatrix(const float_t& radianX, const float_t& radianY, const float_t& radianZ){
	
	//XYZそれぞれの行列を作る
	Matrix4x4 rotateMatrixX = MakeRotateXMatrix(radianX);
	Matrix4x4 rotateMatrixY = MakeRotateYMatrix(radianY);
	Matrix4x4 rotateMatrixZ = MakeRotateZMatrix(radianZ);

	//まとめる
	Matrix4x4 result = Multiply(rotateMatrixX, Multiply(rotateMatrixY, rotateMatrixZ));

	return result;

}

Matrix4x4 Matrix4x4Calculation::MakeTranslateMatrix(const Vector3& translate){
	Matrix4x4 result = {};
	result.m[0][0] = 1.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = 1.0f;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = 1.0f;
	result.m[2][3] = 0.0f;


	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;


	return result;
}

Matrix4x4 Matrix4x4Calculation::MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate){
	
	//Scale
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);

	//Rotate
	Matrix4x4 rotateMatrix = MakeRotateXYZMatrix(rotate.x, rotate.y, rotate.z);

	//Translate
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	//Affine
	Matrix4x4 result = Multiply(scaleMatrix, Multiply(rotateMatrix, translateMatrix));

	return result;
}

Matrix4x4 Matrix4x4Calculation::Inverse(const Matrix4x4& m){
	float MatrixFormula =
		+(m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3])
		+ (m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1])
		+ (m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2])

		- (m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1])
		- (m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3])
		- (m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2])

		- (m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3])
		- (m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1])
		- (m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2])

		+ (m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1])
		+ (m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3])
		+ (m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2])

		+ (m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3])
		+ (m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1])
		+ (m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2])


		- (m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1])
		- (m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3])
		- (m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2])

		- (m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0])
		- (m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0])
		- (m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0])

		+ (m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0])
		+ (m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0])
		+ (m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0]);

	Matrix4x4 result = {
		result.m[0][0] = (1 / MatrixFormula) * (
			+(m.m[1][1] * m.m[2][2] * m.m[3][3])
			+ (m.m[1][2] * m.m[2][3] * m.m[3][1])
			+ (m.m[1][3] * m.m[2][1] * m.m[3][2])
			- (m.m[1][3] * m.m[2][2] * m.m[3][1])
			- (m.m[1][2] * m.m[2][1] * m.m[3][3])
			- (m.m[1][1] * m.m[2][3] * m.m[3][2])),

		result.m[0][1] = (1 / MatrixFormula) * (
			-(m.m[0][1] * m.m[2][2] * m.m[3][3])
			- (m.m[0][2] * m.m[2][3] * m.m[3][1])
			- (m.m[0][3] * m.m[2][1] * m.m[3][2])
			+ (m.m[0][3] * m.m[2][2] * m.m[3][1])
			+ (m.m[0][2] * m.m[2][1] * m.m[3][3])
			+ (m.m[0][1] * m.m[2][3] * m.m[3][2])),


		result.m[0][2] = (1 / MatrixFormula) * (
			+(m.m[0][1] * m.m[1][2] * m.m[3][3])
			+ (m.m[0][2] * m.m[1][3] * m.m[3][1])
			+ (m.m[0][3] * m.m[1][1] * m.m[3][2])
			- (m.m[0][3] * m.m[1][2] * m.m[3][1])
			- (m.m[0][2] * m.m[1][1] * m.m[3][3])
			- (m.m[0][1] * m.m[1][3] * m.m[3][2])),

		result.m[0][3] = (1 / MatrixFormula) * (
			-(m.m[0][1] * m.m[1][2] * m.m[2][3])
			- (m.m[0][2] * m.m[1][3] * m.m[2][1])
			- (m.m[0][3] * m.m[1][1] * m.m[2][2])
			+ (m.m[0][3] * m.m[1][2] * m.m[2][1])
			+ (m.m[0][2] * m.m[1][1] * m.m[2][3])
			+ (m.m[0][1] * m.m[1][3] * m.m[2][2])),



		result.m[1][0] = (1 / MatrixFormula) * (
			-(m.m[1][0] * m.m[2][2] * m.m[3][3])
			- (m.m[1][2] * m.m[2][3] * m.m[3][0])
			- (m.m[1][3] * m.m[2][0] * m.m[3][2])
			+ (m.m[1][3] * m.m[2][2] * m.m[3][0])
			+ (m.m[1][2] * m.m[2][0] * m.m[3][3])
			+ (m.m[1][0] * m.m[2][3] * m.m[3][2])),

		result.m[1][1] = (1 / MatrixFormula) * (
			+(m.m[0][0] * m.m[2][2] * m.m[3][3])
			+ (m.m[0][2] * m.m[2][3] * m.m[3][0])
			+ (m.m[0][3] * m.m[2][0] * m.m[3][2])
			- (m.m[0][3] * m.m[2][2] * m.m[3][0])
			- (m.m[0][2] * m.m[2][0] * m.m[3][3])
			- (m.m[0][0] * m.m[2][3] * m.m[3][2])),

		result.m[1][2] = (1 / MatrixFormula) * (
			-(m.m[0][0] * m.m[1][2] * m.m[3][3])
			- (m.m[0][2] * m.m[1][3] * m.m[3][0])
			- (m.m[0][3] * m.m[1][0] * m.m[3][2])
			+ (m.m[0][3] * m.m[1][2] * m.m[3][0])
			+ (m.m[0][2] * m.m[1][0] * m.m[3][3])
			+ (m.m[0][0] * m.m[1][3] * m.m[3][2])),

		result.m[1][3] = (1 / MatrixFormula) * (
			+(m.m[0][0] * m.m[1][2] * m.m[2][3])
			+ (m.m[0][2] * m.m[1][3] * m.m[2][0])
			+ (m.m[0][3] * m.m[1][0] * m.m[2][2])
			- (m.m[0][3] * m.m[1][2] * m.m[2][0])
			- (m.m[0][2] * m.m[1][0] * m.m[2][3])
			- (m.m[0][0] * m.m[1][3] * m.m[2][2])),



		result.m[2][0] = (1 / MatrixFormula) * (
			+(m.m[1][0] * m.m[2][1] * m.m[3][3])
			+ (m.m[1][1] * m.m[2][3] * m.m[3][0])
			+ (m.m[1][3] * m.m[2][0] * m.m[3][1])
			- (m.m[1][3] * m.m[2][1] * m.m[3][0])
			- (m.m[1][1] * m.m[2][0] * m.m[3][3])
			- (m.m[1][0] * m.m[2][3] * m.m[3][1])),

		result.m[2][1] = (1 / MatrixFormula) * (
			-(m.m[0][0] * m.m[2][1] * m.m[3][3])
			- (m.m[0][1] * m.m[2][3] * m.m[3][0])
			- (m.m[0][3] * m.m[2][0] * m.m[3][1])
			+ (m.m[0][3] * m.m[2][1] * m.m[3][0])
			+ (m.m[0][1] * m.m[2][0] * m.m[3][3])
			+ (m.m[0][0] * m.m[2][3] * m.m[3][1])),

		result.m[2][2] = (1 / MatrixFormula) * (
			+(m.m[0][0] * m.m[1][1] * m.m[3][3])
			+ (m.m[0][1] * m.m[1][3] * m.m[3][0])
			+ (m.m[0][3] * m.m[1][0] * m.m[3][1])
			- (m.m[0][3] * m.m[1][1] * m.m[3][0])
			- (m.m[0][1] * m.m[1][0] * m.m[3][3])
			- (m.m[0][0] * m.m[1][3] * m.m[3][1])),

		result.m[2][3] = (1 / MatrixFormula) * (
			-(m.m[0][0] * m.m[1][1] * m.m[2][3])
			- (m.m[0][1] * m.m[1][3] * m.m[2][0])
			- (m.m[0][3] * m.m[1][0] * m.m[2][1])
			+ (m.m[0][3] * m.m[1][1] * m.m[2][0])
			+ (m.m[0][1] * m.m[1][0] * m.m[2][3])
			+ (m.m[0][0] * m.m[1][3] * m.m[2][1])),


		result.m[3][0] = (1 / MatrixFormula) * (
			-(m.m[1][0] * m.m[2][1] * m.m[3][2])
			- (m.m[1][1] * m.m[2][2] * m.m[3][0])
			- (m.m[1][2] * m.m[2][0] * m.m[3][1])
			+ (m.m[1][2] * m.m[2][1] * m.m[3][0])
			+ (m.m[1][1] * m.m[2][0] * m.m[3][2])
			+ (m.m[1][0] * m.m[2][2] * m.m[3][1])),


		result.m[3][1] = (1 / MatrixFormula) * (
			+(m.m[0][0] * m.m[2][1] * m.m[3][2])
			+ (m.m[0][1] * m.m[2][2] * m.m[3][0])
			+ (m.m[0][2] * m.m[2][0] * m.m[3][1])
			- (m.m[0][2] * m.m[2][1] * m.m[3][0])
			- (m.m[0][1] * m.m[2][0] * m.m[3][2])
			- (m.m[0][0] * m.m[2][2] * m.m[3][1])),

		result.m[3][2] = (1 / MatrixFormula) * (
			-(m.m[0][0] * m.m[1][1] * m.m[3][2])
			- (m.m[0][1] * m.m[1][2] * m.m[3][0])
			- (m.m[0][2] * m.m[1][0] * m.m[3][1])
			+ (m.m[0][2] * m.m[1][1] * m.m[3][0])
			+ (m.m[0][1] * m.m[1][0] * m.m[3][2])
			+ (m.m[0][0] * m.m[1][2] * m.m[3][1])),

		result.m[3][3] = (1 / MatrixFormula) * (
			+(m.m[0][0] * m.m[1][1] * m.m[2][2])
			+ (m.m[0][1] * m.m[1][2] * m.m[2][0])
			+ (m.m[0][2] * m.m[1][0] * m.m[2][1])
			- (m.m[0][2] * m.m[1][1] * m.m[2][0])
			- (m.m[0][1] * m.m[1][0] * m.m[2][2])
			- (m.m[0][0] * m.m[1][2] * m.m[2][1])),
	};

	return result;
}

Matrix4x4 Matrix4x4Calculation::MakePerspectiveFovMatrix(const float_t& fovY, const float_t& aspectRatio, const float_t& nearClip, const float_t& farClip){
	float theta = fovY / 2.0f;

	Matrix4x4 result = {
		result.m[0][0] = (1.0f / aspectRatio) * SingleCalculation::Cot(theta),
		result.m[0][1] = 0.0f,
		result.m[0][2] = 0.0f,
		result.m[0][3] = 0.0f,

		result.m[1][0] = 0.0f,
		result.m[1][1] = SingleCalculation::Cot(theta),
		result.m[1][2] = 0.0f,
		result.m[1][3] = 0.0f,

		result.m[2][0] = 0.0f,
		result.m[2][1] = 0.0f,
		result.m[2][2] = farClip / (farClip - nearClip),
		result.m[2][3] = 1.0f,

		result.m[3][0] = 0.0f,
		result.m[3][1] = 0.0f,
		result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip),
		result.m[3][3] = 0.0f,
	};

	return result;
}

Matrix4x4 Matrix4x4Calculation::MakeOrthographicMatrix(const float_t& left, const float_t& top, const float_t& right, const float_t& bottom, const float_t& nearClip, const float_t& farClip){
	Matrix4x4 result = {
		result.m[0][0] = 2.0f / (right - left),
		result.m[0][1] = 0.0f,
		result.m[0][2] = 0.0f,
		result.m[0][3] = 0.0f,

		result.m[1][0] = 0.0f,
		result.m[1][1] = 2.0f / (top - bottom),
		result.m[1][2] = 0.0f,
		result.m[1][3] = 0.0f,

		result.m[2][0] = 0.0f,
		result.m[2][1] = 0.0f,
		result.m[2][2] = (1.0f / farClip - nearClip),
		result.m[2][3] = 0.0f,

		result.m[3][0] = (left + right) / (left - right),
		result.m[3][1] = (top + bottom) / (bottom - top),
		result.m[3][2] = nearClip / (nearClip - farClip),
		result.m[3][3] = 1.0f,
	};
	return result;
}

Matrix4x4 Matrix4x4Calculation::MakeTransposeMatrix(const Matrix4x4& m){
	Matrix4x4 result = {

		result.m[0][0] = m.m[0][0],
		result.m[0][1] = m.m[1][0],
		result.m[0][2] = m.m[2][0],
		result.m[0][3] = m.m[3][0],

		result.m[1][0] = m.m[0][1],
		result.m[1][1] = m.m[1][1],
		result.m[1][2] = m.m[2][1],
		result.m[1][3] = m.m[3][1],

		result.m[2][0] = m.m[0][2],
		result.m[2][1] = m.m[1][2],
		result.m[2][2] = m.m[2][2],
		result.m[2][3] = m.m[3][2],

		result.m[3][0] = m.m[0][3],
		result.m[3][1] = m.m[1][3],
		result.m[3][2] = m.m[2][3],
		result.m[3][3] = m.m[3][3],

	};
	return result;
}



