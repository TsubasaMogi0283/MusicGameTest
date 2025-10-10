#pragma once
/**
 * @file BackTexture.h
 * @brief ポストエフェクトのクラス
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
	/// DirectX
	/// </summary>
	class DirectXSetup;
	
	/// <summary>
	/// RTV管理クラス
	/// </summary>
	class RtvManager;

	/// <summary>
	/// SRV管理クラス
	/// </summary>
	class SrvManager;

	/// <summary>
	/// パイプライン管理クラス
	/// </summary>
	class PipelineManager;


	/// <summary>
	/// ポストエフェクト(通常から他のものまで)
	/// </summary>
	class BackTexture {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		BackTexture();

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
		~BackTexture() = default;

	private:
		/// <summary>
		/// タイプ
		/// </summary>
		enum TextureEffectType {
			NoneEffect,	//エフェクトなし
			Monochrome,	//白黒(グレースケール)
			Sepia,		//セピア
			VignetteEffect,	//端が暗くなる

			//Smoothing(平滑化)
			//輪郭などのくっきりしたところをぼかして滑らかな雰囲気を出すよ
			BoxFilter3x3a,
			BoxFilter5x5a,

			//GaussianFilter
			//BoxFilterよりこっちの方良い感じらしい
			GaussianFilter3x3a,
			GaussianFilter5x5a,
		};

		/// <summary>
		/// ビネットに関する情報
		/// </summary>
		struct VignetteInformation {
			//倍
			float scale;
			//乗
			float pow;
		};

		/// <summary>
		/// ガウシアンフィルタに関する情報
		/// </summary>
		struct GaussianFilterInformation {
			//標準偏差
			//StandardDivision。一般的にσ(シグマ)が使われる
			float sigma;
		};

	public:
		/// <summary>
		/// 色の設定
		/// </summary>
		/// <param name="color"></param>
		void SetClearColour(const Vector4& color) {
			this->color_ = color;
		}

	private:
		//ウィンドウクラス
		WindowsSetup* windowsSetup_ = nullptr;
		//DirectX
		DirectXSetup* directXSetup_ = nullptr;
		//RTV管理クラス
		RtvManager* rtvManager_ = nullptr;;
		//SRV管理クラス
		SrvManager* srvManager_ = nullptr;
		//パイプライン管理クラス
		PipelineManager* pipelineManager_ = nullptr;

	private:
		//リソース
		ComPtr<ID3D12Resource> effectResource_ = nullptr;

		int32_t effectType_ = NoneEffect;
		int32_t* effectTypeData_ = nullptr;

		//Vignette
		ComPtr<ID3D12Resource> vignetteResource_ = nullptr;
		VignetteInformation vignetteInformation_ = {};
		VignetteInformation* vignetteData_ = nullptr;

		//GaussianFilter
		ComPtr<ID3D12Resource> gaussianFilterResource_ = nullptr;
		GaussianFilterInformation gaussianFilterInformation_ = {};
		GaussianFilterInformation* gaussianFilterData_ = nullptr;


		uint32_t rtvHandle_ = 0;
		//RTV
		ComPtr<ID3D12Resource> rtvResource_ = nullptr;
		Vector4 color_ = { .x = 0.0f,.y = 0.5f,.z = 0.5f,.w = 1.0f };
		uint32_t srvHandle_ = 0;

		//バリア
		D3D12_RESOURCE_BARRIER barrier = {};
	};

}