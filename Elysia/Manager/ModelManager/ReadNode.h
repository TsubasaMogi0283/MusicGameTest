#pragma once

#pragma once
/**
 * @file ReadNode.h
 * @brief ノードの読み込みクラス
 * @author 茂木翼
 */

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Node.h>


/// <summary>
/// ノードの読み込み
/// </summary>
class ReadNode final{
private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ReadNode()=default;
	
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ReadNode() = default;

public:
	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns></returns>
	static ReadNode* GetInstance();

	/// <summary>
	/// コピーコンストラクタ禁止
	/// </summary>
	/// <param name="readNode"></param>
	ReadNode(const ReadNode& readNode) = delete;

	/// <summary>
	/// 代入演算子を無効にする
	/// </summary>
	/// <param name="readNode"></param>
	/// <returns></returns>
	ReadNode& operator=(const ReadNode& readNode) = delete;


public:
	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="node"></param>
	/// <returns></returns>
	Node Read(aiNode* node);

};

