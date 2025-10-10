#pragma once
/**
 * @file PointLight.h
 * @brief 点光源の構造体
 * @author 茂木翼
 */

#include <Vector4.h>
#include <Vector3.h>
#include "DirectXSetup.h"

/// <summary>
/// 点光源のデータ
/// </summary>
struct PointLightData {
	//ライトの色
	Vector4 color;
	//ライトの位置
	Vector3 position;
	//輝度
	float_t intensity;

	//ライトに届く最大距離
	float_t radius;
	//減衰率
	float_t decay;
	//埋め合わせ
	float_t padding[2];

};


/// <summary>
/// 点光源
/// </summary>
struct PointLight {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();


	/// <summary>
	/// 更新
	/// </summary>
	void Update();


public:
	//色
	Vector4 color= {.x =1.0f,.y = 1.0f,.z = 1.0f,.w =1.0f };
	//位置
	Vector3 position = {.x =0.0f,.y = 0.0f,.z = 0.0f};
	//輝度
	float_t intensity=4.0f;
	//ライトに届く最大距離
	float_t radius=5.0f;
	//減衰率
	float_t decay=5.0f;

	//定数バッファ
	ComPtr<ID3D12Resource> resource=nullptr;
	//書き込みデータ
	PointLightData* pointLightdata = nullptr;


};