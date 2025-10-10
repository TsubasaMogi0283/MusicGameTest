#pragma once
/**
 * @file VectorCalculation.h
 * @brief ベクトルの計算
 * @author 茂木翼
 */

#include <vector>

#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"


/// <summary>
/// ベクトルの計算
/// </summary>
namespace VectorCalculation {

	/// <summary>
	/// 加算(Vector2)
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <returns></returns>
	Vector2 Add(const Vector2& v1, const Vector2& v2);

	/// <summary>
	/// 加算(Vector3)
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <returns></returns>
	Vector3 Add(const Vector3& v1, const Vector3& v2);

	/// <summary>
	/// 減算(Vector2)
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <returns></returns>
	Vector2 Subtract(const Vector2& v1, const Vector2& v2);

	/// <summary>
	/// 減算(Vector3)
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <returns></returns>
	Vector3 Subtract(const Vector3& v1, const Vector3& v2);


	/// <summary>
	/// 乗算(Vector2)
	/// </summary>
	/// <param name="v1"></param>
	/// <param name="v2"></param>
	/// <returns></returns>
	Vector2 Multiply(const Vector2& v1, const Vector2& v2);

	/// <summary>
	/// 乗算(Vector3)
	/// </summary>
	/// <param name="v1"></param>
	/// <param name="v2"></param>
	/// <returns></returns>
	Vector3 Multiply(const Vector3& v1, const Vector3& v2);

	/// <summary>
	/// 乗算(Vector3*float)
	/// </summary>
	/// <param name="v1"></param>
	/// <param name="value"></param>
	/// <returns></returns>
	Vector3 Multiply(const Vector3& v1, const float_t& value);

	/// <summary>
	/// 除算(Vector3/float)
	/// </summary>
	/// <param name="v1"></param>
	/// <param name="value"></param>
	/// <returns></returns>
	Vector3 Divide(const Vector3& v1, const float_t& value);


	/// <summary>
	/// 正規化(Vector2)
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns></returns>
	Vector2 Normalize(const Vector2& v);

	/// <summary>
	/// 正規化(Vector3)
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns></returns>
	Vector3 Normalize(const Vector3& v);

	/// <summary>
	/// 線形補間(Vector2)
	/// </summary>
	/// <param name="v1"></param>
	/// <param name="v2"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	Vector2 Lerp(const Vector2& v1, const Vector2& v2, const float_t& t);

	/// <summary>
	/// 線形補間(Vector3)
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <param name="t">t</param>
	/// <returns></returns>
	Vector3 Lerp(const Vector3& v1, const Vector3& v2, const float_t& t);

	/// <summary>
	/// 球面線形補間
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <param name="t">t</param>
	/// <returns></returns>
	Vector3 Slerp(const Vector3& v1, const Vector3& v2, const float_t& t);

	/// <summary>
	/// 外積
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <returns></returns>
	Vector3 Cross(const Vector3& v1, const Vector3& v2);


	/// <summary>
	/// 正射影ベクトル
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <returns></returns>
	Vector3 Project(const Vector3& v1, const Vector3& v2);


	
	/// <summary>
	/// 回転
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <param name="m">行列</param>
	/// <returns></returns>
	Vector3 TransformCalculation(const Vector3& v, const Matrix4x4& m);


	/// <summary>
	/// Catmull-romスプライン曲線(3D)
	/// </summary>
	/// <param name="p0"></param>
	/// <param name="p1"></param>
	/// <param name="p2"></param>
	/// <param name="p3"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	Vector3 CatmullRom(const Vector3& p0,const Vector3& p1,const Vector3& p2,const Vector3& p3,const float_t& t);



}