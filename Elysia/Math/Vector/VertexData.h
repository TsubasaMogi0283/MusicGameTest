#pragma once
/**
 * @file VertexData.h
 * @brief 頂点データ
 * @author 茂木翼
 */

#include "Vector4.h"
#include "Vector2.h"
#include "Vector3.h"



/// <summary>
/// 頂点データ
/// </summary>
struct VertexData {
	Vector4 position;
	Vector2 texCoord;
	Vector3 normal;
};