#pragma once
/**
 * @file GaussianFilter.h
 * @brief ガウシアンフィルタのクラス
 * @author 茂木翼
 */


#include "DirectXSetup.h"
#include "Vector4.h"
#include "Vector3.h"
#include "VertexData.h"

/// <summary>
/// 種類
/// </summary>
enum GaussianFilterSelection {
	//GaussianFilter
	//BoxFilterよりこっちの方良い感じらしい
	GaussianFilter3x3,
	GaussianFilter5x5,



};

/// <summary>
/// データ
/// </summary>
struct GaussianFilterData {
	//種類
	int32_t type;
	
	//標準偏差
	//StandardDivision。一般的にσ(シグマ)が使われる
	float sigma;

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
	/// ガウシアンフィルタ
	/// </summary>
	class GaussianFilter {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		GaussianFilter();


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
		~GaussianFilter() = default;


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
		GaussianFilterData* boxFilterTypeData_ = nullptr;
		int32_t boxFilterType_ = GaussianFilter3x3;
		float sigma_ = 0.0f;

		//RTV
		//ハンドル
		uint32_t rtvHandle_ = 0;
		//リソース
		ComPtr<ID3D12Resource> rtvResource_ = nullptr;
		
		//SRVハンドル
		uint32_t srvHandle_ = 0;

		//バリア
		D3D12_RESOURCE_BARRIER barrier = {};
	};
}