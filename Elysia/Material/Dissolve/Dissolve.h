#pragma once

/**
 * @file Dissolve.h
 * @brief ディゾルブの構造体
 * @author 茂木翼
 */

#include "Vector3.h"
#include "DirectXSetup.h"


/// <summary>
/// ディゾルブ
/// </summary>
struct Dissolve{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


private:
	/// <summary>
	/// データ
	/// </summary>
	struct DissolveData {
		//Edgeを使うかどうか
		bool isUseEdge;
		//Edgeの色
		Vector3 edgeColor;
		//Edgeの厚さ
		float_t edgeThinkness;
		//閾値
		float_t threshold;
	};

	//データ
	DissolveData* dissolveData = nullptr;

public:
	//リソース
	ComPtr<ID3D12Resource> resource = nullptr;

	//Edgeを使うかどうか
	bool isUseEdge;
	//Edgeの色
	Vector3 edgeColor;
	//Edgeの厚さ
	float_t edgeThinkness;
	//閾値
	float_t threshold;
	//マスクのテクスチャハンドル
	uint32_t maskTextureHandle = 0u;

	


};

