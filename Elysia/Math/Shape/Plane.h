#pragma once
/**
 * @file Plane.h
 * @brief 平面の構造体
 * @author 茂木翼
 */

#include "Vector3.h"

/// <summary>
/// 平面
/// </summary>
struct Plane {
	//位置
	Vector3 position;
	//縦
	float_t length;
	//横
	float_t width;

};