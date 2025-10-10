#pragma once

/**
 * @file Dissolve.h
 * @brief ディゾルブのクラス
 * @author 茂木翼
 */


#include "DirectXSetup.h"
#include "Vector4.h"
#include "Vector3.h"
#include "VertexData.h"

/// <summary>
/// ディゾルブ
/// </summary>
struct Dissolve;


/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {
	/// <summary>
	/// ウィンドウクラス
	/// </summary>
	class WindowsSetup;

	/// <summary>
	/// DirectXクラス
	/// </summary>
	class DirectXSetup;

	/// <summary>
	/// パイプライン管理クラス
	/// </summary>
	class PipelineManager;

	/// <summary>
	/// RTV管理クラス
	/// </summary>
	class RtvManager;

	/// <summary>
	/// SRV管理クラス
	/// </summary>
	class SrvManager;

	/// <summary>
	/// ディゾルブ(ポストエフェクト)
	/// </summary>
	class DissolvePostEffect {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		DissolvePostEffect();

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="clearColor"></param>
		void Initialize(const Vector4& clearColor);

		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw();

		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="dissolve">ディゾルブ</param>
		void Draw(const Dissolve& dissolve);

		/// <summary>
		/// デストラク
		/// </summary>
		~DissolvePostEffect() = default;

	private:
		//Windowクラス
		Elysia::WindowsSetup* windowSetup_ = nullptr;
		//DirectXクラス
		Elysia::DirectXSetup* directXSetup_ = nullptr;
		//パイプライン管理クラス
		Elysia::PipelineManager* pipelineManager_ = nullptr;
		//RTV管理クラス
		Elysia::RtvManager* rtvManager_ = nullptr;
		//SRV管理クラス
		Elysia::SrvManager* srvManager_ = nullptr;

	private:

		

		//RTV
		ComPtr<ID3D12Resource> rtvResource_ = nullptr;
		//RTVハンドル
		uint32_t rtvHandle_ = 0;
		//レンダーの色
		Vector4 renderTargetClearColor_ = {};
		//SRVハンドル
		uint32_t srvHandle_ = 0;
		//マスクテクスチャハンドル
		uint32_t maskTextureHandle = 0u;

		//バリア
		D3D12_RESOURCE_BARRIER barrier = {};
	};


}