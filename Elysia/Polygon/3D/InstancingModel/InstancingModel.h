#pragma once
/**
 * @file InstancingModel.h
 * @brief インスタンシングモデル
 * @author 茂木翼
 */

#include "DirectXSetup.h"

#include "Matrix4x4.h"
#include "Vector4.h"
#include "TransformationMatrix.h"
#include "Matrix4x4Calculation.h"
#include "VertexData.h"
#include "LightingType.h"
#include "ModelData.h"

#pragma region 前方宣言


/// <summary>
/// ワールドトランスフォーム
/// </summary>
struct WorldTransform;

/// <summary>
/// カメラ
/// </summary>
struct Camera;

/// <summary>
/// GPUに送るためのカメラ
/// </summary>
struct CameraForGPU;

/// <summary>
/// マテリアル
/// </summary>
struct Material;

/// <summary>
/// 点光源
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
/// ElysiaEngine
/// </summary>
namespace Elysia {
	

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
};


#pragma endregion


/// <summary>
/// インスタンシング描画
/// </summary>
class InstancingModel{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	InstancingModel();

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="modelHandle">モデルハンドル</param>
	/// <returns></returns>
	static InstancingModel* Create(const uint32_t& modelHandle);

#pragma region 描画

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="camera">カメラ</param>
	/// <param name="directionalLight">平行光源</param>
	void Draw(const WorldTransform& worldTransform, const Camera& camera, const Material& material, const DirectionalLight& directionalLight);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="worldTransform"></param>
	/// <param name="camera"></param>
	/// <param name="pointLight"></param>
	void Draw(const WorldTransform& worldTransform, const Camera& camera, const Material& material, const PointLight& pointLight);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="worldTransform"></param>
	/// <param name="camera"></param>
	/// <param name="spotLight"></param>
	void Draw(const WorldTransform& worldTransform, const Camera& camera, const Material& material, const SpotLight& spotLight);

#pragma endregion

	/// <summary>
	/// デストラクタ
	/// </summary>
	~InstancingModel() = default;


public:


	/// <summary>
	/// 環境マップテクスチャの設定
	/// </summary>
	/// <param name="textureHandle"></param>
	void SetEviromentTexture(const uint32_t& textureHandle) {
		this->eviromentTextureHandle_ = textureHandle;
	}



private:
	//DirectXクラス
	Elysia::DirectXSetup* directXSetup_ = nullptr;
	//テクスチャ管理クラス
	Elysia::TextureManager* textureManager_ = nullptr;
	//モデル管理クラス
	Elysia::ModelManager* modelmanager_ = nullptr;
	//パイプライン管理クラス
	Elysia::PipelineManager* pipelineManager_ = nullptr;
	//SRV管理クラス
	Elysia::SrvManager* srvManager_ = nullptr;

private:
	//頂点
	//リソース
	ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	//バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//インデックス
	//リソース
	ComPtr<ID3D12Resource> indexResource_ = nullptr;
	//バッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};


	//カメラリソース
	ComPtr<ID3D12Resource> cameraResource_ = nullptr;
	//PixelShaderにカメラの座標を送る為の変数
	CameraForGPU* cameraForGPU_ = {};


	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	//環境マップ
	uint32_t eviromentTextureHandle_ = 0;

	//モデルハンドル
	ModelData modelData_ = {};




};

