#pragma once

/**
 * @file RandomNoise.h
 * @brief ランダムノイズの構造体
 * @author 茂木翼
 */

#include <random>

#include "Vector3.h"
#include "DirectXSetup.h"

/// <summary>
/// ランダムノイズ
/// </summary>
struct  RandomNoise{
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
	struct RandomNoiseData {
		//値
		float_t value;
		//ノイズの強さ
		float_t strength;
		//テクスチャと乗算させるか
		bool isUseTexture;
	};

	//データ
	RandomNoiseData* randomNoiseData = nullptr;

public:
	//リソース
	ComPtr<ID3D12Resource> resource = nullptr;
	//値
	float_t value=0.0f;
	//ノイズの強さ
	float_t strength=0.0f;
	//テクスチャと乗算させるか
	bool isUseTexture=false;

private:
	//エンジン
	std::mt19937 randomEngine = {};
};

