#pragma once
#pragma once
/**
 * @file VertexInfluence.h
 * @brief 頂点インフルエンスの構造体
 * @author 茂木翼
 */

#include <cstdint>
#include <array>

//VSでの
//float32_t4 weight:WEIGHT0;
//int32_t index:INDEX0
//これと形を同じにしてね

//最大サイズ
const uint32_t NUM_MAX_INFLUENCE = 4;

/// <summary>
/// 頂点インフルエンス
/// </summary>
struct VertexInfluence {
	std::array<float, NUM_MAX_INFLUENCE> weights;
	std::array<int32_t, NUM_MAX_INFLUENCE> jointIndices;
};