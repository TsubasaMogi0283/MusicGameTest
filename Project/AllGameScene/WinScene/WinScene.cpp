#include "WinScene.h"

#include <imgui.h>

#include "TextureManager.h"
#include "LevelDataManager.h"
#include "GameManager.h"
#include "Input.h"
#include "Audio.h"
#include "VectorCalculation.h"

#include "BaseWinScene/SelectWinScene/SelectWinScene.h"

WinScene::WinScene(){
	//インスタンスの取得
	//レベルデータ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
	//オーディオ
	audio_ = Elysia::Audio::GetInstance();
}

void WinScene::Initialize() {

	//レベルデータの読み込み
	levelDataHandle_ = levelDataManager_->Load("WinStage/WinStage.json");


	//平行光源の初期化
	directionalLight_.Initialize();
	directionalLight_.color = { .x = 1.0f,.y = 0.22f,.z = 0.0f,.w = 1.0f };
	directionalLight_.direction = { .x = 0.91f,.y = -1.0f,.z = 0.0f };

	//カメラの初期化
	camera_.Initialize();
	camera_.translate = { .x = 0.0f,.y = 2.0f,.z = -6.0f };
	//ポストエフェクト
	backTexture_ = std::make_unique<Elysia::BackTexture>();
	backTexture_->SetClearColour(directionalLight_.color);
	backTexture_->Initialize();

	//決定音
	bgmHandle_ = audio_->Load("Resources/Audio/Win/WinBGM.wav");
	

	//細かいシーン
	detailWinScene_ = std::make_unique<SelectWinScene>();
	//レベルデータハンドルの設定
	detailWinScene_->SetLevelDataHandle(levelDataHandle_);
	//初期化
	detailWinScene_->Initialize();

	//再生
	audio_->Play(bgmHandle_, true);
	audio_->ChangeVolume(bgmHandle_, bgmVolume_);
}

void WinScene::Update(Elysia::GameManager* gameManager){
	//細かいシーンの更新
	detailWinScene_->Update(this);

	//BGMの音量を設定
	audio_->ChangeVolume(bgmHandle_, bgmVolume_);

	//処理が終わったらタイトルへ
	if (isEnd_ == true && bgmVolume_<=0.0f) {
		//BGMを止める
		audio_->Stop(bgmHandle_);
		gameManager->ChangeScene("Title");
		return;
	}


	//平行光源の更新
	directionalLight_.Update();
	//カメラの更新
	camera_.translate = VectorCalculation::Add(camera_.translate, cameraVelocity_);
	camera_.Update();


	//脱出成功のオブジェクトの浮遊感を出すためにsinを使う
	objectFloatingTheta_ += FLOATING_THETA_;

	//基準となる座標
	Vector3 newPosition = { .x = 0.0f,.y = std::sinf(objectFloatingTheta_) + BASED_POSITION_Y_,.z = 30.0f };
	levelDataManager_->SetTranslate(levelDataHandle_, ESCAPE_SUCCEEDED_TEXT, newPosition);

	//レベルデータの更新
	levelDataManager_->Update(levelDataHandle_);

	
}

void WinScene::PreDrawPostEffect(){
	backTexture_->PreDraw();
}

void WinScene::DrawObject3D(){
	levelDataManager_->Draw(levelDataHandle_, camera_, directionalLight_);
}

void WinScene::DrawPostEffect(){
	backTexture_->Draw();
}

void WinScene::DrawSprite(){
	//細かいシーンの描画
	detailWinScene_->DrawSprite();
}

void WinScene::ChangeDetailScene(std::unique_ptr<BaseWinScene> detailScene){
	//違った時だけ遷移する
	if (detailWinScene_ != detailScene) {
		detailWinScene_ = std::move(detailScene);
		//次に遷移する
		detailWinScene_->SetLevelDataHandle(levelDataHandle_);
		detailWinScene_->Initialize();
	}
}
