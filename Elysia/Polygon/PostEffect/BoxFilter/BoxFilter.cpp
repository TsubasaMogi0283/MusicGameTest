#include "BoxFilter.h"

#include <imgui.h>

#include "WindowsSetup.h"
#include "PipelineManager.h"
#include "SrvManager.h"
#include "RtvManager.h"

Elysia::BoxFilter::BoxFilter(){
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

void Elysia::BoxFilter::Initialize(){

	//リソース
	boxFilterTypeResource_ = directXSetup_->CreateBufferResource(sizeof(BoxFilterType));
	//型の設定
	boxFilterType_ = BoxFilter3x3;
	
	//RTV
	//リソースを生成
	rtvResource_ = rtvManager_->CreateRenderTextureResource(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, color_);
	//ハンドルの取得
	rtvHandle_= rtvManager_->Allocate("BoxFilter");
	//生成
	rtvManager_->GenarateRenderTargetView(rtvResource_, rtvHandle_);

	//SRV
	//ハンドルの取得
	srvHandle_ = srvManager_->Allocate();
	//生成
	srvManager_->CreateSRVForRenderTexture(rtvResource_.Get(), srvHandle_);

}

void Elysia::BoxFilter::PreDraw(){
	
	//RT
	const float RENDER_TARGET_CLEAR_VALUE[] = {color_.x, color_.y, color_.z, color_.w};
	directXSetup_->GetCommandList()->OMSetRenderTargets(
		1, &rtvManager_->GetRtvHandle(rtvHandle_), false, &directXSetup_->GetDsvHandle());
	//クリア
	directXSetup_->GetCommandList()->ClearRenderTargetView(
		rtvManager_->GetRtvHandle(rtvHandle_), RENDER_TARGET_CLEAR_VALUE, 0u, nullptr);
	//DSV
	directXSetup_->GetCommandList()->ClearDepthStencilView(
		directXSetup_->GetDsvHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0u, 0u, nullptr);

	//縦横サイズ
	uint32_t width = windowSetup_->GetClientWidth();
	uint32_t height = windowSetup_->GetClientHeight();

	//ビューポート
	directXSetup_->GenarateViewport(width,height);
	//シザー矩形 
	directXSetup_->GenarateScissor(width, height);
	
}

void Elysia::BoxFilter::Draw(){

	//ResourceBarrierを張る
	directXSetup_->SetResourceBarrier(
		rtvResource_.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	//BoxFilterの設定
	boxFilterTypeResource_->Map(0u, nullptr, reinterpret_cast<void**>(&boxFilterTypeData_));
	boxFilterTypeData_->type=boxFilterType_;
	boxFilterTypeResource_->Unmap(0u, nullptr);


	//パイプラインの設定
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetBoxFilterRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetBoxFilterGraphicsPipelineState().Get());
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//SRV
	srvManager_->SetGraphicsRootDescriptorTable(0u, srvHandle_);
	//Type
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(1u, boxFilterTypeResource_->GetGPUVirtualAddress());
	//描画(DrawCall)３頂点で１つのインスタンス。
	directXSetup_->GetCommandList()->DrawInstanced(3u, 1u, 0u, 0u);

	//ResourceBarrierを張る
	directXSetup_->SetResourceBarrier(
		rtvResource_.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

}
