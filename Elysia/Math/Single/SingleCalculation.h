#pragma once

/**
 * @file SingleCalculation.h
 * @brief 単体の計算
 * @author 茂木翼
 */
#include <cmath>

#pragma region 前方宣言

/// <summary>
/// ベクトル(2D)
/// </summary>
struct Vector2;

/// <summary>
/// ベクトル(3D)
/// </summary>
struct Vector3;

#pragma endregion

/// <summary>
/// 単体の計算
/// </summary>
namespace SingleCalculation{

	/// <summary>
	/// コタンジェント
	/// </summary>
	/// <param name="theta_">θ</param>
	/// <returns>値</returns>
	float Cot(const float_t& theta);

	/// <summary>
	/// 線形補間(float)
	/// </summary>
	/// <param name="start">始点</param>
	/// <param name="end">終点</param>
	/// <param name="t">値</param>
	/// <returns>始点から終点までの値</returns>
	float_t Lerp(const float_t& start, const float_t& end, const float_t& t);

	/// <summary>
	/// 割合を求める(線形補間の逆)
	/// </summary>
	/// <param name="start">始点</param>
	/// <param name="end">終点</param>
	/// <param name="value">値</param>
	/// <returns>割合</returns>
	float InverseLerp(const float_t& start, const float_t& end,const float_t& value);



	/// <summary>
	/// 長さを求める(Vector2)
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>長さ</returns>
	float_t Length(const Vector2& v);

	/// <summary>
	/// 長さを求める(Vector3)
	/// </summary>
	/// <param name="v">ベクトル</param>
	/// <returns>長さ</returns>
	float_t Length(const Vector3& v);

	/// <summary>
	/// 内積(Vector2)
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">ベクトル2</param>
	/// <returns>結果</returns>
	float_t Dot(const Vector2& v1, const Vector2& v2);


	/// <summary>
	/// 内積(Vector3)
	/// </summary>
	/// <param name="v1">ベクトル1</param>
	/// <param name="v2">べクトル2</param>
	/// <returns>内積の計算結果</returns>
	float_t Dot(const Vector3& v1, const Vector3& v2);

	


};

