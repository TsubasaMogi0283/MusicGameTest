#pragma once

/**
 * @file Camera.h
 * @brief カメラクラス
 * @author 茂木翼
 */
#include "Matrix4x4.h"
#include "Vector3.h"
#include "DirectXSetup.h"

/// <summary>
/// GPUに送る行列データ
/// </summary>
struct CameraMatrixData {
	//ビュー行列
	Matrix4x4 viewMatrix_;
	//射影行列
	Matrix4x4 projectionMatrix_;
	//正射影行列
	Matrix4x4 orthographicMatrix_;
};

/// <summary>
/// GPUに送る座標データ
/// </summary>
struct CameraForGPU {
	Vector3 worldPosition;
};

/// <summary>
/// カメラ
/// </summary>
struct Camera {
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
	/// 転送
	/// </summary>
	void Transfer();


public:

	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPosition() const{
		Vector3 worldPosition = {
			.x = worldMatrix.m[3][0],
			.y = worldMatrix.m[3][1],
			.z = worldMatrix.m[3][2],
		};
		return worldPosition;
	}


	
public:
	//リソース
	ComPtr<ID3D12Resource> resource;

	//角度
	float_t fov_ = 0.45f;
	//アスペクト比
	float_t aspectRatio = 0.0f;

	//奥行の変数
	float_t nearClip = 0.1f;
	float_t farClip = 1000.0f;

	//回転
	Vector3 rotate = {};
	//移動
	Vector3 translate = {};


	//ワールド列
	Matrix4x4 worldMatrix = {};
	//ビュー行列
	Matrix4x4 viewMatrix = {};
	//射影行列
	Matrix4x4 projectionMatrix = {};
	//正射影行列
	Matrix4x4 orthographicMatrix={};
	//転送用のデータ
	CameraMatrixData* cameraMatrixData = nullptr;

private:
	//スケール
	Vector3 scale = {.x= 1.0f,.y= 1.0f,.z= 1.0f };

};