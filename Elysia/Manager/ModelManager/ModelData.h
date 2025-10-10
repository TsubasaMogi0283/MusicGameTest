#pragma once
/**
 * @file ModelData.h
 * @brief モデルデータの構造体
 * @author 茂木翼
 */

#include <vector>
#include <VertexData.h>
#include <Node.h>
#include <map>
#include "JoinWeightData.h"

/// <summary>
/// モデルデータ
/// </summary>
struct ModelData {
	//スキンクラスターデータ
	std::map<std::string, JointWeightData> skinClusterData;
	//頂点
	std::vector<VertexData> vertices;
	//index
	std::vector <uint32_t>indices;
	//テクスチャのパス
	std::string textureFilePath;
	//ノード
	Node rootNode;
};