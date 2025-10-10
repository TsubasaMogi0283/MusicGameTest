#pragma once
/**
 * @file RadialBlur.h
 * @brief ラジアルブラーのクラス
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
	/// ラジアルブラー
	/// </summary>
	class RadialBlur {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		RadialBlur();

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
		void Draw();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~RadialBlur() = default;

	public:
		/// <summary>
		/// 色の設定
		/// </summary>
		/// <param name="color"></param>
		inline void SetColor(const Vector4& color) {
			this->color_ = color;
		}

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
		//SRVハンドル
		uint32_t srvHandle_ = 0;

		//カラー
		Vector4 color_ = { .x = 0.1f,.y = 0.1f,.z = 0.7f,.w = 1.0f };

		//RTV
		//リソース
		ComPtr<ID3D12Resource> rtvResource_ = nullptr;
		//ハンドル
		uint32_t rtvHandle_ = 0;

		//バリア
		D3D12_RESOURCE_BARRIER barrier = {};
	};


};