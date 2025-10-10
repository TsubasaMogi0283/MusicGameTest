#include "WorldTransform.h"
#include "Matrix4x4Calculation.h"

#include "DirectXSetup.h"
#include "Camera.h"
#include "Calculation/QuaternionCalculation.h"

void WorldTransform::Initialize() {
	//リソースの作成
	resource = Elysia::DirectXSetup::GetInstance()->CreateBufferResource(sizeof(WorldTransformData)).Get();

	//初期値
	//スケール
	scale = { .x = 1.0f,.y = 1.0f,.z = 1.0f };
	//回転
	rotate = { .x = 0.0f,.y = 0.0f,.z = 0.0f };
	//座標
	translate = { .x = 0.0f,.y = 0.0f,.z = 0.0f };
}


void WorldTransform::Update() {

	//クォータニオンを使う場合
	if (isUseQuarternion_==true) {
		//Scale
		Matrix4x4 scaleMatrix = Matrix4x4Calculation::MakeScaleMatrix(scale);
		//Rotate。行列へ変換
		Matrix4x4 rotateMatrix = QuaternionCalculation::MakeRotateMatrix(quaternion_);
		//Translate
		Matrix4x4 translateMatrix = Matrix4x4Calculation::MakeTranslateMatrix(translate);

		//合成
		worldMatrix = Matrix4x4Calculation::Multiply(scaleMatrix, Matrix4x4Calculation::Multiply(rotateMatrix, translateMatrix));
	}
	//使わない場合
	else {
		//そのままSRT合成
		worldMatrix = Matrix4x4Calculation::MakeAffineMatrix(scale, rotate, translate);

	}

	//逆転置行列
	//ワールド行列を逆転置にする
	Matrix4x4 worldInverseMatrix = Matrix4x4Calculation::Inverse(worldMatrix);
	
	//転置にした
	worldInverseTransposeMatrix = Matrix4x4Calculation::MakeTransposeMatrix(worldInverseMatrix);

	//親があれば親のワールド行列を掛ける
	if (parent!=nullptr) {
		worldMatrix = Matrix4x4Calculation::Multiply(worldMatrix, parent->worldMatrix);
	}

	//転送
	Transfer();
}


void WorldTransform::Transfer() {

	//書き込み書き込む
	resource->Map(0u, nullptr, reinterpret_cast<void**>(&tranceformationData));
	//ワールド
	tranceformationData->world = worldMatrix;
	//ノーマル
	tranceformationData->normal = Matrix4x4Calculation::MakeIdentity4x4();
	//ワールド逆転置
	tranceformationData->worldInverseTranspose = worldInverseTransposeMatrix;
	//書き込み終了
	resource->Unmap(0u, nullptr);
}
