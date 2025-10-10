#include "SrvManager.h"


Elysia::SrvManager* Elysia::SrvManager::GetInstance(){
	static SrvManager instance;
	return &instance;
}

void Elysia::SrvManager::Initialize(){
	//デスクリプタヒープの生成
	descriptorHeap_ = DirectXSetup::GetInstance()->GenarateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, MAX_SRV_COUNT_, true);

	//SRV...ShaderResourceView
	//デスクリプタ1個分のサイズを取得して記録
	descriptorSize_ = DirectXSetup::GetInstance()->GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


}

uint32_t Elysia::SrvManager::Allocate(){

	//上限だったらasset
	assert(useIndex_ < MAX_SRV_COUNT_);

	//return する番号を一旦記録しておく
	int index = useIndex_;

	//次のために番号を1進める
	useIndex_++;
	
	//上で記録した番号をreturn
	return index;;
}



void Elysia::SrvManager::CreateSRVForTexture2D(const uint32_t& srvIndex, ID3D12Resource* resource, const  DXGI_FORMAT& format, const UINT& mipLevels, const bool& isCubeMap) {
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//キューブマップの場合
	if (isCubeMap == true) {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		//unionがTextureCubeになったが、内部パラメータの意味はTexture2Dと変わらない
		srvDesc.TextureCube.MostDetailedMip = 0u;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}
	else {
		//2Dテクスチャ
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(mipLevels);

	}

	//SRVを作る
	DirectXSetup::GetInstance()->GetDevice()->CreateShaderResourceView(resource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
	
}

void Elysia::SrvManager::CreateSRVForStructuredBuffer(const uint32_t& srvIndex, ID3D12Resource* resource, const UINT& numElements, const UINT& structureByteStride){
	
	//SRVの設定
	D3D12_BUFFER_SRV bufferSRV = {
		.FirstElement = 0u,
		.NumElements = numElements,
		.StructureByteStride = structureByteStride,
		.Flags = D3D12_BUFFER_SRV_FLAG_NONE,
	};

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc={
		.Format = DXGI_FORMAT_UNKNOWN,
		.ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		.Buffer = bufferSRV,
	
	};
	
	//SRVの作成
	DirectXSetup::GetInstance()->GetDevice()->CreateShaderResourceView(
		resource, &srvDesc, GetCPUDescriptorHandle(srvIndex));

}

void Elysia::SrvManager::CreateSRVForRenderTexture(ID3D12Resource* resource,const uint32_t& handle){

	//SRVの設定
	D3D12_TEX2D_SRV tex2DRV = {
		.MipLevels = 1u,
	};
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc={
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		.Texture2D = tex2DRV,
	};
	
	//SRVの生成
	DirectXSetup::GetInstance()->GetDevice()->CreateShaderResourceView(
		resource, &srvDesc, GetCPUDescriptorHandle(handle));

}

void Elysia::SrvManager::CreateSRVForDepthTexture(const uint32_t& handle){

	//情報を設定
	D3D12_TEX2D_SRV tex2DRV = {
		.MipLevels = 1u,
	};
	D3D12_SHADER_RESOURCE_VIEW_DESC depthTextureSrvDesc={
		.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		.Texture2D = tex2DRV,
	
	};
	
	//リソースを持ってくる
	ComPtr<ID3D12Resource> resource = DirectXSetup::GetInstance()->GetDepthStencilResource();
	//SRVの生成
	DirectXSetup::GetInstance()->GetDevice()->CreateShaderResourceView(resource.Get(), &depthTextureSrvDesc, GetCPUDescriptorHandle(handle));


}

void Elysia::SrvManager::CreateSRVForPalette(const UINT& numElements, const UINT& structureByteStride, ID3D12Resource* resource, const uint32_t& handle){
	
	
	//SRVの設定
	D3D12_BUFFER_SRV bufferSRV = {
		.FirstElement = 0u,
		.NumElements = numElements,
		.StructureByteStride = structureByteStride,
		.Flags = D3D12_BUFFER_SRV_FLAG_NONE,
	};


	//palette用のSRV情報を設定
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc={
		.Format = DXGI_FORMAT_UNKNOWN,
		.ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		.Buffer = bufferSRV,

	};

	//SRVを作る
	Elysia::DirectXSetup::GetInstance()->GetDevice()->CreateShaderResourceView(
		resource, &paletteSrvDesc, GetCPUDescriptorHandle(handle));

}

void Elysia::SrvManager::PreDraw() {
	//コマンドを積む
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap_.Get() };
	DirectXSetup::GetInstance()->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

}

void Elysia::SrvManager::SetGraphicsRootDescriptorTable(const UINT& rootParameterIndex,const uint32_t& srvIndex){
	DirectXSetup::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(
		rootParameterIndex,
		GetGPUDescriptorHandle(srvIndex));
}
