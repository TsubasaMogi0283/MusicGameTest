#include "WaitingTitleScene.h"

#include <imgui.h>

#include "Input.h"
#include "TextureManager.h"
#include "LevelDataManager.h"
#include "Audio.h"

#include "TitleScene/TitleScene.h"
#include "TitleScene/BaseTitleScene/Noise/NoiseTitleScene.h"

WaitingTitleScene::WaitingTitleScene(){
	//インスタンスの取得
	//テクスチャ管理クラス
	textureManager_ = Elysia::TextureManager::GetInstance();
	//入力クラス
	input_ = Elysia::Input::GetInstance();
	//レベルエディタ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
	//オーディオ
	audio_ = Elysia::Audio::GetInstance();
}

void WaitingTitleScene::Initialize(){

	//テクスチャハンドルの取得
	//テキスト
	uint32_t textTextureHandle = textureManager_->Load("Resources/Sprite/Title/StartText.png");
	//タイトルテクスチャ
	uint32_t logoTextureHandle = textureManager_->Load("Resources/Sprite/Title/TitleNormal.png");
	
	//スプライトの生成
	//テキスト
	textSprite_.reset(Elysia::Sprite::Create(textTextureHandle, SPRITE_INITIAL_POSITION_));
	//ロゴ
	logoSprite_.reset(Elysia::Sprite::Create(logoTextureHandle, SPRITE_INITIAL_POSITION_));

	//平行光源の初期化
	directionalLight_.intensity = 0.5f;
	directionalLight_.color = { .x = 1.0f,.y = 0.22f,.z = 0.0f,.w = 1.0f };
	directionalLight_.direction = { .x = 0.91f,.y = -1.0f,.z = 0.0f };

	//決定音
	desideSEHandle_ = audio_->Load("Resources/Audio/SE/Deside.wav");
	audio_->ChangeVolume(desideSEHandle_, desideSEVolume_);
}

void WaitingTitleScene::Update(TitleScene* titleScene){
	//まだボタンを押していない時
	//通常点滅
	if (isFlash_ == true) {
		
		//時間の加算
		flashTime_ += INCREASE_VALUE_;

		//表示
		if (flashTime_ > RESTART_TIME &&
			flashTime_ <= FLASH_TIME_LIMIT_ * 1u) {
			textSprite_->SetInvisible(false);
		}
		//非表示
		if (flashTime_ > FLASH_TIME_LIMIT_ * 1u &&
			flashTime_ <= FLASH_TIME_LIMIT_ * 2u) {
			textSprite_->SetInvisible(true);

		}
		//循環
		if (flashTime_ > FLASH_TIME_LIMIT_ * 2u) {
			flashTime_ = RESTART_TIME;
		}


		//スペースをまたはBボタンを押したら高速点滅
		if (input_->IsPushKey(DIK_SPACE) == true || input_->IsTriggerButton(XINPUT_GAMEPAD_B) == true) {

			//決定音の再生
			audio_->Play(desideSEHandle_, false);

			//高速点滅へ
			isFastFlash_ = true;
			isFlash_ = false;
		}


	}

	//高速点滅
	if (isFastFlash_ == true) {
		//時間の加算
		fastFlashTime_ += INCREASE_VALUE_;
		if (fastFlashTime_ % FAST_FLASH_TIME_INTERVAL_ == INCREASE_COUNT_TIME) {
			++textDisplayCount_;
		}
		//表示
		if (textDisplayCount_ % FLASH_INTERVAL_ == DISPLAY_TIME_) {
			textSprite_->SetInvisible(true);
		}
		//非表示
		else {
			textSprite_->SetInvisible(false);
		}

		//指定した時間を超えたらノイズシーンへ
		if (fastFlashTime_ > FAST_FLASH_TIME_LIMIT_) {
			//ノイズへ
			titleScene->ChangeDetailScene(std::make_unique<NoiseTitleScene>());
			return;
		}
	}

#ifdef _DEBUG
	//ImGui表示用
	DisplayImGui();
#endif // _DEBUG

	

}


void WaitingTitleScene::DrawSprite(){
	//ロゴ
	logoSprite_->Draw();
	//テキスト
	textSprite_->Draw();
}

void WaitingTitleScene::DisplayImGui(){
	//intに変換
	int newFlashTime = static_cast<int32_t>(flashTime_);

	ImGui::Begin("待ち(タイトルシーン)");
	ImGui::Checkbox("通常点滅", &isFlash_);
	ImGui::Checkbox("高速点滅", &isFastFlash_);
	ImGui::InputInt("点滅時間", &newFlashTime);
	if (ImGui::TreeNode("平行光源") == true) {
		ImGui::SliderFloat("強さ", &directionalLight_.intensity, 0.0f, 10.0f);
		ImGui::SliderFloat3("方向", &directionalLight_.direction.x, -1.0f, 1.0f);
		ImGui::TreePop();
	}

	ImGui::End();


}
