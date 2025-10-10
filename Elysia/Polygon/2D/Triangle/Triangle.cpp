#include "Triangle.h"

#include <vector>

#include "WindowsSetup.h"
#include "DirectXSetup.h"
#include "TextureManager.h"
#include "Transform.h"




Triangle::Triangle(){

	//ウィンドウクラスの取得
	windowsSetup_ = Elysia::WindowsSetup::GetInstance();
	//DirectXクラスの取得
	directXSetup_ = Elysia::DirectXSetup::GetInstance();

}

void Triangle::Initialize() {

	//ここでBufferResourceを作る
	//頂点を6に増やす
	vertexResouce_ = directXSetup_->CreateBufferResource(sizeof(VertexData) * 6);
	////マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_= directXSetup_->CreateBufferResource(sizeof(MaterialData));

	

	//WVP用のリソースを作る。Matrix4x4　1つ分のサイズを用意する
	wvpResource_ = directXSetup_-> CreateBufferResource(sizeof(TransformationMatrix));

	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResouce_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点３つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	//１頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	


}






//描画
void Triangle::Draw(const Transform& transform, const Vector4& color) {

	//TextureCoordinate(テクスチャ座標系)
	//TexCoord,UV座標系とも呼ばれている

	//左上が(0,0)右下が(1,1)で画像全体を指定することが出来る
	//U(x)V(y)

	//書き込むためのアドレスを取得
	vertexResouce_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//左下
	vertexData_[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData_[0].texCoord = { 0.0f,1.0f };
	//上
	vertexData_[1].position = {0.0f,0.5f,0.0f,1.0f};
	vertexData_[1].texCoord = { 0.5f,0.0f };
	//右下
	vertexData_[2].position = {0.5f,-0.5f,0.0f,1.0f} ;
	vertexData_[2].texCoord = { 1.0f,1.0f };
	//範囲外は危険だよ！！


	//左下2
	vertexData_[3].position = {-0.5f,-0.5f,0.5f,1.0f};
	vertexData_[3].texCoord = { 0.0f,1.0f };
	//上2
	vertexData_[4].position = {0.0f,0.0f,0.0f,1.0f};
	vertexData_[4].texCoord = { 0.5f,0.0f };
	//右下2
	vertexData_[5].position = {0.5f,-0.5f,-0.5f,1.0f} ;
	vertexData_[5].texCoord = { 1.0f,1.0f };

	vertexResouce_->Unmap(0u, nullptr);

	//マテリアルにデータを書き込む
	

	//書き込むためのアドレスを取得
	//reinterpret_cast...char* から int* へ、One_class* から Unrelated_class* へなどの変換に使用
	materialResource_->Map(0u, nullptr, reinterpret_cast<void**>(&materialData_));

	materialData_->color = color;
	materialData_->lightingKinds = 0;
	materialData_->uvTransform = Matrix4x4Calculation::MakeIdentity4x4();
	
	materialResource_->Unmap(0u, nullptr);
	
	//サイズに注意を払ってね！！！！！
	//どれだけのサイズが必要なのか考えよう

	//新しく引数作った方が良いかも
	Matrix4x4 worldMatrix = Matrix4x4Calculation::MakeAffineMatrix(transform.scale,transform.rotate,transform.translate);
	//遠視投影行列
	Matrix4x4 viewMatrixSprite = Matrix4x4Calculation::MakeIdentity4x4();
	
	Matrix4x4 projectionMatrixSprite = Matrix4x4Calculation::MakeOrthographicMatrix(0.0f, 0.0f, float(windowsSetup_->GetClientWidth()), float(windowsSetup_->GetClientHeight()), 0.0f, 100.0f);
	

	//WVP行列を作成
	Matrix4x4 worldViewProjectionMatrix = Matrix4x4Calculation::Multiply(worldMatrix, Matrix4x4Calculation::Multiply(viewMatrixSprite, projectionMatrixSprite));

	//書き込む為のアドレスを取得
	wvpResource_->Map(0u, nullptr, reinterpret_cast<void**>(&wvpData_));
	
	
	
	//さっき作ったworldMatrixの情報をここに入れる
	wvpData_->WVP = worldViewProjectionMatrix;
	wvpData_->World = Matrix4x4Calculation::MakeIdentity4x4();
	

	wvpResource_->Unmap(0u, nullptr);

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXSetup_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//マテリアルCBufferの場所を設定
	//ここでの[0]はregisterの0ではないよ。rootParameter配列の0番目
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(0u, materialResource_->GetGPUVirtualAddress());
	//CBVを設定する
	//wvp用のCBufferの場所を指定
	//今回はRootParameter[1]に対してCBVの設定を行っている
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(1u, wvpResource_->GetGPUVirtualAddress());


	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	if (textureHandle_ != 0) {
		Elysia::TextureManager::GetInstance()->GraphicsCommand(2u,textureHandle_);
	}

	//描画(DrawCall)３頂点で１つのインスタンス。
	directXSetup_->GetCommandList()->DrawInstanced(6u, 1u, 0u, 0u);

}

