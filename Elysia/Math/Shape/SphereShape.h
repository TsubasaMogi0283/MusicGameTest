#pragma once
/**
 * @file SphereShape.h
 * @brief 球の構造体
 * @author 茂木翼
 */

#include "Vector3.h"

/// <summary>
/// 球
/// </summary>
struct SphereShape {
	Vector3 center;
	float_t radius;
};