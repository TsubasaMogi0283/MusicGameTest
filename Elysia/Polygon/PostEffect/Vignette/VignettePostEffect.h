#pragma once
/**
 * @file VignettePostEffect.h
 * @brief ビネットポストエフェクトのクラス
 * @author 茂木翼
 */

#include "DirectXSetup.h"
#include "Vector3.h"


/// <summary>
/// ベクトル(4次元)
/// </summary>
struct Vector4;

/// <summary>
/// ビネット
/// </summary>
struct Vignette;


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
	/// ビネットポストエフェクト
	/// </summary>
	class VignettePostEffect {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		VignettePostEffect();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw();

		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="vignette">ビネット</param>
		void Draw(const Vignette& vignette);

		/// <summary>
		/// デストラクタ
		/// </summary>
		~VignettePostEffect() = default;


	private:
		//ウィンドウクラス
		Elysia::WindowsSetup* windowsSetup_ = nullptr;
		//DirectXクラス
		Elysia::DirectXSetup* directXSetup_ = nullptr;
		//パイプライン管理クラス
		Elysia::PipelineManager* pipelinemanager_ = nullptr;
		//RTV管理クラス
		Elysia::RtvManager* rtvManager_ = nullptr;
		//SRV管理クラス
		Elysia::SrvManager* srvManager_ = nullptr;

	private:
		//ハンドル
		uint32_t rtvHandle_ = 0;
		//RTV
		ComPtr<ID3D12Resource> rtvResource_ = nullptr;

		//SRV
		uint32_t srvHandle_ = 0;
		//リソースバリア
		D3D12_RESOURCE_BARRIER barrier = {};

	};

};