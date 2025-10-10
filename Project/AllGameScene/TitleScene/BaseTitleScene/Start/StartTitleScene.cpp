#include "StartTitleScene.h"

#include "Input.h"
#include "TextureManager.h"
#include "LevelDataManager.h"
#include "WindowsSetup.h"
#include "Audio.h"

#include "TitleScene/TitleScene.h"
#include "TitleScene/BaseTitleScene/Waiting/WaitingTitleScene.h"

StartTitleScene::StartTitleScene(){
	//インスタンスの取得
	//テクスチャ管理クラス
	textureManager_ = Elysia::TextureManager::GetInstance();
	//入力クラス
	input_ = Elysia::Input::GetInstance();
	//レベルエディタ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
	//ウィンドウクラス
	windowsSetup_ = Elysia::WindowsSetup::GetInstance();
	//オーディオ
	audio_ = Elysia::Audio::GetInstance();
}

void StartTitleScene::Initialize(){

	//黒い円
	uint32_t blackCircleTextureHandle = textureManager_->Load("Resources/Sprite/SceneChange/BlackCircle.png");
	//画面の中心の座標を取りたいのでウィンドウクラスから座標を持ってくる
	const Vector2 CIRCLE_POSITION = { .x = static_cast<float_t>(windowsSetup_->GetClientWidth()) / 2.0f,.y = static_cast<float_t>(windowsSetup_->GetClientHeight()) / 2.0f };
	//アンカーポイント
	const Vector2 CIRCLE_ANCHOR_POINT = { .x = 0.5f,.y = 0.5f };
	
	//生成
	circleSprite_.reset(Elysia::Sprite::Create(blackCircleTextureHandle, CIRCLE_POSITION));
	circleSprite_->SetAnchorPoint(CIRCLE_ANCHOR_POINT);
	//サイズの設定
	circleScaleSize_ = 10.0f;
	circleSprite_->SetScale({.x= circleScaleSize_,.y= circleScaleSize_ });


	//平行光源の初期化
	directionalLight_.intensity = 0.5f;
	directionalLight_.color = { .x = 1.0f,.y = 0.22f,.z = 0.0f,.w = 1.0f };
	directionalLight_.direction = { .x = 0.91f,.y = -1.0f,.z = 0.0f };

}

void StartTitleScene::Update(TitleScene* titleScene){
	//円が小さくなっていく
	circleScaleSize_ -= SIZE_DOWN_VALUE_;
	circleSprite_->SetScale({ .x = circleScaleSize_,.y = circleScaleSize_ });

	//音量の設定
	enviromentAudioVolume_ += VOLUME_INCREASE_VALUE_;
	if (enviromentAudioVolume_ > MAX_VOLUME_) {
		enviromentAudioVolume_ = MAX_VOLUME_;
	}
	audio_->ChangeVolume(titleScene->GetEnviromentAudioHandle(), enviromentAudioVolume_);

	//見えなくなるほど小さくなったら待ちへ
	if (circleScaleSize_ < 0.0f) {
		//もう一度設定する
		circleScaleSize_ = 0.0f;
		circleSprite_->SetScale({ .x = circleScaleSize_,.y = circleScaleSize_ });
		//待ち状態へ
		//待ちと言ってもボタンを押してスタートのテキストが出るというもの
		titleScene->ChangeDetailScene(std::make_unique<WaitingTitleScene>());
		
		return;
	}


#ifdef _DEBUG
	//ImGui表示用
	DisplayImGui();
#endif // _DEBUG


}


void StartTitleScene::DrawSprite(){

	//黒い円の描画
	circleSprite_->Draw();
}

void StartTitleScene::DisplayImGui(){
	ImGui::Begin("開始(タイトル)");
	ImGui::InputFloat("円のサイズ", &circleScaleSize_);

	ImGui::End();


	
}
