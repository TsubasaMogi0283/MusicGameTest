#include "SkyBox.h"

#include "VectorCalculation.h"
#include "WorldTransform.h"
#include "Camera.h"

#include "DirectXSetup.h"
#include "PipelineManager.h"
#include "TextureManager.h"


Elysia::SkyBox::SkyBox(){
	//DirectX
	directXSetup_ = Elysia::DirectXSetup::GetInstance();
	//パイプライン管理クラス
	pipelineManager_ = Elysia::PipelineManager::GetInstance();
}

//初期化
void Elysia::SkyBox::Create() {

	//頂点リソース
	vertexResource_ = DirectXSetup::GetInstance()->CreateBufferResource(sizeof(VertexData) * SURFACE_VERTEX_* SURFACE_AMOUNT_);

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点３つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * SURFACE_VERTEX_* SURFACE_AMOUNT_;
	//１頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//書き込み用のアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));


	//外から見て反時計回り
	//右面
	vertexData_[0].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData_[1].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData_[2].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData_[3].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData_[4].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData_[5].position = { 1.0f,-1.0f,-1.0f,1.0f };

	//左面
	vertexData_[6].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData_[7].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData_[8].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[9].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData_[10].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[11].position = { -1.0f,1.0f,1.0f,1.0f };

	//前面
	vertexData_[12].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData_[13].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData_[14].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData_[15].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData_[16].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData_[17].position = { 1.0f,1.0f,1.0f,1.0f };

	//後面
	vertexData_[18].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData_[19].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData_[20].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[21].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData_[22].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[23].position = { 1.0f,-1.0f,-1.0f,1.0f };

	//上面
	vertexData_[24].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData_[25].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData_[26].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData_[27].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData_[28].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData_[29].position = { 1.0f,1.0f,-1.0f,1.0f };

	//下面
	vertexData_[30].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData_[31].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData_[32].position = { 1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[33].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData_[34].position = { 1.0f,-1.0f,-1.0f,1.0f };
	vertexData_[35].position = { -1.0f,-1.0f,-1.0f,1.0f };

	vertexResource_->Unmap(0, nullptr);

	////マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_= DirectXSetup::GetInstance()->CreateBufferResource(sizeof(SkyBoxMaterial));

}



void Elysia::SkyBox::Draw(const uint32_t& texturehandle, const WorldTransform& worldTransform, const Camera& camera) {

	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	//reinterpret_cast...char* から int* へ、One_class* から Unrelated_class* へなどの変換に使用
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = {.x = 1.0f,.y = 1.0f,.z = 1.0f,.w = 1.0f};
	materialData_->uvTransform = Matrix4x4Calculation::MakeIdentity4x4();
	materialResource_->Unmap(0u, nullptr);
	
	//パイプラインの設定
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetSkyBoxRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetSkyBoxGraphicsPipelineState().Get());
	
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXSetup_->GetCommandList()->IASetVertexBuffers(0u, 1u, &vertexBufferView_);
	//ワールドトランスフォーム
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(0u, worldTransform.resource->GetGPUVirtualAddress());
	//カメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(1u, camera.resource->GetGPUVirtualAddress());
	//テクスチャ
	if (texturehandle != 0u) {
		TextureManager::GetInstance()->GraphicsCommand(2u, texturehandle);
	}
	//マテリアル
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(3u, materialResource_->GetGPUVirtualAddress());
	//描画
	directXSetup_->GetCommandList()->DrawInstanced(SURFACE_VERTEX_ * SURFACE_AMOUNT_, 1u, 0u, 0u);
	
}

