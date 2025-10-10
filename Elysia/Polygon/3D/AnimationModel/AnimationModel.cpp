#include "AnimationModel.h"

#include <numbers>


#include "SrvManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "PipelineManager.h"

#include "WorldTransform.h"
#include "Material.h"
#include "Matrix4x4Calculation.h"
#include "SkinCluster.h"
#include "Camera.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "DirectionalLight.h"


Elysia::AnimationModel::AnimationModel(){
	//DirectXのクラスを取得
	directXSetup_ = Elysia::DirectXSetup::GetInstance();
	//パイプライン管理クラスを取得
	pipelineManager_ = Elysia::PipelineManager::GetInstance();
	//SRV管理クラスを取得
	srvManager_ = Elysia::SrvManager::GetInstance();
	//テクスチャ管理クラスを取得
	textureManager_ = Elysia::TextureManager::GetInstance();
	//モデル管理クラスを取得
	modelManager_ = Elysia::ModelManager::GetInstance();

}

Elysia::AnimationModel* Elysia::AnimationModel::Create(const uint32_t& modelHandle){
	//新たなModel型のインスタンスのメモリを確保
	Elysia::AnimationModel* model = new AnimationModel();

	//テクスチャの読み込み
	model->textureHandle_ = model->textureManager_->Load(Elysia::ModelManager::GetInstance()->GetModelData(modelHandle).textureFilePath);
	//Drawでも使いたいので取り入れる
	model->modelHandle_ = modelHandle;

	//モデルデータ
	model->modelData_ = model->modelManager_->GetModelData(modelHandle);

	//頂点
	//リソースを作る
	model->vertexResource_ = model->directXSetup_->CreateBufferResource(sizeof(VertexData) * Elysia::ModelManager::GetInstance()->GetModelData(modelHandle).vertices.size()).Get(); 
	//リソースの先頭のアドレスから使う
	model->vertexBufferView_.BufferLocation = model->vertexResource_->GetGPUVirtualAddress();
	//使用するリソースは頂点のサイズ
	model->vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * model->modelData_.vertices.size());
	//１頂点あたりのサイズ
	model->vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//インデックス
	//ソースの作成
	model->indexResource_ = model->directXSetup_->CreateBufferResource(sizeof(uint32_t) * Elysia::ModelManager::GetInstance()->GetModelData(modelHandle).indices.size()).Get();
	//リソースの先頭のアドレスから使う
	model->indexBufferView_.BufferLocation = model->indexResource_->GetGPUVirtualAddress();
	//サイズ
	model->indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * model->modelData_.indices.size());
	//フォーマット
	model->indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//カメラ
	model->cameraResource_ = model->directXSetup_->CreateBufferResource(sizeof(CameraForGPU)).Get();

	return model;

	
}

void Elysia::AnimationModel::Draw(const WorldTransform& worldTransform, const Camera& camera, const SkinCluster& skinCluster, const Material& material, const DirectionalLight& directionalLight){
	//Materialのライティングの設定が平行光源ではない場合止める
	assert(material.lightingKinds == DirectionalLighting);

	//頂点バッファにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
	vertexResource_->Unmap(0u, nullptr);

	//Index
	uint32_t* mappedIndex = nullptr;
	indexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&mappedIndex));
	std::memcpy(mappedIndex, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
	indexResource_->Unmap(0u, nullptr);

	//PixelShaderに送る方のカメラ
	cameraResource_->Map(0u, nullptr, reinterpret_cast<void**>(&cameraForGPU_));
	cameraForGPU_->worldPosition = camera.GetWorldPosition();
	cameraResource_->Unmap(0u, nullptr);


	//コマンドを積む
	//パイプラインの設定
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetAnimationModelRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetAnimationModelGraphicsPipelineState().Get());

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[2] = {
		//VertexDataのVBV
		vertexBufferView_,
		//InfluenceのVBV
		skinCluster.influenceBufferView
	};
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXSetup_->GetCommandList()->IASetVertexBuffers(0u, 2u, vertexBufferViews);
	//IBVを設定
	directXSetup_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Material
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(0u, material.resource->GetGPUVirtualAddress());
	//ワールドトランスフォーム
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(1u, worldTransform.resource->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	if (textureHandle_ != 0u) {
		textureManager_->GetInstance()->GraphicsCommand(2u,textureHandle_);
	}
	//DirectionalLight
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(3u, directionalLight.resource->GetGPUVirtualAddress());
	//カメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(4u, camera.resource->GetGPUVirtualAddress());
	//PixelShaderに送る方のカメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(5u, cameraResource_->GetGPUVirtualAddress());
	//paletteSrvHandle
	directXSetup_->GetCommandList()->SetGraphicsRootDescriptorTable(8u, skinCluster.paletteSrvHandle.second);
	//環境マップを使う場合
	if (material.isEnviromentMap==true && eviromentTextureHandle_ != 0u) {
		srvManager_->SetGraphicsRootDescriptorTable(9u, eviromentTextureHandle_);
	}
	//DrawCall
	directXSetup_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1u, 0u, 0u, 0u);

}

void Elysia::AnimationModel::Draw(const WorldTransform& worldTransform, const Camera& camera, const SkinCluster& skinCluster, const Material& material, const PointLight& pointLight){
	
	//Materialのライティングの設定が点光源ではない場合止める
	assert(material.lightingKinds == PointLighting);

	//頂点
	VertexData* vertexData = nullptr;
	vertexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
	vertexResource_->Unmap(0u, nullptr);

	//Index
	uint32_t* mappedIndex = nullptr;
	indexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&mappedIndex));
	std::memcpy(mappedIndex, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
	indexResource_->Unmap(0u, nullptr);


	//PixelShaderに送る方のカメラ
	cameraResource_->Map(0u, nullptr, reinterpret_cast<void**>(&cameraForGPU_));
	cameraForGPU_->worldPosition = camera.GetWorldPosition();
	cameraResource_->Unmap(0u, nullptr);

	//コマンドを積む
	//パイプラインの設定
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetAnimationModelRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetAnimationModelGraphicsPipelineState().Get());

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[2] = {
		//VertexDataのVBV
		vertexBufferView_,
		//InfluenceのVBV
		skinCluster.influenceBufferView
	};

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXSetup_->GetCommandList()->IASetVertexBuffers(0u, 2u, vertexBufferViews);
	//IBVを設定
	directXSetup_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Material
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(0u, material.resource->GetGPUVirtualAddress());
	//資料見返してみたがhlsl(GPU)に計算を任せているわけだった
	//コマンド送ってGPUで計算
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(1u, worldTransform.resource->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	if (textureHandle_ != 0u) {
		textureManager_->GetInstance()->GraphicsCommand(2u, textureHandle_);
	}

	//カメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(4u, camera.resource->GetGPUVirtualAddress());
	//PixelShaderに送る方のカメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(5u, cameraResource_->GetGPUVirtualAddress());
	//PointLight
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(6u, pointLight.resource->GetGPUVirtualAddress());
	//paletteSrvHandle
	directXSetup_->GetCommandList()->SetGraphicsRootDescriptorTable(8u, skinCluster.paletteSrvHandle.second);
	//環境マップを使う場合
	if (material.isEnviromentMap == true && eviromentTextureHandle_ != 0u) {
		srvManager_->SetGraphicsRootDescriptorTable(9u, eviromentTextureHandle_);
	}

	//DrawCall
	directXSetup_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1u, 0u, 0u, 0u);
}

void Elysia::AnimationModel::Draw(const WorldTransform& worldTransform, const Camera& camera, const SkinCluster& skinCluster, const Material& material, const SpotLight& spotLight){
	
	//Materialのライティングの設定がスポットライトではない場合止める
	assert(material.lightingKinds == SpotLighting);
	//頂点
	VertexData * vertexData = nullptr;
	vertexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
	vertexResource_->Unmap(0u, nullptr);

	//Index
	uint32_t* mappedIndex = nullptr;
	indexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&mappedIndex));
	std::memcpy(mappedIndex, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
	indexResource_->Unmap(0u, nullptr);

	//PixelShaderに送る方のカメラ
	cameraResource_->Map(0u, nullptr, reinterpret_cast<void**>(&cameraForGPU_));
	cameraForGPU_->worldPosition = camera.GetWorldPosition();
	cameraResource_->Unmap(0u, nullptr);

	//コマンドを積む
	//パイプラインの設定
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetAnimationModelRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetAnimationModelGraphicsPipelineState().Get());

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[2] = {
		//VertexDataのVBV
		vertexBufferView_,
		//InfluenceのVBV
		skinCluster.influenceBufferView
	};

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXSetup_->GetCommandList()->IASetVertexBuffers(0u, 2u, vertexBufferViews);
	//IBVを設定
	directXSetup_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Material
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(0u, material.resource->GetGPUVirtualAddress());
	//資料見返してみたがhlsl(GPU)に計算を任せているわけだった
	//コマンド送ってGPUで計算
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(1u, worldTransform.resource->GetGPUVirtualAddress());
	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	if (textureHandle_ != 0u) {
		textureManager_->GraphicsCommand(2u, textureHandle_);
	}
	//カメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(4u, camera.resource->GetGPUVirtualAddress());
	//PixelShaderに送る方のカメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(5u, cameraResource_->GetGPUVirtualAddress());
	//SpotLight
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(7u, spotLight.resource->GetGPUVirtualAddress());
	//paletteSrvHandle
	directXSetup_->GetCommandList()->SetGraphicsRootDescriptorTable(8u, skinCluster.paletteSrvHandle.second);
	if (material.isEnviromentMap == true && eviromentTextureHandle_ != 0u) {
		srvManager_->SetGraphicsRootDescriptorTable(9u, eviromentTextureHandle_);
	}

	//DrawCall
	directXSetup_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1u, 0u, 0u, 0u);
}
