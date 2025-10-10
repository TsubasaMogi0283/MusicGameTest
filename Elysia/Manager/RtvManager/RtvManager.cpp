#include "RtvManager.h"

#include "WindowsSetup.h"
#include "Vector4.h"

D3D12_CPU_DESCRIPTOR_HANDLE Elysia::RtvManager::rtvHandles_[RtvManager::RTV_DESCRIPTOR_SIZE_] = {};

Elysia::RtvManager* Elysia::RtvManager::GetInstance(){
    static RtvManager instance;
    return &instance;
}


ComPtr<ID3D12Resource> Elysia::RtvManager::CreateRenderTextureResource(const DXGI_FORMAT& format, const Vector4& clearColor) {

	uint32_t width = WindowsSetup::GetInstance()->GetClientWidth();
	uint32_t height= WindowsSetup::GetInstance()->GetClientHeight();

	D3D12_RESOURCE_DESC resourceDesc{};
	//Textureの幅
	resourceDesc.Width = width;
	//Textureの高さ
	resourceDesc.Height = height;
	//mipmapの数
	resourceDesc.MipLevels = 1u;
	//奥行 or 配列Textureの配列数
	resourceDesc.DepthOrArraySize = 1u;
	//利用可能なフォーマット
	resourceDesc.Format = format;
	//サンプリングカウント。1固定
	resourceDesc.SampleDesc.Count = 1u;
	//2次元
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//RenderTargetとして利用可能にする
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	//resourceDescとclearValueにあるFormatはしっかり統一させてね


	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	//VRAM上に作る
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//クリア設定
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;


	//Resourceの作成
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = DirectXSetup::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProperties,					//Heapの設定 
		D3D12_HEAP_FLAG_NONE,				//Heapの特殊な設定。特になし。
		&resourceDesc,						//Resourceの設定
		D3D12_RESOURCE_STATE_RENDER_TARGET,	//これから描画することを前提としたTextureなのでRenderTargetとして使うことから始める
		&clearValue,						//Clear最適値。ClearRenderTargetをこの色でClearするようにする。最適化されているので高速！
		IID_PPV_ARGS(&resource));			//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));


	return resource;
}

ComPtr<ID3D12Resource>  Elysia::RtvManager::CreateRenderTextureResourceForDepth(const DXGI_FORMAT& format, const Vector4& clearColor){

	uint32_t width = WindowsSetup::GetInstance()->GetClientWidth();
	uint32_t height = WindowsSetup::GetInstance()->GetClientHeight();

	D3D12_RESOURCE_DESC resourceDesc{};
	//Textureの幅
	resourceDesc.Width = width;
	//Textureの高さ
	resourceDesc.Height = height;
	//mipmapの数
	resourceDesc.MipLevels = 1;
	//奥行 or 配列Textureの配列数
	resourceDesc.DepthOrArraySize = 1;
	//利用可能なフォーマット
	resourceDesc.Format = format;
	//サンプリングカウント。1固定
	resourceDesc.SampleDesc.Count = 1;
	//2次元
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//RenderTargetとして利用可能にする
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	//resourceDescとclearValueにあるFormatはしっかり統一させてね


	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	//VRAM上に作る
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;




	//クリア設定
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;


	//Resourceの作成
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = DirectXSetup::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProperties,					//Heapの設定 
		D3D12_HEAP_FLAG_NONE,				//Heapの特殊な設定。特になし。
		&resourceDesc,						//Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//これから描画することを前提としたTextureなのでRenderTargetとして使うことから始める
		&clearValue,						//Clear最適値。ClearRenderTargetをこの色でClearするようにする。最適化されているので高速！
		IID_PPV_ARGS(&resource));			//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));


	return resource;
}


uint32_t  Elysia::RtvManager::Allocate(const std::string& name){
	//上限だったらasset
	assert(index_ < RTV_DESCRIPTOR_SIZE_);

	//既存だったらindexを返す
	for (uint32_t i = 0u; i < RTV_DESCRIPTOR_SIZE_; ++i) {
		if (RtvManager::GetInstance()->rtvInformation_[i].name == name) {
			uint32_t index = RtvManager::GetInstance()->rtvInformation_[i].index;
			return index;
		}

	}

	
	
	//return する番号を一旦記録しておく
	int index = index_;
	
	//次のために番号を1進める
	++index_;

	RtvManager::GetInstance()->rtvInformation_[index].name = name;
	RtvManager::GetInstance()->rtvInformation_[index].index =index;



	//上で記録した番号をreturn
	return index;


}

void  Elysia::RtvManager::GenarateRenderTargetView(const ComPtr<ID3D12Resource>& resource,const uint32_t& handle){
	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;				//出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	
	
	//handleはAllowcateで返された値を使ってね

	//0の時だけ少し違うので専用のものを作成
	if (handle == 0) {
		//ディスクリプタの先頭を取得する
		D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle;
		rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
		rtvHandles_[0] = rtvStartHandle;
	}
	else {
		//ハンドルを計算
		rtvHandles_[handle].ptr = rtvHandles_[handle - 1].ptr + DirectXSetup::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);;

	}

	
	//RTVの作成(本体)
	DirectXSetup::GetInstance()->GetDevice()->CreateRenderTargetView(
		resource.Get(), &rtvDesc, rtvHandles_[handle]);


}




void Elysia::RtvManager::Initialize(){
	//Resourceに対して作業を行うにはViewが必要
	//Viewは作業方法
	
	//DescriptorHeapを作る
	rtvDescriptorHeap_ = DirectXSetup::GetInstance()->GenarateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV, RTV_DESCRIPTOR_SIZE_, false);


	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//出力結果をSRGBに変換して書き込む
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	
	//2dテクスチャとして書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;			
}



