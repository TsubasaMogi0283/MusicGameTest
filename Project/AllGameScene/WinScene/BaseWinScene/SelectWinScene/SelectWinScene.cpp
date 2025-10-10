#include "SelectWinScene.h"

#include <cmath>
#include <numbers>
#include <imgui.h>

#include "Input.h"
#include "LevelDataManager.h"
#include "TextureManager.h"
#include "GlobalVariables.h"
#include "Audio.h"

#include "WinScene/WinScene.h"
#include "WinScene/BaseWinScene/ReturnTitleWinScene/ReturnTitleWinScene.h"

SelectWinScene::SelectWinScene() {
	//インスタンスの取得
	//入力クラス
	input_ = Elysia::Input::GetInstance();
	//レベルデータ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
	//テクスチャ管理クラス
	textureManager_ = Elysia::TextureManager::GetInstance();
	//グローバル変数クラス
	globalVariables_ = Elysia::GlobalVariables::GetInstance();
	//オーディオ
	audio_ = Elysia::Audio::GetInstance();

}


void SelectWinScene::Initialize(){

	//テキスト(タイトルへ)
	uint32_t textHandle = textureManager_->Load("Resources/Sprite/Result/Win/WinText.png");
	textSprite_.reset(Elysia::Sprite::Create(textHandle, INITIAL_SPRITE_POSITION_));

	//決定音
	desideSEHandle_ = audio_->Load("Resources/Audio/SE/Deside.wav");
	
}

void SelectWinScene::Update(WinScene* winScene){
	
	//まだ決定していない時
	if (isDeside_ == false) {
		//通常の点滅
		flashTime_ += DELTA_TIME_;
		//表示
		if (flashTime_ > RESTART_TIME_ &&
			flashTime_ <= FLASH_TIME_LIMIT_) {
			textSprite_->SetInvisible(false);
		}
		//非表示
		else if (flashTime_ > FLASH_TIME_LIMIT_ &&
			flashTime_ <= FLASH_END_TIME_) {
			textSprite_->SetInvisible(true);
		}
		//循環
		else if (flashTime_ > FLASH_END_TIME_) {
			flashTime_ = RESTART_TIME_;
		}

		//次のシーンへ
		//Bボタンまたはスペースを押したとき
		if (input_->IsTriggerButton(XINPUT_GAMEPAD_B) == true|| input_->IsTriggerKey(DIK_SPACE) == true) {
			isDeside_ = true;
			//決定音の再生
			audio_->Play(desideSEHandle_, false);
			audio_->ChangeVolume(desideSEHandle_, desideSEVolume_);
		}
	}
	else {
		//時間を足していく
		//除算するときuintではないと上手く加算できなかったので
		//fastFlashTime_はuintにしている
		fastFlashTime_ += FAST_FLASH_DELTA_TIME_;
		if (fastFlashTime_%FAST_FLASH_TIME_INTERVAL_ == INCREASE_COUNT_TIME_) {
			++textDisplayCount_;
		}
		//表示
		if (textDisplayCount_ % FLASH_INTERVAL_ == DISPLAY_) {
			textSprite_->SetInvisible(true);
		}
		//非表示
		else {
			textSprite_->SetInvisible(false);
		}

		//指定した時間を超えたら戻る演出へ
		if (textDisplayCount_ > MAX_DISPLAY_) {
			winScene->ChangeDetailScene(std::make_unique<ReturnTitleWinScene>());
			return;
		}
	}

#ifdef _DEBUG
	//ImGui表示用
	DisplayImGui();
#endif // _DEBUG
}

void SelectWinScene::DrawSprite(){
	//テキストの描画
	textSprite_->Draw();
}

void SelectWinScene::DisplayImGui(){

	//intに直す
	int32_t newTextDisplayCount = static_cast<int32_t>(textDisplayCount_);
	int32_t newFastFlashTime =static_cast<int32_t>(fastFlashTime_);

	ImGui::Begin("選択(勝利シーン)");
	ImGui::InputInt("表示回数", &newTextDisplayCount);
	ImGui::InputInt("高速点滅時間", &newFastFlashTime);
	ImGui::End();
}
