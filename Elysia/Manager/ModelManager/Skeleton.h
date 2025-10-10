#pragma once
#pragma once
/**
 * @file Skeleton.h
 * @brief スケルトンの構造体
 * @author 茂木翼
 */

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <Joint.h>
#include <Node.h>

/// <summary>
/// スケルトン
/// </summary>
struct Skeleton {
public:
	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="rootNode"></param>
	void Create(const Node& rootNode);


	/// <summary>
	/// 更新
	/// </summary>
	void Update();


public:
	//RootJointのIndex
	int32_t root;
	//Joint名とIndexとの辞書
	std::map<std::string, int32_t>jointMap;
	//所属しているジョイント
	std::vector<Joint> joints;


};