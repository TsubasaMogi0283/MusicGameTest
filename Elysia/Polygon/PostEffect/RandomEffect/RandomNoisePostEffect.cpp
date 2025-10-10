#include "RandomNoisePostEffect.h"

#include <imgui.h>

#include "WindowsSetup.h"
#include "PipelineManager.h"
#include "TextureManager.h"
#include "SrvManager.h"
#include "RtvManager.h"
#include "RandomNoise.h"

Elysia::RandomNoisePostEffect::RandomNoisePostEffect(){
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

void Elysia::RandomNoisePostEffect::Initialize() {

	//RTV
	const Vector4 RENDER_TARGET_CLEAR_VALUE = {.x = 0.0f,.y = 0.0f,.z = 0.0f,.w = 1.0f };
	//リソース
	rtvResource_ = rtvManager_->CreateRenderTextureResource(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, RENDER_TARGET_CLEAR_VALUE);
	//ハンドルの取得
	rtvHandle_ = rtvManager_->Allocate("RandomNoisePostEffect");
	//生成
	rtvManager_->GenarateRenderTargetView(rtvResource_, rtvHandle_);

	//SRV
	//ハンドル
	srvHandle_ = srvManager_->Allocate();
	//生成
	srvManager_->CreateSRVForRenderTexture(rtvResource_.Get(), srvHandle_);
}

void Elysia::RandomNoisePostEffect::PreDraw() {
	
	//RTVの設定
	const float_t RENDER_TARGET_CLEAR_VALUE[] = { 0.0f,0.0f,0.0f,1.0f };
	directXSetup_->GetCommandList()->OMSetRenderTargets(
		1u, &rtvManager_->GetRtvHandle(rtvHandle_), false, &directXSetup_->GetDsvHandle());
	//クリア
	directXSetup_->GetCommandList()->ClearRenderTargetView(
		rtvManager_->GetRtvHandle(rtvHandle_), {RENDER_TARGET_CLEAR_VALUE}, 0u, nullptr);
	//Depth
	directXSetup_->GetCommandList()->ClearDepthStencilView(
		directXSetup_->GetDsvHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0u, 0u, nullptr);
	//縦横のサイズを取得
	uint32_t width = windowSetup_->GetClientWidth();
	uint32_t height = windowSetup_->GetClientHeight();

	//ビューポート
	directXSetup_->GenarateViewport(width, height);
	//シザー矩形 
	directXSetup_->GenarateScissor(width, height);

}

void Elysia::RandomNoisePostEffect::Draw(const RandomNoise& randomNoise) {

	//ResourceBarrierを張る
	directXSetup_->SetResourceBarrier(
		rtvResource_,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);


	//パイプラインの設定
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetRandomEffectRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetRandomEffectGraphicsPipelineState().Get());
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//SRV
	srvManager_->SetGraphicsRootDescriptorTable(0u, srvHandle_);
	//ランダム
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(1u, randomNoise.resource->GetGPUVirtualAddress());
	//描画(DrawCall)３頂点で１つのインスタンス。
	directXSetup_->GetCommandList()->DrawInstanced(3u, 1u, 0u, 0u);
	
	//ResourceBarrierを張る
	directXSetup_->SetResourceBarrier(
		rtvResource_,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
}
