#include "Model.h"

#include <numbers>
#include <cassert>

#include "Camera.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "PipelineManager.h"

#include "SrvManager.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Material.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

Elysia::Model::Model() {
	//テクスチャ管理クラスの取得
	textureManager_ = Elysia::TextureManager::GetInstance();
	//モデル管理クラスの取得
	modelmanager_ = Elysia::ModelManager::GetInstance();
	//DirectXクラスの取得
	directXSetup_ = Elysia::DirectXSetup::GetInstance();
	//パイプライン管理クラスの取得
	pipelineManager_ = Elysia::PipelineManager::GetInstance();
	//SRV管理クラスも取得
	srvManager_ = Elysia::SrvManager::GetInstance();
}

Elysia::Model* Elysia::Model::Create(const uint32_t& modelHandle) {

	//生成
	Elysia::Model* model = new Elysia::Model();

	//テクスチャの読み込み
	model->textureHandle_ = model->textureManager_->Load(model->modelmanager_->GetModelData(modelHandle).textureFilePath);
	//モデルデータ
	model->modelData_ = model->modelmanager_->GetModelData(modelHandle);

	//頂点リソースを作る
	model->vertexResource_ = model->directXSetup_->CreateBufferResource(sizeof(VertexData) * model->modelmanager_->GetModelData(modelHandle).vertices.size()).Get();
	//リソースの先頭のアドレスから使う
	model->vertexBufferView_.BufferLocation = model->vertexResource_->GetGPUVirtualAddress();
	//使用するリソースは頂点のサイズ
	model->vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * model->modelData_.vertices.size());
	//１頂点あたりのサイズ
	model->vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//解析したデータを使ってResourceとBufferViewを作成する
	model->indexResource_ = model->directXSetup_->CreateBufferResource(sizeof(uint32_t) * model->modelmanager_->GetModelData(modelHandle).indices.size()).Get();
	//場所
	model->indexBufferView_.BufferLocation = model->indexResource_->GetGPUVirtualAddress();
	//サイズ
	model->indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * model->modelData_.indices.size());
	//フォーマット
	model->indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//カメラ
	model->cameraResource_ = model->directXSetup_->CreateBufferResource(sizeof(CameraForGPU)).Get();

	return model;

}

void Elysia::Model::Draw(const WorldTransform& worldTransform, const Camera& camera, const Material& material){
	//非表示設定がtrueになっていた場合は描画しない
	if (isInvisible_ == true) {
		return;
	}

	//ライティング無しだけ
	assert(material.lightingKinds == LightingType::NoneLighting);


	//頂点バッファにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
	vertexResource_->Unmap(0u, nullptr);

	//インデックス
	uint32_t* index = nullptr;
	indexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&index));
	std::memcpy(index, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
	indexResource_->Unmap(0u, nullptr);

	//PixelShaderに送る方のカメラ
	cameraResource_->Map(0u, nullptr, reinterpret_cast<void**>(&cameraForGPU_));
	cameraForGPU_->worldPosition = camera.GetWorldPosition();
	cameraResource_->Unmap(0u, nullptr);

	//コマンドを積む
	//パイプラインの設定
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetModelRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetModelGraphicsPipelineState().Get());
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXSetup_->GetCommandList()->IASetVertexBuffers(0u, 1u, &vertexBufferView_);
	//IBVを設定
	directXSetup_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Material
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(0u, material.resource->GetGPUVirtualAddress());
	//資料見返してみたがhlsl(GPU)に計算を任せているわけだった
	//コマンド送ってGPUで計算
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(1u, worldTransform.resource->GetGPUVirtualAddress());
	//テクスチャ
	if (textureHandle_ != 0u) {
		textureManager_->GraphicsCommand(2u, textureHandle_);
	}
	//カメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(4u, camera.resource->GetGPUVirtualAddress());
	//PixelShaderに送る方のカメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(5u, cameraResource_->GetGPUVirtualAddress());
	//DrawCall
	directXSetup_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1u, 0u, 0u, 0u);

}

void Elysia::Model::Draw(const WorldTransform& worldTransform, const Camera& camera, const Material& material, const DirectionalLight& directionalLight) {
	//非表示設定がtrueになっていた場合は描画しない
	if (isInvisible_ == true) {
		return;
	}

	//平行光源だけ
	assert(material.lightingKinds == LightingType::DirectionalLighting);


	//頂点バッファにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
	vertexResource_->Unmap(0u, nullptr);

	//インデックス
	uint32_t* index = nullptr;
	indexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&index));
	std::memcpy(index, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
	indexResource_->Unmap(0u, nullptr);

	//PixelShaderに送る方のカメラ
	cameraResource_->Map(0u, nullptr, reinterpret_cast<void**>(&cameraForGPU_));
	cameraForGPU_->worldPosition = camera.GetWorldPosition();
	cameraResource_->Unmap(0u, nullptr);

	//コマンドを積む
	//パイプラインの設定
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetModelRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetModelGraphicsPipelineState().Get());
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXSetup_->GetCommandList()->IASetVertexBuffers(0u, 1u, &vertexBufferView_);
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
	//DirectionalLight
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(3u, directionalLight.resource->GetGPUVirtualAddress());
	//カメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(4u, camera.resource->GetGPUVirtualAddress());
	//PixelShaderに送る方のカメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(5u, cameraResource_->GetGPUVirtualAddress());
	//環境マップ用のテクスチャ
	if (material.isEnviromentMap == true && eviromentTextureHandle_ != 0u) {
		srvManager_->SetGraphicsRootDescriptorTable(8u, eviromentTextureHandle_);
	}
	//DrawCall
	directXSetup_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1u, 0u, 0u, 0u);

}

void Elysia::Model::Draw(const WorldTransform& worldTransform, const Camera& camera, const Material& material, const PointLight& pointLight) {
	//非表示設定がtrueになっていた場合は描画しない
	if (isInvisible_ == true) {
		return;
	}
	
	//点光源だけ
	assert(material.lightingKinds == LightingType::PointLighting);

	//頂点バッファにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
	vertexResource_->Unmap(0u, nullptr);

	//インデックス
	uint32_t* index = nullptr;
	indexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&index));
	std::memcpy(index, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
	indexResource_->Unmap(0u, nullptr);


	//PixelShaderに送る方のカメラ
	cameraResource_->Map(0u, nullptr, reinterpret_cast<void**>(&cameraForGPU_));
	cameraForGPU_->worldPosition = camera.GetWorldPosition();
	cameraResource_->Unmap(0u, nullptr);

	//コマンドを積む
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetModelRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetModelGraphicsPipelineState().Get());
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXSetup_->GetCommandList()->IASetVertexBuffers(0u, 1u, &vertexBufferView_);
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
	//PointLight
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(6u, pointLight.resource->GetGPUVirtualAddress());

	if (material.isEnviromentMap == true && eviromentTextureHandle_ != 0u) {
		srvManager_->SetGraphicsRootDescriptorTable(8u, eviromentTextureHandle_);
	}

	//DrawCall
	directXSetup_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1u, 0u, 0u, 0u);

}

void Elysia::Model::Draw(const WorldTransform& worldTransform, const Camera& camera, const Material& material, const SpotLight& spotLight) {
	//非表示設定がtrueになっていた場合は描画しない
	if (isInvisible_ == true) {
		return;
	}

	//スケール0の時は見えないので早期リターンさせたい
	if (worldTransform.scale.x == 0.0f && 
		worldTransform.scale.y == 0.0f && 
		worldTransform.scale.z == 0.0f) {
		return;
	}

	

	//頂点バッファ
	VertexData* vertexData = nullptr;
	vertexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
	vertexResource_->Unmap(0u, nullptr);

	//インデックス
	uint32_t* index = nullptr;
	indexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&index));
	std::memcpy(index, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
	indexResource_->Unmap(0u, nullptr);

	//PixelShaderに送る方のカメラ
	cameraResource_->Map(0u, nullptr, reinterpret_cast<void**>(&cameraForGPU_));
	cameraForGPU_->worldPosition = camera.GetWorldPosition();
	cameraResource_->Unmap(0u, nullptr);

	//コマンドを積む
	//PSOの設定
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetModelRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetModelGraphicsPipelineState().Get());

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXSetup_->GetCommandList()->IASetVertexBuffers(0u, 1u, &vertexBufferView_);
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

	//環境マッピングの設定
	if (material.isEnviromentMap == true && eviromentTextureHandle_ != 0u) {
		srvManager_->SetGraphicsRootDescriptorTable(8u, eviromentTextureHandle_);
	}

	//DrawCall
	directXSetup_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1u, 0u, 0u, 0u);

}