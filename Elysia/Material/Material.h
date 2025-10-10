#pragma once

/**
 * @file Material.h
 * @brief マテリアル
 * @author 茂木翼
 */



#include <cstdint>

#include "Vector4.h"
#include "Matrix4x4.h"
#include "DirectXSetup.h"
#include "LightingType.h"

 /// <summary>
	 /// データ
	 /// </summary>
struct MaterialData {
	//色
	Vector4 color;
	//ライティングの種類
	int32_t lightingKinds;
	float_t padding[3];
	//UVトランスフォーム
	Matrix4x4 uvTransform;
	//輝度
	float_t shininess;
	//環境光の強さ
	float_t ambientIntensity;
	//環境マッピングするかどうか
	bool isEnviromentMap;

};

/// <summary>
/// マテリアル
/// </summary>
struct Material {
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
	Vector4 color = {.x = 1.0f,.y = 1.0f,.z = 1.0f,.w = 1.0f};
	//Lightingの種類
	int32_t lightingKinds=DirectionalLighting;
	//UV行列
	Matrix4x4 uvTransform = {};
	//輝度
	float_t shininess=100.0f;
	//環境光の強さ
	float_t ambientIntensity=0.0f;
	//環境マップ
	bool isEnviromentMap = false;

	//定数バッファ
	ComPtr<ID3D12Resource> resource = nullptr;




private:

	

	//書き込みのデータ
	MaterialData* materialData_ = nullptr;

};
