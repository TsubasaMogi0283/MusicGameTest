#include "TextureManager.h"
#include "SrvManager.h"

#include <vector>

#include "Convert.h"

Elysia::TextureManager* Elysia::TextureManager::GetInstance() {
	static Elysia::TextureManager instance;
	return &instance;
}




const D3D12_RESOURCE_DESC Elysia::TextureManager::GetResourceDesc(const uint32_t& textureHandle) {
	//テクスチャの情報を取得
	// handleからfilePathを取得
	auto it = handleToFilePathMap_.find(textureHandle);
	if (it != handleToFilePathMap_.end()) {
		const std::string& filePath = it->second;
		return textureInformation_[filePath].resource->GetDesc();
	}

	//見つからなかった場合
	return D3D12_RESOURCE_DESC{};
}

uint64_t Elysia::TextureManager::GetTextureWidth(const uint32_t& textureHandle){
	//テクスチャの情報を取得
	//handleからfilePathを取得
	auto it = handleToFilePathMap_.find(textureHandle);
	if (it != handleToFilePathMap_.end()) {
		const std::string& filePath = it->second;
		return textureInformation_[filePath].resource->GetDesc().Width;		
	}
	//見つからなかった場合は0uを返す
	return 0u;
}

uint64_t Elysia::TextureManager::GetTextureHeight(const uint32_t& textureHandle){
	//テクスチャの情報を取得
	//handleからfilePathを取得
	auto it = handleToFilePathMap_.find(textureHandle);
	if (it != handleToFilePathMap_.end()) {
		const std::string& filePath = it->second;
		return textureInformation_[filePath].resource->GetDesc().Height;
	}
	//見つからなかった場合は0uを返す
	return 0u;
}




//統合させた関数
uint32_t Elysia::TextureManager::Load(const std::string& filePath) {

	Elysia::TextureManager* textureManager = TextureManager::GetInstance();

	//一度読み込んだものはその値を返す
	//新規は勿論読み込みをする
	auto it = TextureManager::GetInstance()->textureInformation_.find(filePath);
	if (it != TextureManager::GetInstance()->textureInformation_.end()) {
		return it->second.handle;
	}

	//読み込むたびにインデックスを増やし重複を防ごう
	textureManager->index_ = Elysia::SrvManager::GetInstance()->Allocate();

	//読み込んだデータを保存
	TextureInformation textureInfo;
	textureInfo.handle = textureManager->index_;
	textureInfo.name = filePath;
	textureInfo.mipImages = LoadTextureData(filePath);

	//メタデータの取得
	const DirectX::TexMetadata& metadata = textureInfo.mipImages.GetMetadata();
	//リソースの設定
	textureInfo.resource = CreateTextureResource(metadata);
	textureInfo.internegiateResource = TransferTextureData(
		textureInfo.resource.Get(), textureInfo.mipImages).Get();

	//SRVの生成
	Elysia::SrvManager::GetInstance()->CreateSRVForTexture2D(
		textureInfo.handle,
		textureInfo.resource.Get(),
		metadata.format, UINT(metadata.mipLevels), metadata.IsCubemap());


	// 読み込んだデータをmapに保存
	textureManager->GetTextureInformation().try_emplace(filePath, std::move(textureInfo));
	TextureManager::GetInstance()->handleToFilePathMap_[textureManager->index_] = filePath;

	return textureInfo.handle;
}



#pragma region テクスチャの読み込み
//1.TextureデータそのものをCPUで読み込む
//2.DirectX12のTextureResourceを作る
//3.CPUで書き込む用にUploadHeapのResourceを作る(IntermediateResource)
//4.3に対してCPUでデータを書き込む
//5.CommandListに3を2に転送するコマンドを積む
//6.CommandQueueを使って実行する
//7.6の実行完了を待つ



//CubeMap...箱を構成する6毎のTextureをひとまとめにしたTextureのこと
//さいころみたいなもの


//DSSについて
//DSS...DirectDrawSurface
//png,jpegだとCubemapを使えないがDSSだと出来る!!
//GPUに圧縮したまま送ることが出来るので処理速度が速い




#pragma region 上のLoadTextureにまとめた
//Textureを読み込むためのLoad関数
//1.TextureデータそのものをCPUで読み込む

DirectX::ScratchImage Elysia::TextureManager::LoadTextureData(const std::string& filePath) {

	HRESULT hResult = {};

	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = Convert::Text::ToWString(filePath);
	//dssファイルの場合
	if (filePathW.ends_with(L".dds")) {
		hResult = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		assert(SUCCEEDED(hResult));
	}
	//その他のpngやjpegなど
	else {
		hResult = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hResult));

	}

	//ミップマップの作成
	//ミップマップ...元画像より小さなテクスチャ群
	DirectX::ScratchImage mipImages{};
	//圧縮フォーマットかどうかを調べる
	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		//圧縮フォーマットならそのまま使う
		mipImages = std::move(image);
	}
	else {
		hResult = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 4, mipImages);
		assert(SUCCEEDED(hResult));

	}


	//ミップマップ付きのデータを返す
	return mipImages;
}

//2.DirectX12のTextureResourceを作る

ComPtr<ID3D12Resource> Elysia::TextureManager::CreateTextureResource(const DirectX::TexMetadata& metadata) {
	ComPtr<ID3D12Resource> resource = nullptr;

	//1.metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	//Textureの幅
	resourceDesc.Width = UINT(metadata.width);
	//Textureの高さ
	resourceDesc.Height = UINT(metadata.height);
	//mipmapの数
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	//奥行き or 配列Textureの配列数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	//TextureのFormat
	resourceDesc.Format = metadata.format;
	//サンプリングカウント
	resourceDesc.SampleDesc.Count = 1u;
	//Textureの次元数。普段使っているのは2次元
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	//2.利用するHeapの設定
	//利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	//細かい設定を行う
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	//WriteBackポリシーでCPUアクセス可能
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	//プロセッサの近くに配置
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;


	//3.Resourceを生成する
	HRESULT hResult = Elysia::DirectXSetup::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProperties,					//Heapの設定
		D3D12_HEAP_FLAG_NONE,				//Heapの特殊な設定
		&resourceDesc,						//Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,		//初回のResourceState。データの転送を受け入れられるようにする
		nullptr,							//Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));			//作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hResult));

	return resource;


}

//3.TextureResourceに1で読んだデータを転送する
//書き換え

[[nodiscard]]
ComPtr<ID3D12Resource> Elysia::TextureManager::TransferTextureData(ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages) {

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(Elysia::DirectXSetup::GetInstance()->GetDevice().Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermidiateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	ComPtr<ID3D12Resource> intermediateSizeResource = Elysia::DirectXSetup::GetInstance()->CreateBufferResource(intermidiateSize);
	UpdateSubresources(Elysia::DirectXSetup::GetInstance()->GetCommandList().Get(), texture.Get(), intermediateSizeResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

	//Textureへの転送後は利用できるよう、D312_RESOURCE_STATE_COPY_DESTから
	//D3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	Elysia::DirectXSetup::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);


	return intermediateSizeResource;
}



#pragma endregion



#pragma endregion


void Elysia::TextureManager::GraphicsCommand(const uint32_t& rootParameter, const uint32_t& textureHandle) {
	Elysia::SrvManager::GetInstance()->SetGraphicsRootDescriptorTable(rootParameter, textureHandle);
}

