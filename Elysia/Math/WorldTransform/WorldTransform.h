#pragma once
/**
 * @file WorldTransform.h
 * @brief ワールドトランスフォーム
 * @author 茂木翼
 */

#include "Matrix4x4.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "DirectXSetup.h"



/// <summary>
/// ワールドトランスフォーム
/// </summary>
struct WorldTransform {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ペアレントの設定
	/// </summary>
	/// <param name="parent"></param>
	inline void SetParent(const WorldTransform* newParent) {
		this->parent = newParent;
	}

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns></returns>
	inline Vector3 GetWorldPosition()const {
		Vector3 position = {
			.x = worldMatrix.m[3][0],
			.y = worldMatrix.m[3][1],
			.z = worldMatrix.m[3][2],
		};

		return position;
	}


private:
	/// <summary>
	/// 転送
	/// </summary>
	void Transfer();

private:
	/// <summary>
	/// シェーダーに送るデータ
	/// </summary>
	struct WorldTransformData {
		//ワールド
		Matrix4x4 world;
		//ノーマル
		Matrix4x4 normal;
		//逆転置
		Matrix4x4 worldInverseTranspose;
	};


public:

	//スケール
	Vector3 scale = {.x = 1.0f,.y = 1.0f,.z = 1.0f };
	//回転
	Vector3 rotate = {.x = 0.0f,.y = 0.0f,.z = 0.0f };
	//座標
	Vector3 translate = {.x = 0.0f,.y = 0.0f,.z = 0.0f };

	//クォータニオンを使うかどうか
	bool isUseQuarternion_ = false;
	//クォータニオン
	Quaternion quaternion_ = {};


	//定数バッファ
	ComPtr<ID3D12Resource> resource = nullptr;
	//送るデータ
	WorldTransformData* tranceformationData = nullptr;

	//ワールド行列
	Matrix4x4 worldMatrix = {};
	//逆転置行列
	Matrix4x4 worldInverseTransposeMatrix = {};

	//親となるワールド変換へのポインタ
	const WorldTransform* parent = nullptr;


};

