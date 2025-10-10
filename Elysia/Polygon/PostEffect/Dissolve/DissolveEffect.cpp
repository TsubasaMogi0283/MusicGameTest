#include "DissolveEffect.h"
#include <imgui.h>

#include"WindowsSetup.h"
#include "PipelineManager.h"
#include "SrvManager.h"
#include "RtvManager.h"
#include "Dissolve.h"

Elysia::DissolvePostEffect::DissolvePostEffect(){
	//ウィンドウクラスの取得
	windowSetup_ = Elysia::WindowsSetup::GetInstance();
	//DirectXクラスの取得
	directXSetup_ = Elysia::DirectXSetup::GetInstance();
	//パイプライン管理クラスの取得
	pipelineManager_ = Elysia::PipelineManager::GetInstance();
	//RTV管理クラスの取得
	rtvManager_ = Elysia::RtvManager::GetInstance();
	//SRV管理クラスの取得
	srvManager_ = Elysia::SrvManager::GetInstance();
}

void Elysia::DissolvePostEffect::Initialize(const Vector4& clearColor){
	
	
	renderTargetClearColor_ = clearColor;
	//リソースの作成
	rtvResource_ = rtvManager_->CreateRenderTextureResource(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, renderTargetClearColor_);
	//ハンドルの取得
	rtvHandle_ = rtvManager_->Allocate("Dissolve");
	//作成
	rtvManager_->GenarateRenderTargetView(rtvResource_, rtvHandle_);

	//SRV
	//ハンドルの取得
	srvHandle_ = srvManager_->Allocate();
	//作成
	srvManager_->CreateSRVForRenderTexture(rtvResource_.Get(), srvHandle_);

}

void Elysia::DissolvePostEffect::PreDraw(){
	
	const float CLEAR_COLOR[] = { renderTargetClearColor_.x,renderTargetClearColor_.y,renderTargetClearColor_.z,renderTargetClearColor_.w };
	//RT
	directXSetup_->GetCommandList()->OMSetRenderTargets(
		1u, &rtvManager_->GetRtvHandle(rtvHandle_), false, &directXSetup_->GetDsvHandle());
	//クリア
	directXSetup_->GetCommandList()->ClearRenderTargetView(
		rtvManager_->GetRtvHandle(rtvHandle_), CLEAR_COLOR, 0u, nullptr);
	//DSV
	directXSetup_->GetCommandList()->ClearDepthStencilView(
		directXSetup_->GetDsvHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0u, 0u, nullptr);

	//縦横のサイズ
	uint32_t width = windowSetup_->GetClientWidth();
	uint32_t height = windowSetup_->GetClientHeight();

	//ビューポート
	directXSetup_->GenarateViewport(width, height);
	//シザー矩形 
	directXSetup_->GenarateScissor(width, height);


}

void Elysia::DissolvePostEffect::Draw(const Dissolve& dissolve){

	//ResourceBarrierを張る
	directXSetup_->SetResourceBarrier(
		rtvResource_,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);


	//パイプラインの設定
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetDissolveRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetDissolveGraphicsPipelineState().Get());
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//SRV
	srvManager_->SetGraphicsRootDescriptorTable(0u, srvHandle_);
	//マスクテクスチャ
	srvManager_->SetGraphicsRootDescriptorTable(1u, dissolve.maskTextureHandle);
	//ディゾルブ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(2u, dissolve.resource->GetGPUVirtualAddress());
	//描画(DrawCall)３頂点で１つのインスタンス。
	directXSetup_->GetCommandList()->DrawInstanced(3u, 1u, 0u, 0u);
	
	//ResourceBarrierを張る
	directXSetup_->SetResourceBarrier(
		rtvResource_,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);


}
