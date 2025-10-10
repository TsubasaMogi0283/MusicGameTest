#include "GaussianFilter.h"

#include <imgui.h>

#include "WindowsSetup.h"
#include "PipelineManager.h"
#include "TextureManager.h"
#include "SrvManager.h"
#include "RtvManager.h"



Elysia::GaussianFilter::GaussianFilter(){
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

void Elysia::GaussianFilter::Initialize(){

	//Effect
	//リソースの生成
	boxFilterTypeResource_ = directXSetup_->CreateBufferResource(sizeof(GaussianFilterData));
	sigma_ = 2.0f;
	boxFilterType_ = GaussianFilter3x3;


	//RTV
	const Vector4 RENDER_TARGET_CLEAR_VALUE = { 1.0f,0.0f,0.0f,1.0f };
	//リソース
	rtvResource_ = rtvManager_->CreateRenderTextureResource(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, RENDER_TARGET_CLEAR_VALUE);
	//ハンドル
	rtvHandle_= rtvManager_->Allocate("GaussianFilter");
	//生成
	rtvManager_->GenarateRenderTargetView(rtvResource_, rtvHandle_);


	//SRV
	//ハンドル
	srvHandle_ = srvManager_->Allocate();
	//生成
	srvManager_->CreateSRVForRenderTexture(rtvResource_.Get(), srvHandle_);
}

void Elysia::GaussianFilter::PreDraw(){
	
	const float RENDER_TARGET_CLEAR_VALUE[] = { 1.0f,0.0f,0.0f,1.0f };
	directXSetup_->GetCommandList()->OMSetRenderTargets(
		1, &rtvManager_->GetRtvHandle(rtvHandle_), false, &directXSetup_->GetDsvHandle());

	directXSetup_->GetCommandList()->ClearRenderTargetView(
		rtvManager_->GetRtvHandle(rtvHandle_), RENDER_TARGET_CLEAR_VALUE, 0, nullptr);


	directXSetup_->GetCommandList()->ClearDepthStencilView(
		directXSetup_->GetDsvHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//縦横のサイズ
	uint32_t width = windowSetup_->GetClientWidth();
	uint32_t height = windowSetup_->GetClientHeight();

	//ビューポート
	directXSetup_->GenarateViewport(width,height);
	//シザー矩形 
	directXSetup_->GenarateScissor(width, height);
	
}

void Elysia::GaussianFilter::Draw(){

	//ResourceBarrierを張る
	directXSetup_->SetResourceBarrier(
		rtvResource_.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);


	boxFilterTypeResource_->Map(0u, nullptr, reinterpret_cast<void**>(&boxFilterTypeData_));
	boxFilterTypeData_->sigma = sigma_;
	boxFilterTypeData_->type = boxFilterType_;
	boxFilterTypeResource_->Unmap(0u, nullptr);

	//パイプラインの設定
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetGaussianFilterRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetGaussianFilterGraphicsPipelineState().Get());

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//SRV
	srvManager_->SetGraphicsRootDescriptorTable(0u, srvHandle_);
	//Effect
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(1u, boxFilterTypeResource_->GetGPUVirtualAddress());
	//描画(DrawCall)３頂点で１つのインスタンス。
	directXSetup_->GetCommandList()->DrawInstanced(3u, 1u, 0u, 0u);

	//ResourceBarrierを張る
	directXSetup_->SetResourceBarrier(
		rtvResource_.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

}
