#pragma once
/**
 * @file Transform.h
 * @brief トランスフォーム
 * @author 茂木翼
 */

#include "Vector3.h"

/// <summary>
/// トランスフォーム
/// </summary>
struct Transform {
	//スケール
	Vector3 scale;
	//回転
	Vector3 rotate;
	//座標
	Vector3 translate;

};