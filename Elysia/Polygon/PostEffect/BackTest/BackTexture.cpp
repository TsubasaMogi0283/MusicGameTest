#include "BackTexture.h"
#include "imgui.h"

#include "WindowsSetup.h"
#include "PipelineManager.h"
#include "TextureManager.h"
#include "SrvManager.h"
#include "RtvManager.h"

Elysia::BackTexture::BackTexture() {
	//ウィンドウクラスの取得
	windowsSetup_ = Elysia::WindowsSetup::GetInstance();
	//DirectXクラスの取得
	directXSetup_= Elysia::DirectXSetup::GetInstance();
	//RTV管理クラスの取得
	rtvManager_ = Elysia::RtvManager::GetInstance();
	//SRV管理クラスの取得
	srvManager_ = Elysia::SrvManager::GetInstance();
	//パイプライン管理クラスの取得
	pipelineManager_ = Elysia::PipelineManager::GetInstance();
}

void Elysia::BackTexture::Initialize(){

	//エフェクトの種類を設定
	effectType_ = NoneEffect;
	//Effect
	effectResource_ = directXSetup_->CreateBufferResource(sizeof(int32_t));
	
	//Vignette
	vignetteResource_ = directXSetup_->CreateBufferResource(sizeof(VignetteInformation));
	vignetteInformation_.pow = 0.8f;
	vignetteInformation_.scale = 16.0f;

	//GaussianFilter
	gaussianFilterResource_ = directXSetup_->CreateBufferResource(sizeof(GaussianFilterInformation));
	gaussianFilterInformation_.sigma = 2.0f;


	//リソース作成
	rtvResource_ = rtvManager_->CreateRenderTextureResource(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, color_);
	//ハンドルの取得
	rtvHandle_= rtvManager_->Allocate("BackText");
	//RTV作成
	rtvManager_->GenarateRenderTargetView(rtvResource_, rtvHandle_);


	//SRV
	srvHandle_ = srvManager_->Allocate();
	srvManager_->CreateSRVForRenderTexture(rtvResource_.Get(), srvHandle_);

}

void Elysia::BackTexture::PreDraw(){
	
	//RTの設定
	const float RENDER_TARGET_CLEAR_VALUE[] = { color_.x,color_.y,color_.z,color_.w };
	directXSetup_->GetCommandList()->OMSetRenderTargets(
		1, &rtvManager_->GetRtvHandle(rtvHandle_), false, &directXSetup_->GetDsvHandle());

	//RTVのクリア
	directXSetup_->GetCommandList()->ClearRenderTargetView(
		rtvManager_->GetRtvHandle(rtvHandle_), RENDER_TARGET_CLEAR_VALUE, 0u, nullptr);

	//Depthのクリア
	directXSetup_->GetCommandList()->ClearDepthStencilView(
		directXSetup_->GetDsvHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0u, 0u, nullptr);

	//クライアントサイズの取得
	uint32_t width = windowsSetup_->GetClientWidth();
	uint32_t height = windowsSetup_->GetClientHeight();

	//ビューポート
	directXSetup_->GenarateViewport(width,height);
	
	//シザー矩形 
	directXSetup_->GenarateScissor(width, height);
	
}

void Elysia::BackTexture::Draw(){

	//ResourceBarrierを張る
	directXSetup_->SetResourceBarrier(
		rtvResource_.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);


#ifdef _DEBUG
	ImGui::Begin("Effect");
	ImGui::SliderInt("Type",&effectType_,0,7);
	ImGui::End();
#endif

	//書き込むためのアドレスを取得
	//reinterpret_cast...char* から int* へ、One_class* から Unrelated_class* へなどの変換に使用
	effectResource_->Map(0u, nullptr, reinterpret_cast<void**>(&effectTypeData_));
	//選択したEffectTypeを書き込み
	*effectTypeData_ = effectType_;
	effectResource_->Unmap(0u, nullptr);

	//パイプラインの設定
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetFullScreenRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetFullScreenGraphicsPipelineState().Get());
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Effect
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(0u, effectResource_->GetGPUVirtualAddress());
	//Vignette
	if (effectType_ == VignetteEffect) {
		directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(1u, vignetteResource_->GetGPUVirtualAddress());
	}
	//Texture
	TextureManager::GetInstance()->GraphicsCommand(2u, srvHandle_);
	//GaussianFilter
	if (effectType_ == GaussianFilter3x3a ||
		effectType_ == GaussianFilter5x5a) {
		directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(3u, gaussianFilterResource_->GetGPUVirtualAddress());
	}
	//描画(DrawCall)３頂点で１つのインスタンス。
	directXSetup_->GetCommandList()->DrawInstanced(3u, 1u, 0u, 0u);

	//ResourceBarrierを張る
	directXSetup_->SetResourceBarrier(
		rtvResource_.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

}
