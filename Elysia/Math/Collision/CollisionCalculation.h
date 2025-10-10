#pragma once
#include "AABB.h"
#include "Fan.h"
#include "Plane.h"
#include "Vector2.h"
#include "Vector3.h"

/// <summary>
/// 衝突判定の計算
/// </summary>
namespace CollisionCalculation {

	/// <summary>
	/// AABB同士の判定
	/// </summary>
	/// <param name="aabb1">AABB1</param>
	/// <param name="aabb2">AABB2</param>
	/// <returns></returns>
	bool IsCollisionAABBPair(const AABB& aabb1, const AABB& aabb2);


	/// <summary>
	/// AABBとPointの判定
	/// </summary>
	/// <param name="aabb">AABB</param>
	/// <param name="point">点</param>
	/// <returns></returns>
	bool IsCollisionAABBAndPoint(const AABB& aabb, const Vector3& point);

	/// <summary>
	/// 平面と点の判定
	/// </summary>
	/// <param name="position">点</param>
	/// <param name="plane">平面座標</param>
	/// <returns></returns>
	bool IsCollisionPlaneAndPoint(const Vector3& position, const Plane& plane);

	/// <summary>
	/// 扇形(2次元)
	/// </summary>
	/// <param name="fan">扇</param>
	/// <param name="point">点</param>
	/// <returns></returns>
	bool IsFanCollision(const Fan2D& fan, const Vector2& point);

	/// <summary>
	/// 扇形(3次元)
	/// </summary>
	/// <param name="fan">扇</param>
	/// <param name="point">点</param>
	/// <returns></returns>
	bool IsFanCollision(const Fan3D& fan, const Vector3& point);

}


