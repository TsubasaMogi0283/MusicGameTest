#pragma once
/**
 * @file QuaternionCalculation.h
 * @brief クォータニオンの計算
 * @author 茂木翼
 */

#include "Quaternion.h"
#include "Matrix4x4.h"
#include "Vector3.h"

/// <summary>
/// クォータニオンの計算
/// </summary>
namespace QuaternionCalculation{
	//計算とそれぞれの要素について
	// 
	//q=w+xi+yj+zk
	//i^2=j^2=k^2=ijk=-1
	//ij=k,jk=i,ki=j,
	//ji=-k,kj=-i,ik=-j

	//q=(qw,qv)
	//qw=w
	//qv=(x,y,z)




	/// <summary>
	/// Quaternionの積
	/// </summary>
	/// <param name="lhs"></param>
	/// <param name="rhs"></param>
	/// <returns></returns>
	Quaternion QuaternionMultiply(const Quaternion& lhs, const Quaternion& rhs);

	/// <summary>
	/// 単位のQuaternionを返す
	/// </summary>
	/// <returns></returns>
	Quaternion IdentityQuaternion();

	/// <summary>
	/// 共役Quaternionを返す
	/// </summary>
	/// <param name="quaternion"></param>
	/// <returns></returns>
	Quaternion Conjugate(const Quaternion& quaternion);

	/// <summary>
	/// Quaternionのnormを返す
	/// </summary>
	/// <param name="quaternion"></param>
	/// <returns></returns>
	float Norm(const Quaternion& quaternion);

	/// <summary>
	/// 正規化したQuaternionを返す
	/// </summary>
	/// <param name="quaternion"></param>
	/// <returns></returns>
	Quaternion Normalize(const Quaternion& quaternion);

	/// <summary>
	/// 逆Quaternionを返す
	/// </summary>
	/// <param name="quaternion"></param>
	/// <returns></returns>
	Quaternion Inverse(const Quaternion& quaternion);

	/// <summary>
	/// 任意軸回転を表すQuaternionの生成
	/// </summary>
	/// <param name="axis"></param>
	/// <param name="angle"></param>
	/// <returns></returns>
	Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle);

	/// <summary>
	/// ベクトルをQuaternionで回転させた結果のベクトルを求める
	/// </summary>
	/// <param name="vector"></param>
	/// <param name="quaternion"></param>
	/// <returns></returns>
	Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

	/// <summary>
	/// Quaternion版の回転行列を求める
	/// </summary>
	/// <param name="quaternion"></param>
	/// <returns></returns>
	Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);

	/// <summary>
	/// 回転の補間
	/// </summary>
	/// <param name="q0"></param>
	/// <param name="q1"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	Quaternion QuaternionSlerp(const Quaternion& q0,const Quaternion& q1,const float& t);



};

