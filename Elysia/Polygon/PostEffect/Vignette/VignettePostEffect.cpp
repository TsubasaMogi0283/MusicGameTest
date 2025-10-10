#include "VignettePostEffect.h"
#include <imgui.h>

#include "WindowsSetup.h"
#include "Vector4.h"
#include "PipelineManager.h"
#include "TextureManager.h"
#include "SrvManager.h"
#include "RtvManager.h"
#include "Vignette.h"

Elysia::VignettePostEffect::VignettePostEffect(){
	//ウィンドウクラスの取得
	windowsSetup_ = Elysia::WindowsSetup::GetInstance();
	//DirectXクラスの取得
	directXSetup_ = Elysia::DirectXSetup::GetInstance();
	//パイプライン管理クラスの取得
	pipelinemanager_ = Elysia::PipelineManager::GetInstance();
	//RTV管理クラスの取得
	rtvManager_ = Elysia::RtvManager::GetInstance();
	//SRV管理クラスの取得
	srvManager_ = Elysia::SrvManager::GetInstance();;
}

void Elysia::VignettePostEffect::Initialize() {

	//RTV用のリソースを生成
	const Vector4 RENDER_TARGET_CLEAR_VALUE = { .x = 0.0f,.y = 0.0f,.z = 0.0f,.w = 1.0f };
	//リソースの生成
	rtvResource_ = rtvManager_->CreateRenderTextureResource(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, RENDER_TARGET_CLEAR_VALUE);
	//ハンドルの取得
	rtvHandle_ = rtvManager_->Allocate("Vignette");
	//RTVの生成
	rtvManager_->GenarateRenderTargetView(rtvResource_, rtvHandle_);

	//ハンドルの取得
	srvHandle_ = srvManager_->Allocate();
	//SRVの生成
	srvManager_->CreateSRVForRenderTexture(rtvResource_.Get(), srvHandle_);

}

void Elysia::VignettePostEffect::PreDraw() {
	//RT
	const float RENDER_TARGET_CLEAR_VALUE[] = {0.0f, 0.0f,0.0f,1.0f };
	directXSetup_->GetCommandList()->OMSetRenderTargets(
		1u, &rtvManager_->GetRtvHandle(rtvHandle_), false, &directXSetup_->GetDsvHandle());
	//クリア
	directXSetup_->GetCommandList()->ClearRenderTargetView(
		rtvManager_->GetRtvHandle(rtvHandle_), RENDER_TARGET_CLEAR_VALUE, 0u, nullptr);
	//DSV
	directXSetup_->GetCommandList()->ClearDepthStencilView(
		directXSetup_->GetDsvHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0u, 0u, nullptr);

	//縦横
	uint32_t width = windowsSetup_->GetClientWidth();
	uint32_t height = windowsSetup_->GetClientHeight();

	//ビューポート
	directXSetup_->GenarateViewport(width, height);
	//シザー矩形 
	directXSetup_->GenarateScissor(width, height);
}

void Elysia::VignettePostEffect::Draw(const Vignette& vignette) {

	//ResourceBarrierを張る
	directXSetup_->SetResourceBarrier(
		rtvResource_.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	//PSOの設定
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelinemanager_->GetVignetteRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelinemanager_->GetVignetteGraphicsPipelineState().Get());

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//SRV
	srvManager_->SetGraphicsRootDescriptorTable(0u, srvHandle_);
	//数値をGPUへ送る
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(1u, vignette.resource->GetGPUVirtualAddress());
	//描画(DrawCall)３頂点で１つのインスタンス。
	directXSetup_->GetCommandList()->DrawInstanced(3u, 1u, 0u, 0u);
	//ResourceBarrierを張る
	directXSetup_->SetResourceBarrier(
		rtvResource_.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

}

