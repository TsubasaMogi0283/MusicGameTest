#pragma once
#include <cstdint>
#include <string>

#include "Vector3.h"
#include "AABB.h"
#include "Fan.h"
#include <Plane.h>

/// <summary>
/// 衝突の種類
/// </summary>
enum ColliderType {
	//点
	PointType,
	//板
	PlaneType,
	//球
	SphereType,
	//AABB
	AABBType,
	//ボックス
	BoxType,
	//扇
	FanType,
	//円
	CircleType,
};

/// <summary>
/// 衝突
/// </summary>
class Collider{
public:
	/// <summary>
	/// 接触
	/// </summary>
	virtual void OnCollision()=0;

	/// <summary>
	/// 非接触
	/// </summary>
	virtual void OffCollision() = 0;

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns></returns>
	virtual Vector3 GetWorldPosition() = 0;

public:
	/// <summary>
	/// 名前を取得
	/// </summary>
	/// <returns></returns>
	inline std::string GetName()const {
		return name_;
	}

	/// <summary>
	/// 半径を取得
	/// </summary>
	/// <returns></returns>
	float_t GetRadius() const{
		return radius_;
	}

	/// <summary>
	/// AABBの取得
	/// </summary>
	/// <returns></returns>
	inline AABB GetAABB()const {
		return aabb_;
	}


	/// <summary>
	/// 3Dの扇の取得
	/// </summary>
	/// <returns></returns>
	inline Fan3D GetFan3D () const{ 
		return fan3D_;
	};

	/// <summary>
	/// 2Dの扇の取得
	/// </summary>
	/// <returns></returns>
	inline Fan2D GetFan2D() const{
		return fan2D_;
	};

	/// <summary>
	/// 平面の取得
	/// </summary>
	/// <returns></returns>
	inline Plane GetPlane()const {
		return plane_;
	}


	/// <summary>
	/// 衝突判定で使う形の種類の値を取得
	/// </summary>
	/// <returns></returns>
	inline uint32_t GetCollisionType()const {
		return collisionType_;
	}


public:
	//衝突属性(自分)を取得
	uint32_t GetCollisionAttribute() {
		return collisionAttribute_;
	}
	//衝突属性(自分)を設定
	void SetCollisionAttribute(const uint32_t& collisionAttribute) {
		this->collisionAttribute_ = collisionAttribute;
	}

	//衝突マスク(相手)を取得
	uint32_t GetCollisionMask() const {
		return collisionMask_;
	}
	//衝突マスク(相手)を設定
	void SetCollisionMask(const uint32_t& collisionMask) {
		this->collisionMask_ = collisionMask;
	}

protected:
	//名前
	std::string name_ = "";


	//当たり判定の種類
	//デフォルトで球にしておく
	uint32_t collisionType_ = ColliderType::SphereType;

	//衝突半径
	float_t radius_ = 1.0f;

	//AABB
	AABB aabb_ = {};

	//3D
	Fan3D fan3D_ = {};

	//2D
	Fan2D fan2D_ = {};

	//平面
	Plane plane_ = {};

private:
	
	//衝突判定(自分)
	uint32_t collisionAttribute_  = 0xffffffff;

	//衝突判定(相手)
	uint32_t collisionMask_ = 0xffffffff;

};

