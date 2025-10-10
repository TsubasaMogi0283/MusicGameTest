#pragma once
/**
 * @file AccelerationField.h
 * @brief 加速地帯
 * @author 茂木翼
 */

#include "Vector3.h"
#include "AABB.h"

/// <summary>
/// 加速地帯
/// </summary>
struct AccelerationField {
	Vector3 acceleration;
	AABB area;
};