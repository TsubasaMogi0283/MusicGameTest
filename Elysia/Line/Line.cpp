#include "Line.h"

#include "Matrix4x4Calculation.h"
#include "PipelineManager.h"
#include "Camera.h"

Elysia::Line::Line(){
	//DirectXのインスタンスを取得
	directXSetup_ = Elysia::DirectXSetup::GetInstance();
	//パイプライン管理クラスのインスタンスを取得
	pipelineManager_ = Elysia::PipelineManager::GetInstance();
}

void Elysia::Line::Initialize() {

	//ここでBufferResourceを作る
	//頂点を6に増やす
	vertexResouce_ = directXSetup_->CreateBufferResource(sizeof(LineVertexData) * 2);
	////マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = directXSetup_->CreateBufferResource(sizeof(Vector4));

	color_ = {.x = 1.0f,.y = 1.0f,.z = 1.0f,.w = 1.0f };

	//WVP用のリソースを作る。Matrix4x4　1つ分のサイズを用意する
	wvpResource_ = DirectXSetup::GetInstance()->CreateBufferResource(sizeof(LineTransformMatrix));

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResouce_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点３つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(LineVertexData) * 2;
	//１頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(LineVertexData);


}




//描画
void Elysia::Line::Draw(const Vector3& start, const Vector3& end, const Camera& camera) {

	
	//データ
	LineVertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResouce_->Map(0u, nullptr, reinterpret_cast<void**>(&vertexData));
	//始点
	vertexData[0].position = { start.x,start.y,start.z,1.0f };
	//終点
	vertexData[1].position = { end.x,end.y,end.z,1.0f };
	vertexResouce_->Unmap(0u, nullptr);

	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	//reinterpret_cast...char* から int* へ、One_class* から Unrelated_class* へなどの変換に使用
	Vector4* materialData_ = nullptr;
	materialResource_->Map(0u, nullptr, reinterpret_cast<void**>(&materialData_));
	*materialData_ = color_;
	materialResource_->Unmap(0u, nullptr);

	Vector3 scale = {.x = 1.0f,.y = 1.0f,.z = 1.0f };
	Vector3 rotate = {.x = 0.0f,.y = 0.0f,.z = 0.0f };
	Matrix4x4 worldMatrix = Matrix4x4Calculation::MakeAffineMatrix(scale, rotate, start);

	//書き込む為のアドレスを取得
	wvpResource_->Map(0u, nullptr, reinterpret_cast<void**>(&wvpData_));
	//さっき作ったworldMatrixの情報をここに入れる
	wvpData_->WVP = Matrix4x4Calculation::MakeIdentity4x4();
	wvpData_->World = worldMatrix;
	wvpResource_->Unmap(0u, nullptr);


	//パイプラインの設定
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetLineRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetLineGraphicsPipelineState().Get());


	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXSetup_->GetCommandList()->IASetVertexBuffers(0u, 1u, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	//CBVを設定する
	//wvp用のCBufferの場所を指定
	//今回はRootParameter[1]に対してCBVの設定を行っている
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(0u, wvpResource_->GetGPUVirtualAddress());
	//カメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(1u, camera.resource->GetGPUVirtualAddress());
	//マテリア
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(2u, materialResource_->GetGPUVirtualAddress());

	//描画(DrawCall)6頂点で１つのインスタンス。
	directXSetup_->GetCommandList()->DrawInstanced(2u, 1u, 0u, 0u);

}

