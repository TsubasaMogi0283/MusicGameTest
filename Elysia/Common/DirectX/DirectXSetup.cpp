#include "DirectXSetup.h"
#include <thread>
#include <d3dx12.h>

#include "WindowsSetup.h"
#include "Convert.h"
#include "PipelineManager.h"
#include "SrvManager.h"
#include "RtvManager.h"
#include "Vector4.h"



Elysia::DirectXSetup::DirectXSetup() {
	//ウィンドウクラスのインスタンスを取得
	windowsSetup_ = Elysia::WindowsSetup::GetInstance();
}

Elysia::DirectXSetup* Elysia::DirectXSetup::GetInstance() {
	//関数内static変数として宣言する
	static DirectXSetup instance;
	return &instance;
}


ComPtr<ID3D12DescriptorHeap> Elysia::DirectXSetup::GenarateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType,UINT numDescriptors, bool shaderVisible) {

	ComPtr<ID3D12DescriptorHeap> descriptorHeap= nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc={
		//型
		.Type = heapType,
		//ディスクリプタの数
		.NumDescriptors = numDescriptors,
		//フラグの設定
		.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE,

	};
	
	//生成
	HRESULT hr = DirectXSetup::GetInstance()->device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));

	return descriptorHeap;

}

ComPtr<ID3D12Resource> Elysia::DirectXSetup::CreateBufferResource(const size_t& sizeInBytes) {

	ComPtr<ID3D12Resource> resource = nullptr;
	
	//HeapPropertiesの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties_ = {
		.Type= D3D12_HEAP_TYPE_UPLOAD,
	};
	
	//ResourceDescの設定
	D3D12_RESOURCE_DESC resourceDesc_ = {
		//バッファリソース。テクスチャの場合はまた別の設定をする
		.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
		.Width = sizeInBytes,
		.Height = 1,
		.DepthOrArraySize = 1,
		.MipLevels = 1,
		.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
	};
	resourceDesc_.SampleDesc.Count = 1;

	//作成
	HRESULT hr = DirectXSetup::GetInstance()->GetDevice()->CreateCommittedResource(
		&uploadHeapProperties_,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc_,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}

ComPtr<ID3D12Resource> Elysia::DirectXSetup::GenerateDepthStencilTextureResource(const uint32_t& width, const uint32_t& height) {
	D3D12_RESOURCE_DESC resourceDesc{
		//2次元
		.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		//Textureの幅
		.Width = width,
		//Textureの高さ
		.Height = height,
		//奥行 or 配列Textureの配列数
		.DepthOrArraySize = 1,
		//mipmapの数
		.MipLevels = 1,
		//DepthStencilとして利用可能なフォーマット
		.Format = DXGI_FORMAT_D24_UNORM_S8_UINT,		
		//DepthStencilとして使う通知
		.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
	};
	//サンプリングカウント。1固定
	resourceDesc.SampleDesc.Count = 1;


	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{
		//VRAM上に作る
		.Type = D3D12_HEAP_TYPE_DEFAULT,
	};
	
	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{
		//フォーマット。Resourceと合わせる
		.Format= DXGI_FORMAT_D24_UNORM_S8_UINT,
	};
	//1.0f(最大値)でクリア
	depthClearValue.DepthStencil.Depth = 1.0f;


	//Resourceの作成
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = Elysia::DirectXSetup::GetInstance()->device_->CreateCommittedResource(
		//Heapの設定 
		&heapProperties,
		//Heapの特殊な設定。特になし。
		D3D12_HEAP_FLAG_NONE,
		//Resourceの設定
		&resourceDesc,
		//深度値を書き込む状態にしておく
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		//Clear最適値
		&depthClearValue,
		//作成するResourceポインタへのポインタ
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;

}

#pragma region Initializeの所で使う関数

void Elysia::DirectXSetup::GenerateDXGIFactory() {
	//DXGIファクトリーの生成
#ifdef _DEBUG
	ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);

	}

#endif 
	
}

void Elysia::DirectXSetup::SelectAdapter() {
	//仕様するアダプタ用の変数、最初にnullptrを入れておく

	ComPtr<IDXGIFactory7> dxgiFactory;
	//関数が成功したかSUCCEEDEDでマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	//初期でエラーが発生した場合どうにもできないのでassert
	assert(SUCCEEDED(hr));
	DirectXSetup::GetInstance()->dxgiFactory_ = dxgiFactory;

	ComPtr<IDXGIAdapter4> useAdapter = nullptr;

	//良い順でアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; i++) {

		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));

		//ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報をログに出力.(wstring)
			OutputDebugStringA(Convert::Text::ToString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)).c_str());
			break;
		}
		//ソフトウェアアダプタだった場合無視
		useAdapter = nullptr;


	}
	//適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);
	
	//更新したデータをメンバ変数にまた保存
	DirectXSetup::GetInstance()->dxgiFactory_ = dxgiFactory;
	DirectXSetup::GetInstance()->useAdapter_ = useAdapter;



}

void Elysia::DirectXSetup::GenerateD3D12Device() {
	//機能レベルとログ出力用の文字
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0
	};
	

	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成出来るか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプターでデバイスが生成
		HRESULT hr = {};
		hr = D3D12CreateDevice(
			DirectXSetup::GetInstance()->useAdapter_.Get(), 
			featureLevels[i], 
			IID_PPV_ARGS(&DirectXSetup::GetInstance()->device_));
		//指定した機能レベルでデバイスが生成できたか確認
		if (SUCCEEDED(hr)) {
			//生成できたのでログ出力を行ってループを抜ける
			windowsSetup_->OutPutStringA(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}

	//デバイスの生成が上手くいかなかったので起動できない
	assert(DirectXSetup::GetInstance()->device_ != nullptr);
	windowsSetup_->OutPutStringA("Complete create D3D12Device!!!\n");

}

void Elysia::DirectXSetup::StopErrorWarning() {

#ifdef _DEBUG
	

	//エラー・警告、即ち停止
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(DirectXSetup::GetInstance()->device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		////全ての情報を出す
		//以下をコメントアウト
		//大丈夫だった場合元に戻してあげる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		

		////エラーと警告の抑制
		//Windowsの不具合だと解消できない
		//その時に停止させないよう特定のエラーや警告を無視するしかない

		//抑制するメッセージのID 		
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGデバッグれーやーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		//抑制する
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter={};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif 

}

void Elysia::DirectXSetup::GenerateCommand() {
	
	//コマンドキューを生成する
	HRESULT hr = {};
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc_{};
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	hr = DirectXSetup::GetInstance()->device_->CreateCommandQueue(&commandQueueDesc_, IID_PPV_ARGS(&commandQueue));
	//コマンドキューの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドアロケータを生成する
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	hr = DirectXSetup::GetInstance()->device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	//コマンドアロケータの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドリストを生成する
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	hr = DirectXSetup::GetInstance()->device_->CreateCommandList(
		0, 
		D3D12_COMMAND_LIST_TYPE_DIRECT, 
		commandAllocator.Get(),
		nullptr, 
		IID_PPV_ARGS(&commandList));

	//コマンドリストの生成が上手くいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//ローカルに入れた値をメンバ変数に保存しよう
	DirectXSetup::GetInstance()->commandQueue_ = commandQueue;
	DirectXSetup::GetInstance()->commandAllocator_ = commandAllocator;
	DirectXSetup::GetInstance()->commandList_ = commandList;
}

void Elysia::DirectXSetup::GenerateSwapChain() {
	
	//60fpsそのまま映すと大変なので2枚用意して
	//描画(フロントバッファ)と表示(バックバッファ、プライマリバッファ)に分ける。
	//このことをダブルバッファリングという。
	HWND hwnd = WindowsSetup::GetInstance()->GetHwnd();



	DXGI_SAMPLE_DESC sampleDesc = {
		//カウント
		.Count = 1u,
	};

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc={
		//画面の幅。ウィンドウのクライアント領域を同じものにしておく
		.Width = WindowsSetup::GetInstance()->GetClientWidth(),
		
		//画面の高さ。ウィンドウのクライアント領域を同じものにしておく
		.Height = WindowsSetup::GetInstance()->GetClientHeight(),
		
		//色の形式
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		
		//マルチサンプルしない
		.SampleDesc = sampleDesc,

		//描画のターゲットとして利用する
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,				
		
		//ダブルバッファ
		.BufferCount = 2,

		//モニタにうつしたら中身を破棄
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,					

	
	};
	

	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	HRESULT hr = DirectXSetup::GetInstance()->dxgiFactory_->CreateSwapChainForHwnd(
		DirectXSetup::GetInstance()->commandQueue_.Get(),
		hwnd, 
		&swapChainDesc, 
		nullptr, 
		nullptr, 
		reinterpret_cast<IDXGISwapChain1**>(&DirectXSetup::GetInstance()->swapChain_));
	assert(SUCCEEDED(hr));
	
	DirectXSetup::GetInstance()->swapChain_.swapChainDesc = swapChainDesc;

}

void Elysia::DirectXSetup::GenarateDescriptorHeap() {
	
	ComPtr<ID3D12Resource> depthStencilResource = GenerateDepthStencilTextureResource(
		WindowsSetup::GetInstance()->GetClientWidth(),
		WindowsSetup::GetInstance()->GetClientHeight());

	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = GenarateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);



	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	//Format 基本的にはResourceに合わせる
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//2DTexture
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	//DSVHeapの先頭にDSVを作る
	DirectXSetup::GetInstance()->device_->CreateDepthStencilView(
		depthStencilResource.Get(), 
		&dsvDesc, 
		dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	DirectXSetup::GetInstance()->dsvDescriptorHeap_ = dsvDescriptorHeap;
	DirectXSetup::GetInstance()->depthStencilResource_ = depthStencilResource;
}

void Elysia::DirectXSetup::PullResourcesFromSwapChain() {
	 
	HRESULT hr = DirectXSetup::GetInstance()->swapChain_.swapChain->GetBuffer(0, IID_PPV_ARGS(& DirectXSetup::GetInstance()->swapChain_.resource[0]));
	//上手く取得できなければ起動できない
	assert(SUCCEEDED(hr));
	hr = DirectXSetup::GetInstance()->swapChain_.swapChain->GetBuffer(1, IID_PPV_ARGS(& DirectXSetup::GetInstance()->swapChain_.resource[1]));
	assert(SUCCEEDED(hr));

	

}

void Elysia::DirectXSetup::GenarateFence() {

	//上の2つはSwapChain用

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = DirectXSetup::GetInstance()->dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	DirectXSetup::GetInstance()->dsvHandle_ = dsvHandle;


	//FenceとEvent
	//Fence・・・CPUとGPUの同期を取るために利用するオブジェクト。
	//			 GPUで値を書き込み、CPUで値を読み取ったりWindowsにメッセージ(Event)を送ったりできる
	//			 理想を実現するためのもの
	//Event・・・Windowsへのメッセージなどのこと
	//初期位置0でフェンスを作る
	//EventはWindowsのものである
	uint64_t fenceValue = 0;
	ComPtr<ID3D12Fence> fence = nullptr;
	HRESULT hr = DirectXSetup::GetInstance()->device_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	//FenceのSignalを待つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);


	DirectXSetup::GetInstance()->fenceValue_ = fenceValue;
	DirectXSetup::GetInstance()->fenceEvent_ = fenceEvent;
	DirectXSetup::GetInstance()->fence_ = fence;


}



void Elysia::DirectXSetup::GenarateViewport(const uint32_t& width,const uint32_t& height) {
	//クライアント領域のサイズと一緒にして画面全体に表示
	D3D12_VIEWPORT viewport = {
		.TopLeftX = 0.0f,
		.TopLeftY = 0.0f,
		.Width = static_cast<float>(width),
		.Height= static_cast<float>(height),
		.MinDepth = 0.0f,
		.MaxDepth = 1.0f
	};
	

	//ビューポートの設定
	DirectXSetup::GetInstance()->GetCommandList()->RSSetViewports(1, &viewport);

}

void Elysia::DirectXSetup::GenarateScissor(const uint32_t& right,const uint32_t& bottom) {
	
	//基本的にビューポートと同じ矩形が構成されるようにする

	//シザー矩形 
	D3D12_RECT scissorRect={
		.left=0u,
		.top=0u,
		.right=static_cast<LONG>(right),
		.bottom=static_cast<LONG>(bottom),
	};
	
	//シザーを生成
	DirectXSetup::GetInstance()->GetCommandList()->RSSetScissorRects(1, &scissorRect);
	

}

void Elysia::DirectXSetup::SetResourceBarrier(const ComPtr<ID3D12Resource>& resource,const D3D12_RESOURCE_STATES& beforeState,const D3D12_RESOURCE_STATES& afterState){
	

	//TransitionBarrierを張るコード
	//現在のResourceStateを設定する必要がある → ResorceがどんなStateなのかを追跡する必要がある
	//追跡する仕組みはStateTrackingという
	D3D12_RESOURCE_BARRIER barrier={
		//タイプを設定
		.Type= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
		//Noneにする
		.Flags= D3D12_RESOURCE_BARRIER_FLAG_NONE,
	};

	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = resource.Get();
	//遷移前(現在)のResourceState
	barrier.Transition.StateBefore = beforeState;
	//遷移後のResourceState
	barrier.Transition.StateAfter = afterState;
	//TransitionBarrierを張る
	DirectXSetup::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);


}

void Elysia::DirectXSetup::SetResourceBarrierForSwapChain(const D3D12_RESOURCE_STATES& beforeState,const D3D12_RESOURCE_STATES& afterState){

	//TransitionBarrierを張るコード
	//現在のResourceStateを設定する必要がある → ResorceがどんなStateなのかを追跡する必要がある
	//追跡する仕組みはStateTrackingという
	
	//コマンドを積みこんで確定させる
	//これから書き込むバックバッファのインデックスを取得
	DirectXSetup::GetInstance()->backBufferIndex_ = DirectXSetup::GetInstance()->swapChain_.swapChain->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrier = {
		//タイプを設定
		.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
		//Noneにする
		.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
	};
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = DirectXSetup::GetInstance()->swapChain_.resource[DirectXSetup::GetInstance()->backBufferIndex_].Get();
	//遷移前(現在)のResourceState
	barrier.Transition.StateBefore = beforeState;
	//遷移後のResourceState
	barrier.Transition.StateAfter = afterState;
	//TransitionBarrierを張る
	DirectXSetup::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);

}


//FPS固定初期化
void Elysia::DirectXSetup::InitializeFPS() {
	//現在時間を記録する
	//初期化前の時間を記録
	//std::chrono::steady_clock...逆行しないタイマー
	DirectXSetup::GetInstance()->frameEndTime_ = std::chrono::steady_clock::now();

	
}

#pragma endregion

void Elysia::DirectXSetup::FirstInitialize() {
	//出力ウィンドウへの文字出力
	OutputDebugStringA("Hello,DirectX!\n");

	//FPSの初期化
	InitializeFPS();

	//DXGIFactoryを生成
	GenerateDXGIFactory();

	//最適なアダプターを選択
	SelectAdapter();

	//デバイスを生成
	GenerateD3D12Device();

	//エラー・警告、即ち停止
	StopErrorWarning();
	
	//コマンドリストを作る
	GenerateCommand();

	//スワップチェーンを生成する
	GenerateSwapChain();


	//Resource...DirectX12が管理しているGPU上のメモリであり、このデータのこと
	//View...Resourceに対してどのような処理を行うのか手順をまとめたもの

	//Descriptor...view(作業方法)の情報を格納している場所
	//DescriptorHeap...Descriptorを束ねたもの


	//流れ
	//1.DescriptorHeapを生成する
	//2.swapChainからResourceを引っ張ってくる
	//3.引っ張ってきたResourceに対してDescriptor上にRTVを作る

	////DescriptorHeap(RTV用)を生成する
	GenarateDescriptorHeap();

	//スワップチェーンを引っ張ってくる
	PullResourcesFromSwapChain();

}

void Elysia::DirectXSetup::SecondInitialize() {

	//名前を設定
	DirectXSetup::GetInstance()->swapChainName_[0] = "SwapChainNumber1";
	DirectXSetup::GetInstance()->swapChainName_[1] = "SwapChainNumber2";

	//ハンドルの計算
	DirectXSetup::GetInstance()->rtvHandle_[0] = RtvManager::GetInstance()->Allocate(DirectXSetup::GetInstance()->swapChainName_[0]);
	DirectXSetup::GetInstance()->rtvHandle_[1] = RtvManager::GetInstance()->Allocate(DirectXSetup::GetInstance()->swapChainName_[1]);
	
	//スワップチェーン1枚目
	RtvManager::GetInstance()->GenarateRenderTargetView(DirectXSetup::GetInstance()->GetSwapChain().resource[0].Get(), DirectXSetup::GetInstance()->rtvHandle_[0]);
	//スワップチェーン2枚目
	RtvManager::GetInstance()->GenarateRenderTargetView(DirectXSetup::GetInstance()->GetSwapChain().resource[1].Get(), DirectXSetup::GetInstance()->rtvHandle_[1]);


	//フェンスを生成
	GenarateFence();

}



void Elysia::DirectXSetup::UpdateFPS() {
	//1/60秒ピッタリの時間
	//1フレームの時間
	const std::chrono::microseconds MIN_TIME(uint64_t(1000000.0f / 60.0f));
	
	//1/60秒よりわずかに短い時間
	const std::chrono::microseconds MIN_CHECK_TIME(uint64_t(1000000.0f / 65.0f));



	//現在時間を取得する
	//VSync待ちが完了した時点での時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	//前回記録からの経過時間を取得する
	//現在時間から前回の時間を引くことで前回からの経過時間を取得する
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - DirectXSetup::GetInstance()->frameEndTime_);

	//前回から1/60秒経つまで待機する
	//1/60秒(よりわずかに短い時間)経っていない場合
	if (elapsed < MIN_CHECK_TIME) {
		//1/60秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - DirectXSetup::GetInstance()->frameEndTime_ < MIN_TIME) {
			//1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	//次のフレームの計算に使うため、待機完了後の時間を記録しておく
	//現在の時間を記録する
	DirectXSetup::GetInstance()->frameEndTime_ = std::chrono::steady_clock::now();

}



IDxcBlob* Elysia::DirectXSetup::CompileShader(const std::wstring& filePath, const wchar_t* profile){
	//DXCの初期化
	//dxcCompilerを初期化
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));

	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	//現時点でincludeはしないが、includeに対応
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));

	//1.hlslファイルを読む
	windowsSetup_->OutPutStringA(Convert::Text::ToString(std::format(L"Begin CompileShader,path:{},profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	//2.Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(),
		L"-E",L"main",
		L"-T",profile,
		L"-Zi",L"-Qembed_debug",
		L"-Od",
		L"-Zpr",
	};


	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler_->Compile(&shaderSourceBuffer, arguments, _countof(arguments), includeHandler_.Get(), IID_PPV_ARGS(&shaderResult));
	//コンパイルエラーではなくdxcが起動出来ないなど致命的な状況
	assert(SUCCEEDED(hr));


	//3.警告・エラーが出ていないかを確認する
	//警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		windowsSetup_->OutPutStringA(shaderError->GetStringPointer());
		assert(false);
	}


	//4.Compile結果を受け取って返す
	//BLOB・・・BinaryLargeOBject
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	windowsSetup_->OutPutStringA(Convert::Text::ToString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;

}

void Elysia::DirectXSetup::StartDraw() {

	////コマンドをキックする
	//コマンドを積む・・・CommandListに処理を追加していくこと
	//キックする・・・CommandQueueCommandListを渡してGPUの実行を開始すること
	//画面をクリアするためのコマンドを積み、キックし、メインループを完成させる



	//処理の内容
	//1.BackBufferを決定する
	//2.書き込む作業(画面のクリア)をしたいのでRTVを設定する
	//3.画面のクリアを行う
	//4.CommandListを閉じる
	//5.CommandListの実行(キック)
	//6.画面のスワップ(BackBufferとFrontBufferを入れ替える)
	//7.次のフレーム用にCommandListを再準備

	
	
	//スワップチェーンのバリアを張る
	//TransitionBarrierの設定
	//今回のバリアはTransition
	//D3D12_RESOURCE_STATE_PRESENT...見せるモード
	//D3D12_RESOURCE_STATE_RENDER_TARGET...RTV使う用
	SetResourceBarrierForSwapChain(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);


	//描画先のRTVとDSVを設定する
	//描画先のRTVを設定する
	DirectXSetup::GetInstance()->GetCommandList()->OMSetRenderTargets(1, &RtvManager::GetInstance()->GetRtvHandle(backBufferIndex_), false, &DirectXSetup::GetInstance()->dsvHandle_);
	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.0f,0.0f,0.0f,1.0f };
	DirectXSetup::GetInstance()->GetCommandList()->ClearRenderTargetView(RtvManager::GetInstance()->GetRtvHandle(backBufferIndex_), clearColor, 0, nullptr);


	uint32_t width = WindowsSetup::GetInstance()->GetClientWidth();
	uint32_t height = WindowsSetup::GetInstance()->GetClientHeight();

	//ビューポートの生成
	GenarateViewport(width, height);
	
	//シザーを生成
	GenarateScissor(width, height);
	
}





void Elysia::DirectXSetup::EndDraw() {
	////画面表示出来るようにする
	//ここがflameの最後
	//画面に描く処理は「全て終わり」、画面に映すので、状態を遷移
	//今回はRenderTargetからPresentにする
	
	SetResourceBarrierForSwapChain(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);


	//コマンドリストの内容を確定させる。全てのコマンドを積んでからCloseすること
	HRESULT hr = DirectXSetup::GetInstance()->GetCommandList()->Close();
	assert(SUCCEEDED(hr));

	//コマンドをキックする
	//GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { DirectXSetup::GetInstance()->GetCommandList().Get()};
	DirectXSetup::GetInstance()->commandQueue_->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うよう通知する

	DirectXSetup::GetInstance()->swapChain_.swapChain->Present(1u, 0u);

	////GPUにSignalを送る
	//GPUの実行完了が目的
	//1.GPUに実行が完了したタイミングでFEnceに指定した値を書き込んでもらう
	//  GPUに対してSignalを発行する
	//	Signal・・・GPUの指定の場所までたどり着いたら、指定の値を書き込んでもらうお願いのこと
	//2.CPUではFenceに指定した値が書き込まれているかを確認する
	//3.指定した値が書き込まれていない場合は、書き込まれるまで待つ
	//Fenceの値を更新
	fenceValue_++;
	//GPUがここまでたどりついた時に、Fenceの値を代入するようSignalを送る
	DirectXSetup::GetInstance()->commandQueue_->Signal(DirectXSetup::GetInstance()->fence_.Get(), fenceValue_);
	

	//Fenceの値が指定したSignal値にたどりついているか確認する
	//GetCompletedValueの初期値はFence作成時に渡した初期値
	if (DirectXSetup::GetInstance()->fence_->GetCompletedValue() < fenceValue_) {
		//指定したSignalにたどりついていないので、たどり着くまで待つようにイベントを設定する
		DirectXSetup::GetInstance()->fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		//イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
	
	//FPSの更新
	UpdateFPS();

	//リセット
	hr = DirectXSetup::GetInstance()->commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = DirectXSetup::GetInstance()->commandList_->Reset(DirectXSetup::GetInstance()->commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

void Elysia::DirectXSetup::Release() {

	//解放処理
	CloseHandle(fenceEvent_);
}




