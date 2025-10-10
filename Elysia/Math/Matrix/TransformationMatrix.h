#pragma once
/**
 * @file TransformationMatrix.h
 * @brief トランスフォーメーション行列
 * @author 茂木翼
 */

#include "Matrix4x4.h"

/// <summary>
/// トランスフォーメーション行列
/// </summary>
struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};