#pragma once
/**
 * @file EulerTransform.h
 * @brief トランスフォーム(オイラー)
 * @author 茂木翼
 */

 /// <summary>
 /// ベクトル(3D)
 /// </summary>

#include <Vector3.h>

/// <summary>
/// トランスフォーム(オイラー)
/// </summary>
struct EulerTransform {
	//スケール
	Vector3 scale;
	//回転
	Vector3 rotate;
	//座標
	Vector3 translate;
};
