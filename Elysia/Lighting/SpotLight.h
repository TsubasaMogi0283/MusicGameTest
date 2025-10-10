#pragma once
/**
 * @file SpotLight.h
 * @brief スポットライトのクラス
 * @author 茂木翼
 */

#include "Vector4.h"
#include "Vector3.h"
#include "DirectXSetup.h"


 /// <summary>
 /// スポットライト
 /// </summary>
struct SpotLight {
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
	/// スポットライトのデータ
	/// </summary>
	struct SpotLightData {
		//ライトの色
		Vector4 color;
		//ライトの位置
		Vector3 position;
		//輝度
		float_t intensity;
		//スポットライトの方向
		Vector3 direction;
		//ライトの届く最大距離
		float_t distance;
		//減衰率
		float_t decay;
		//Fallowoffを制御する
		float_t cosFallowoffStart;
		//スポットライトの余弦
		float_t cosAngle;
		//当たっていない所をこれで明るくする
		float_t aroundOffset;

		//埋め合わせ
		float_t padding[2];

	};


public:
	//ライトの色
	Vector4 color;
	//ライトの位置
	Vector3 position;
	//輝度
	float_t intensity;
	//方向
	Vector3 direction;
	//届く最大距離
	float_t distance;
	//減衰率
	float_t decay;
	//Fallowoffを制御する
	float_t cosFallowoffStart;
	//スポットライトの余弦
	float_t cosAngle;
	//当たっていない所をこれで明るくする
	float_t aroundOffset;

	//定数バッファ
	ComPtr<ID3D12Resource> resource = nullptr;
	//書き込みデータ
	SpotLightData* data_ = nullptr;


};
