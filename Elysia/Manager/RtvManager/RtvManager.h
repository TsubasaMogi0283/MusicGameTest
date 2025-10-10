#pragma once
/**
 * @file RtvManager.h
 * @brief RTV管理クラス
 * @author 茂木翼
 */


#include <array>
#include <string>


#include "DirectXSetup.h"

/// <summary>
/// ベクトル(4次元)
/// </summary>
struct Vector4;

/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {
	/// <summary>
	/// RTV管理クラス
	/// </summary>
	class RtvManager final {
	private:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		RtvManager() = default;

		/// <summary>
		/// デストラクタ
		/// </summary>
		~RtvManager() = default;

	public:
		/// <summary>
		/// インスタンスの取得
		/// </summary>
		/// <returns>インスタンス</returns>
		static RtvManager* GetInstance();

		/// <summary>
		/// コピーコンストラクタ禁止
		/// </summary>
		/// <param name="rtvHeapManager"></param>
		RtvManager(const RtvManager& rtvHeapManager) = delete;

		/// <summary>
		/// 代入演算子を無効にする
		/// </summary>
		/// <param name="rtvHeapManager"></param>
		/// <returns></returns>
		RtvManager& operator=(const RtvManager& rtvHeapManager) = delete;

	public:
		/// <summary>
		/// RenderTextureを作る
		/// </summary>
		/// <param name="format">フォーマットクリアカラー</param>
		/// <param name="clearColor"></param>
		/// <returns>リソース</returns>
		static ComPtr<ID3D12Resource> CreateRenderTextureResource(const DXGI_FORMAT& format, const Vector4& clearColor);


		/// <summary>
		/// RenderTextureを作る(Depth版)
		/// </summary>
		/// <param name="format">フォーマット</param>
		/// <param name="clearColor">クリアカラー</param>
		/// <returns>リソース</returns>
		static ComPtr<ID3D12Resource> CreateRenderTextureResourceForDepth(const DXGI_FORMAT& format, const Vector4& clearColor);


		/// <summary>
		/// ディスクリプタヒープの取得
		/// </summary>
		/// <returns>ディスクリプタヒープ</returns>
		inline ComPtr<ID3D12DescriptorHeap> GetRtvDescriptorHeap() const {
			return  rtvDescriptorHeap_;
		}


		// <summary>
		// ハンドルの取得
		// </summary>
		// <param name="number">数</param>
		// <returns>ハンドル</returns>
		inline D3D12_CPU_DESCRIPTOR_HANDLE& GetRtvHandle(const uint32_t& number) const {
			return rtvHandles_[number];
		}




	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// nameによって返すindexが変わる
		/// </summary>
		/// <param name="name"></param>
		/// <returns>ハンドル</returns>
		uint32_t Allocate(const std::string& name);

		/// <summary>
		/// RTV作成
		/// </summary>
		/// <param name="resource">リソース</param>
		/// <param name="handle">ハンドル</param>
		void GenarateRenderTargetView(const ComPtr<ID3D12Resource>& resource, const uint32_t& handle);



	private:

		//最大のサイズ
		static const uint32_t RTV_DESCRIPTOR_SIZE_ = 1024u;
		//インデックス
		uint32_t index_ = 0;

		//ディスクリプタ
		ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
		//ハンドル
		static D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[RTV_DESCRIPTOR_SIZE_];

		/// <summary>
		/// RTVの情報
		/// </summary>
		struct RTVInformation {
			//PostEffect名
			std::string name;
			//index
			uint32_t index;

		};

		std::array<RTVInformation, RTV_DESCRIPTOR_SIZE_ > rtvInformation_={};


	};




};