#pragma once
/**
 * @file JointWeightData.h
 * @brief ジョイントのウェイトデータの構造体
 * @author 茂木翼
 */

#include <vector>
#include <Matrix4x4.h>
#include <VertexWeightData.h>


/// <summary>
/// ジョイントのウェイトデータ
/// </summary>
struct JointWeightData {
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;
};