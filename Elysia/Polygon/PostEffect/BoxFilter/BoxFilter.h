#pragma once
/**
 * @file BoxFilter.h
 * @brief ボックスフィルタのクラス
 * @author 茂木翼
 */


#include "DirectXSetup.h"
#include "Vector4.h"
#include "Vector3.h"
#include "VertexData.h"

/// <summary>
/// ボックスフィルタの種類
/// </summary>
enum BoxFilterSelection {
	BoxFilter3x3,
	BoxFilter5x5,

}; 

/// <summary>
/// 平滑化に関するデータ
/// </summary>
struct BoxFilterType {

	//Smoothing(平滑化)
	//輪郭などのくっきりしたところをぼかして滑らかな雰囲気を出すよ
	int32_t type;

};

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
	/// ボックスフィルタ
	/// </summary>
	class BoxFilter {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		BoxFilter();

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
		~BoxFilter() = default;

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
		//リソース
		ComPtr<ID3D12Resource> boxFilterTypeResource_ = nullptr;
		//データ
		BoxFilterType* boxFilterTypeData_ = nullptr;
		//型
		uint32_t boxFilterType_ = BoxFilter3x3;

		//RTV
		uint32_t rtvHandle_ = 0;
		ComPtr<ID3D12Resource> rtvResource_ = nullptr;

		//SRV
		uint32_t srvHandle_ = 0;

		//色
		Vector4 color_ = { .x = 1.0f,.y = 0.0f,.z = 0.0f,.w = 1.0f };

		//バリア
		D3D12_RESOURCE_BARRIER barrier = {};
	};
}