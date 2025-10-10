#include "Door.h"

#include <numbers>
#include <imgui.h>

#include <Input.h>
#include <LevelDataManager.h>
#include <TextureManager.h>
#include <SingleCalculation.h>
#include <VectorCalculation.h>
#include <Player/Player.h>

Door::Door(){
	//入力クラス
	input_ = Elysia::Input::GetInstance();
	//レベルエディタ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
}

void Door::Initialize() {
	//空の場合停止
	assert(player_);

	//テクスチャの読み込み
	uint32_t textureHandle = Elysia::TextureManager::GetInstance()->Load("Resources/Sprite/Open/Open.png");

	//生成
	openSptrite_ .reset(Elysia::Sprite::Create(textureHandle, INITIAL_POSITION));
	//最初は非表示
	openSptrite_->SetInvisible(true);

	//初期回転を取得
	initialRotateY_ = levelDataManager_->GetInitialRotate(levelDataHandle_, "Door").y;
	//最大の回転の値を設定
	maxRotateY_ = initialRotateY_ + (std::numbers::pi_v<float_t>*3.0f) / 4.0f;
	//初期回転
	rotateY_ = initialRotateY_;

	//コリジョンの座標の初期化
	Vector3 collisionInitialPosition= levelDataManager_->GetInitialTranslate(levelDataHandle_, DOOR_COLLISION_STRING_);
	levelDataManager_->SetTranslate(levelDataHandle_, DOOR_COLLISION_STRING_, collisionInitialPosition);

	//開ける音の読み込み
	openSEHandle_ = Elysia::Audio::GetInstance()->Load("Resources/Audio/Action/OpenDoor.mp3");
}


void Door::Update() {
	//距離を計算
	Vector3 doorDefference = VectorCalculation::Subtract(player_->GetWorldPosition(), levelDataManager_->GetInitialTranslate(levelDataHandle_, DOOR_STRING_));
	distance_ = SingleCalculation::Length(doorDefference);
	if (distance_ < MAX_DISTANCE_ && isOpen_ == false) {
		//表示
		openSptrite_->SetInvisible(false);

		//スペース化Bボタンで開ける
		if (input_->IsTriggerKey(DIK_SPACE) == true || input_->IsTriggerButton(XINPUT_GAMEPAD_B) == true) {
			//開ける音を鳴らす
			Elysia::Audio::GetInstance()->Play(openSEHandle_, false);
			isOpen_ = true;
		}
	}
	else {
		//非表示
		openSptrite_->SetInvisible(true);
	}

	//開いた状態
	if (isOpen_ == true) {
		//非表示
		openSptrite_->SetInvisible(true);

		//線形補間で開ける
		rotateT_ += ROTATE_VALUE_;
		rotateT_ = std::clamp(rotateT_, MIN_T_VALUE_, MAX_T_VALUE_);
		rotateY_ = SingleCalculation::Lerp(initialRotateY_, maxRotateY_, rotateT_);

		//線形補間が終わったらコリジョンを下げる
		if (rotateT_ >= MAX_T_VALUE_) {
			levelDataManager_->SetTranslate(levelDataHandle_, DOOR_COLLISION_STRING_, noCollisionPosition_);

		}
	}

	//設定
	levelDataManager_->SetRotate(levelDataHandle_, DOOR_STRING_, { .x = 0.0f,.y = rotateY_,.z = 0.0f });

#ifdef _DEBUG
	ImGui::Begin("ドア");
	ImGui::InputFloat("距離", &distance_);
	ImGui::End();
#endif // _DEBUG


}


void Door::DrawSprite() {
	openSptrite_->Draw();
}