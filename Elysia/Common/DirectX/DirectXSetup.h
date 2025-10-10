#pragma once
/**
 * @file DirectXSetup.h
 * @brief DirectXの設定クラス
 * @author 茂木翼
 */

#include <string>
#include <cassert>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <chrono>


#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

#include <wrl.h>
using Microsoft::WRL::ComPtr;

/// <summary>
/// リソースリークチェック
/// ReportLiveObjects
/// DirectX12より低レベルのDXGIに問い合わせをする
/// </summary>
struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		ComPtr<IDXGIDebug1>debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))){
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);

		}
	}
};


/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia{

	/// <summary>
	/// ウィンドウクラス
	/// </summary>
	class WindowsSetup;

	/// <summary>
	/// DirectXの機能をまとめたクラス
	/// </summary>
	class DirectXSetup final {
	private:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		DirectXSetup();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~DirectXSetup() = default;

	public:
		/// <summary>
		/// インスタンスの取得
		/// </summary>
		/// <returns>インスタンス</returns>
		static DirectXSetup* GetInstance();

		/// <summary>
		/// コピーコンストラクタ禁止
		/// </summary>
		/// <param name="directXSetup"></param>
		DirectXSetup(const DirectXSetup& directXSetup) = delete;

		/// <summary>
		/// 代入演算子を無効にする
		/// </summary>
		/// <param name="directXSetup"></param>
		/// <returns></returns>
		DirectXSetup& operator=(const DirectXSetup& directXSetup) = delete;


		/// <summary>
		/// DescriptorHeapの作成関数
		/// </summary>
		/// <param name="heapType"></param>
		/// <param name="numDescriptors"></param>
		/// <param name="shaderVisible"></param>
		/// <returns></returns>
		static ComPtr<ID3D12DescriptorHeap> GenarateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType,UINT numDescriptors, bool shaderVisible);



	private:
		/// <summary>
		/// DepthのResourceを生成する
		/// </summary>
		/// <param name="width"></param>
		/// <param name="height"></param>
		/// <returns></returns>
		static ComPtr<ID3D12Resource> GenerateDepthStencilTextureResource(const uint32_t& width, const uint32_t& height);


		/// <summary>
		/// DXGIFactoryの生成
		/// </summary>
		void GenerateDXGIFactory();

		/// <summary>
		/// アダプターの生成
		/// </summary>
		void SelectAdapter();

		/// <summary>
		/// デバイスの生成
		/// </summary>
		void GenerateD3D12Device();

		/// <summary>
		/// エラーが起こったら止める
		/// </summary>
		void StopErrorWarning();

		/// <summary>
		/// コマンドの生成
		/// </summary>
		void GenerateCommand();

		/// <summary>
		/// スワップチェーンの生成
		/// </summary>
		void GenerateSwapChain();

		/// <summary>
		/// ディスクリプタヒープの生成
		/// </summary>
		void GenarateDescriptorHeap();

		/// <summary>
		/// スワップチェーンを引っ張ってくる
		/// </summary>
		void PullResourcesFromSwapChain();

		/// <summary>
		/// フェンスの生成
		/// </summary>
		void GenarateFence();


	public:
		/// <summary>
		/// ビューポートの生成
		/// </summary>
		/// <param name="width">横幅</param>
		/// <param name="height">縦幅</param>
		void GenarateViewport(const uint32_t& width, const uint32_t& height);


		/// <summary>
		/// シザーの生成
		/// </summary>
		/// <param name="right_">横幅</param>
		/// <param name="bottom">立幅</param>
		void GenarateScissor(const uint32_t& right, const uint32_t& bottom);


		/// <summary>
		/// リソースバリアの設定
		/// </summary>
		/// <param name="resource"></param>
		/// <param name="beforeState"></param>
		/// <param name="afterState"></param>
		void SetResourceBarrier(const ComPtr<ID3D12Resource>& resource, const D3D12_RESOURCE_STATES& beforeState, const D3D12_RESOURCE_STATES& afterState);

		/// <summary>
		/// リソースバリアの設定(スワップチェイン用)
		/// </summary>
		/// <param name="beforeState"></param>
		/// <param name="afterState"></param>
		void SetResourceBarrierForSwapChain(const D3D12_RESOURCE_STATES& beforeState, const D3D12_RESOURCE_STATES& afterState);



	private:
		/// <summary>
		/// FPS固定初期化
		/// </summary>
		void InitializeFPS();

		/// <summary>
		/// FPS固定更新
		/// </summary>
		void UpdateFPS();


	public:
		/// <summary>
		/// 第一初期化
		/// </summary>
		void FirstInitialize();

		/// <summary>
		/// 第二初期化
		/// </summary>
		void SecondInitialize();

		/// <summary>
		/// Resource作成の関数化
		/// </summary>
		/// <param name="sizeInBytes">サイズ</param>
		/// <returns>リソース</returns>
		ComPtr<ID3D12Resource> CreateBufferResource(const size_t& sizeInBytes);

		/// <summary>
		/// CompilerShader関数
		/// </summary>
		/// <param name="filePath">CompilerするShaderファイルへのパス</param>
		/// <param name="profile">Compilerに使用するProfile</param>
		/// <returns></returns>
		IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile);

		/// <summary>
		/// 描画開始
		/// </summary>
		void StartDraw();

		/// <summary>
		/// 描画衆力
		/// </summary>
		void EndDraw();

		/// <summary>
		/// 解放
		/// </summary>
		void Release();


		/// <summary>
		/// スワップチェインで使う変数をまとめた
		/// </summary>
		struct SwapChain {
			ComPtr<IDXGISwapChain4> swapChain;
			ComPtr<ID3D12Resource> resource[2];
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		};


	public:


		/// <summary>
		/// デバイスの取得
		/// </summary>
		/// <returns></returns>
		inline ComPtr<ID3D12Device> GetDevice() const {
			return device_;
		}

		/// <summary>
		/// コマンドリストの取得
		/// </summary>
		/// <returns></returns>
		inline ComPtr<ID3D12GraphicsCommandList> GetCommandList() const {
			return DirectXSetup::GetInstance()->commandList_;
		}

		/// <summary>
		/// DSVディスクリプタヒープの取得
		/// </summary>
		/// <returns></returns>
		inline ComPtr<ID3D12DescriptorHeap> GetDsvDescriptorHeap() const {
			return  dsvDescriptorHeap_;
		}


		/// <summary>
		/// スワップチェーンの取得
		/// </summary>
		/// <returns></returns>
		inline SwapChain GetSwapChain() const {
			return swapChain_;
		}

		/// <summary>
		/// DSVハンドルの取得
		/// </summary>
		/// <returns></returns>
		inline D3D12_CPU_DESCRIPTOR_HANDLE& GetDsvHandle() {
			return dsvHandle_;
		}


		/// <summary>
		/// Depthリソースの取得
		/// </summary>
		/// <returns></returns>
		inline ComPtr<ID3D12Resource> GetDepthStencilResource() const {
			return depthStencilResource_;
		}

	private:
		//ウィンドウクラス
		WindowsSetup* windowsSetup_ = nullptr;

	private:

		//ファクトリー
		ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
		//アダプター
		ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
		//デバイス
		ComPtr<ID3D12Device> device_ = nullptr;

		//コマンドリスト
		ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
		//コマンドキュー
		ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
		//コマンドアロケータ
		ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;

		//DSV
		ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;
		//リソース
		ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
		//ハンドル
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_ = {};

		//スワップチェイン
		SwapChain swapChain_ = {};
		UINT backBufferIndex_ = {};
		//バリア
		D3D12_RESOURCE_BARRIER barrier_{};

		//スワップチェーン
		//RTVハンドル
		uint32_t rtvHandle_[2] = {};
		std::string swapChainName_[2] = {};


		//Fence
		ComPtr<ID3D12Fence> fence_ = nullptr;
		//バリュー
		uint64_t fenceValue_ = 0;
		//イベント
		HANDLE fenceEvent_ = nullptr;

		//デバッグコントローラー
		ComPtr<ID3D12Debug1> debugController_ = nullptr;

		//DXC
		ComPtr<IDxcUtils> dxcUtils_ = nullptr;
		ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
		ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;

		//FPS
		//記録時間(FPS固定用)
		std::chrono::steady_clock::time_point frameEndTime_;

	};

};