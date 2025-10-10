#pragma once
/**
 * @file QuaternionTransform.h
 * @brief クォータニオンのトランスフォームの構造体
 * @author 茂木翼
 */

#include <Vector3.h>
#include <Quaternion.h>

/// <summary>
/// クォータニオンのトランスフォーム
/// </summary>
struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};