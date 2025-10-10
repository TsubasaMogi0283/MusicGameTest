#pragma once

/**
 * @file AnimationModel.h
 * @brief アニメーションモデル
 * @author 茂木翼
 */

#include <cstdint>

#include "ModelData.h"
#include "LightingType.h"
#include "Material.h"

/// <summary>
/// ワールドトランスフォーム
/// </summary>
struct WorldTransform;

/// <summary>
/// カメラ
/// </summary>
struct Camera;

/// <summary>
/// GPUに送るカメラ
/// </summary>
struct CameraForGPU;

/// <summary>
/// スキンクラスター
/// </summary>
struct SkinCluster;

/// <summary>
/// 平行光源
/// </summary>
struct DirectionalLight;

/// <summary>
/// 点光源
/// </summary>
struct PointLight;

/// <summary>
/// スポットライト
/// </summary>
struct SpotLight;

/// <summary>
/// マテリアル
/// </summary>
struct Material;


/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {
	/// <summary>
	/// DirectXのクラス
	/// </summary>
	class DirectXSetup;

	/// <summary>
	/// パイプライン管理クラス
	/// </summary>
	class PipelineManager;
	
	/// <summary>
	/// SRV管理クラス
	/// </summary>
	class SrvManager;

	/// <summary>
	/// テクスチャ管理クラス
	/// </summary>
	class TextureManager;

	/// <summary>
	/// モデル管理クラス
	/// </summary>
	class ModelManager;

	/// <summary>
	/// アニメーションモデル
	/// </summary>
	class AnimationModel {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		AnimationModel();

		/// <summary>
		/// 生成
		/// </summary>
		/// <param name="modelHandle">ハンドル</param>
		/// <returns></returns>
		static AnimationModel* Create(const uint32_t& modelHandle);


		/// <summary>
		/// 描画(平行光源)
		/// </summary>
		/// <param name="worldTransform">ワールドトランスフォーム</param>
		/// <param name="camera">カメラ</param>
		/// <param name="skinCluster">スキンクラスター</param>
		/// <param name="material">マテリアル</param>
		/// <param name="directionalLight">平行光源</param>
		void Draw(const WorldTransform& worldTransform, const Camera& camera, const SkinCluster& skinCluster, const Material& material, const DirectionalLight& directionalLight);

		/// <summary>
		/// 描画(点光源)
		/// </summary>
		/// <param name="worldTransform">ワールドトランスフォーム</param>
		/// <param name="camera">カメラ</param>
		/// <param name="skinCluster">スキンクラスター</param>
		/// <param name="material">マテリアル</param>
		/// <param name="pointLight">点光源</param>
		void Draw(const WorldTransform& worldTransform, const Camera& camera, const SkinCluster& skinCluster, const Material& material, const PointLight& pointLight);

		/// <summary>
		/// 描画(スポットライト)
		/// </summary>
		/// <param name="worldTransform">ワールドトランスフォーム</param>
		/// <param name="camera">カメラ</param>
		/// <param name="skinCluster">スキンクラスター</param>
		/// <param name="material">マテリアル</param>
		/// <param name="spotLight">スポットライト</param>
		void Draw(const WorldTransform& worldTransform, const Camera& camera, const SkinCluster& skinCluster, const Material& material, const SpotLight& spotLight);

		/// <summary>
		/// デストラクタ
		/// </summary>
		~AnimationModel() = default;


	public:
		/// <summary>
		/// 環境マップで使うテクスチャのの設定
		/// </summary>
		/// <param name="textureHandle"></param>
		void SetEviromentTexture(const uint32_t& textureHandle) {
			this->eviromentTextureHandle_ = textureHandle;
		}


	private:

		//DirectXのクラス
		Elysia::DirectXSetup* directXSetup_ = nullptr;
		//パイプライン管理クラス
		Elysia::PipelineManager* pipelineManager_ = nullptr;
		//SRV管理クラス
		Elysia::SrvManager* srvManager_ = nullptr;
		//テクスチャ管理クラス
		Elysia::TextureManager* textureManager_ = nullptr;
		//モデル管理クラス
		Elysia::ModelManager* modelManager_ = nullptr;




	private:
		//頂点リソースを作る
		ComPtr<ID3D12Resource> vertexResource_ = nullptr;
		//頂点バッファビューを作成する
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

		//インデックス
		ComPtr<ID3D12Resource> indexResource_ = nullptr;
		D3D12_INDEX_BUFFER_VIEW indexBufferView_{};


		//PixelShaderにカメラの座標を送る為の変数
		ComPtr<ID3D12Resource> cameraResource_ = nullptr;
		CameraForGPU* cameraForGPU_ = {};


		//アニメーションを再生するときに使う時間
		float_t animationTime_ = 0.0f;

		//テクスチャハンドル
		uint32_t textureHandle_ = 0u;

		//環境マップ
		uint32_t eviromentTextureHandle_ = 0;

		//モデルハンドル
		uint32_t modelHandle_ = 0u;
		ModelData modelData_ = {};

		//アニメーションのローカル座標
		//後々シェーダーで渡す
		Matrix4x4 animationLocalMatrix_ = {};


		//アニメーションをするかどうか
		bool isAnimation_ = false;


	};


};



