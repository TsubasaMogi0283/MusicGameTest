#pragma once
#pragma once
/**
 * @file VertexWeightData.h
 * @brief 頂点ウェイトの構造体
 * @author 茂木翼
 */

#include <cstdint>

/// <summary>
/// 頂点ウェイト
/// </summary>
struct VertexWeightData {
	//ウェイト
	float weight;
	//頂点のインデックス
	uint32_t vertexIndex;
};