#pragma once
/**
 * @file AABB.h
 * @brief AABBの構造体
 * @author 茂木翼
 */

#include "Vector3.h"

//AABB...Axis Aligned Bounding Box

/// <summary>
/// AABB
/// </summary>
struct AABB {
	//最小点(手前の左下)
	Vector3 min;
	//最大点(奥の右上)
	Vector3 max;
};