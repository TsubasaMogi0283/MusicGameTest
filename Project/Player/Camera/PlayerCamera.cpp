#include "PlayerCamera.h"

#include <numbers>
#include <imgui.h>
#include <random>

#include "GlobalVariables.h"
#include "VectorCalculation.h"
#include "Matrix4x4Calculation.h"
#include "Easing.h"

PlayerEyeCamera::PlayerEyeCamera(){
	//インスタンスの取得
	//グローバル変数
	globalVariables_ = Elysia::GlobalVariables::GetInstance();
}

void PlayerEyeCamera::Initialize(){

	//カメラの調整項目
	globalVariables_->CreateGroup(GAME_SCENE_CAMERA_NAME_);
	globalVariables_->AddItem(GAME_SCENE_CAMERA_NAME_, HEIGHT_OFFSET_, cameraPositionOffset_);

	//カメラ座標のオフセットの初期化
	cameraPositionOffset_ = { .x = 0.0f,.y = 1.2f,.z = 0.0f };
}

void PlayerEyeCamera::Update(){

	//位置の計算
	worldTransform_.translate = VectorCalculation::Add(playerPosition_, cameraPositionOffset_);
	
	//振動
	Shake();

	//カメラの回転の計算
	worldTransform_.rotate.x = phi_;
	worldTransform_.rotate.y = -(theta_)+std::numbers::pi_v<float_t> / 2.0f;
	worldTransform_.rotate.z = 0.0f;


	//ワールド行列を計算
	worldTransform_.worldMatrix = Matrix4x4Calculation::MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
	//ビュー行列の計算
	camera_.viewMatrix = Matrix4x4Calculation::Inverse(worldTransform_.worldMatrix);



}

void PlayerEyeCamera::Shake(){
	
	//振動するとき
	if (isShake_ == true) {

		//振動演出
		//体力減っていくと同時に振動幅が大きくなっていくよ
		std::random_device seedGenerator;
		std::mt19937 randomEngine(seedGenerator());
		std::uniform_real_distribution<float_t> distribute(-0.05f, 0.05f);


		//現在の座標に加える
		shakeValue_.x = distribute(randomEngine);
		shakeValue_.y = distribute(randomEngine);
		shakeValue_.z = distribute(randomEngine);

		//割合の計算
		const float_t SHAKE_VALUE = 0.05f;
		shakeRatio_ += SHAKE_VALUE;
		float_t newRatio = Easing::EaseInOutQuart(shakeRatio_);
		Vector3 newShakeValue = VectorCalculation::Multiply(shakeValue_, (1.0f- newRatio));

		//振動分を加算
		worldTransform_.translate = VectorCalculation::Add(worldTransform_.translate, newShakeValue);
		//振動が終わったらリセットする
		if (newRatio>=1.0f) {
			isShake_ = false;
		}

	}
	else {
		shakeRatio_ = 0.0f;
	}


}
