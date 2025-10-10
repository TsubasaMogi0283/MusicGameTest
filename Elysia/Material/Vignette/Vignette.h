#pragma once

/**
 * @file Vignette.h
 * @brief ビネットのクラス
 * @author 茂木翼
 */

#include "DirectXSetup.h"
#include "Vector3.h"

/// <summary>
/// ビネット
/// </summary>
struct Vignette {
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
	/// 送るデータ
	/// </summary>
	struct VignetteData {
	public:
		//倍
		float_t scale;
		//乗
		float_t pow;
		//色
		Vector3 color;

	private:
		float_t padding[3];

	};

	//データ
	VignetteData* vignetteData = nullptr;

public:

	//リソース
	ComPtr<ID3D12Resource> resource = nullptr;
	//倍
	float_t scale=0.0f;
	//乗
	float_t pow=0.0f;
	//色
	Vector3 color = {};

};