#pragma once

/**
 * @file LuminanceBasedOutline.h
 * @brief アウトライン(輝度)の管理クラス
 * @author 茂木翼
 */


#include "DirectXSetup.h"
#include "Vector4.h"
#include "Vector3.h"
#include "VertexData.h"


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
	/// アウトライン(輝度)
	/// </summary>
	class LuminanceBasedOutline {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		LuminanceBasedOutline();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 描画前の処理
		/// </summary>
		void PreDraw();

		/// <summary>
		/// 描画
		/// </summary>
		void Draw();


		/// <summary>
		/// デストラクタ
		/// </summary>
		~LuminanceBasedOutline() = default;


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

		//SRV
		//ハンドル
		uint32_t srvHandle_ = 0;

		//RTV
		//リソース
		ComPtr<ID3D12Resource> rtvResource_ = nullptr;
		//ハンドル
		uint32_t rtvHandle_ = 0;

		//バリア
		D3D12_RESOURCE_BARRIER barrier = {};
	};

}