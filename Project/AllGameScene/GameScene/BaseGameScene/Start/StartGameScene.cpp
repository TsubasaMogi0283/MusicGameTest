#include "StartGameScene.h"

#include <imgui.h>

#include "Input.h"
#include "TextureManager.h"
#include "LevelDataManager.h"
#include "Audio.h"
#include "GameScene/GameScene.h"
#include "GameScene/BaseGameScene/Explanation/ExplanationGameScene.h"

StartGameScene::StartGameScene(){
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


void StartGameScene::Initialize(){

	//黒フェード
	//黒画像読み込み
	uint32_t blackTextureHandle = textureManager_->Load("Resources/Sprite/Back/Black.png");
	//生成
	blackFadeSprite_.reset(Elysia::Sprite::Create(blackTextureHandle, INITIAL_SPRITE_POSITION_));
	//初期の透明度を設定
	fadeTransparency_ = 1.0f;
	blackFadeSprite_->SetTransparency(fadeTransparency_);
}

void StartGameScene::Update(GameScene* gameScene){

	//環境音の音量の設定
	enviromentAudioVolume_+=VOLUME_INCREASE_VALUE_;
	gameScene->SetEnviromentAudioVolume(enviromentAudioVolume_);

	//透明度の設定
	//徐々に透明になっていく
	fadeTransparency_ -= FADE_VALUE_;
	blackFadeSprite_->SetTransparency(fadeTransparency_);

	if (enviromentAudioVolume_ >= MAX_VOLUME_&& fadeTransparency_<=0.0f) {
		fadeTransparency_ = MIN_VOLUME_;
		enviromentAudioVolume_ = MAX_VOLUME_;
		//説明シーンへ
		gameScene->ChangeDetailScene(std::make_unique<ExplanationGameScene>());
		return;
	}

#ifdef _DEBUG
	//ImGui表示用
	DisplayImGui();
#endif // _DEBUG
}


void StartGameScene::DrawSprite(){
	blackFadeSprite_->Draw();
}

void StartGameScene::DisplayImGui(){
	ImGui::Begin("開始(ゲーム)");
	ImGui::InputFloat("環境音の音量", &enviromentAudioVolume_);
	ImGui::InputFloat("フェードの透明度", &fadeTransparency_);
	ImGui::End();

}
