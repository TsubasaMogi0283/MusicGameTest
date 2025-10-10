#include "TitleScene.h"
#include <imgui.h>
#include <numbers>
#include <array>

#include "Input.h"
#include "GameManager.h"
#include "TextureManager.h"
#include "LevelDataManager.h"
#include "Audio.h"
#include "VectorCalculation.h"
#include "SingleCalculation.h"
#include "Calculation/QuaternionCalculation.h"


#include "BaseBackTexture/Sunset/SunsetBackTexture.h"
#include "BaseTitleScene/Start/StartTitleScene.h"

TitleScene::TitleScene() {
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

void TitleScene::Initialize() {
	//レベルデータの読み込み
	levelHandle_ = levelDataManager_->Load("TitleStage/TitleStage.json");

	//平行光源
	directionalLight_.Initialize();
	directionalLight_.color = { .x = 1.0f,.y = 0.22f,.z = 0.0f,.w = 1.0f };
	directionalLight_.direction = { .x = 0.91f,.y = -1.0f,.z = 0.0f };

	//ランダムノイズ
	randomNoise_.Initialize();

	//カメラの初期化
	camera_.Initialize();

	//レールカメラ
	titleRailCamera_ = std::make_unique<TitleRailCamera>();
	//初期化
	titleRailCamera_->Initialize("Resources/CSV/TitleRailCameraPoint.csv");

	//背景
	//ポストエフェクト
	baseTitleBackTexture_ = std::make_unique<SunsetBackTexture>();
	baseTitleBackTexture_->Initialize();

	//ランダムエフェクトの生成
	randomEffect_ = std::make_unique<Elysia::RandomNoisePostEffect>();
	//初期化
	randomEffect_->Initialize();

	//環境音の読み込み
	enviromentAudioHandle_ = audio_->Load("Resources/Audio/Environment/Environment.wav");
	//最初は音量を0にする
	audio_->ChangeVolume(enviromentAudioHandle_, 0.0f);

	//細かいシーン
	detailTitleScene_ = std::make_unique<StartTitleScene>();
	//レベルデータハンドルの設定
	detailTitleScene_->SetLevelDataHandle(levelHandle_);
	//初期化
	detailTitleScene_->Initialize();

	
	//再生
	//遅延を防ぐためには初期化の最後でやった方が良いと気づいた
	audio_->Play(enviromentAudioHandle_, true);
	
}

void TitleScene::Update(Elysia::GameManager* gameManager) {
	//細かいシーンの更新
	detailTitleScene_->Update(this);

	//レベルエディタの更新
	levelDataManager_->Update(levelHandle_);

	//平行光源の更新
	directionalLight_.intensity = detailTitleScene_->GetDirectionalLight().intensity;
	directionalLight_.color = detailTitleScene_->GetDirectionalLight().color;
	directionalLight_.direction = detailTitleScene_->GetDirectionalLight().direction;
	directionalLight_.Update();

	//ランダムノイズの更新
	randomNoise_.Update();

	//レールカメラの更新
	titleRailCamera_->Update();
	//レールカメラから2つの行列を取得
	camera_.viewMatrix = titleRailCamera_->GetCamera().viewMatrix;
	camera_.projectionMatrix = titleRailCamera_->GetCamera().projectionMatrix;
	//カメラの更新
	camera_.Transfer();

	//処理を終えたらゲームシーンへ
	if(isEnd_ == true) {
		gameManager->ChangeScene("Game");
		return;
	}
	
#ifdef _DEBUG
	//ImGui用
	DisplayImGui();
#endif
}

void TitleScene::DrawObject3D() {
	//ステージオブジェクト
	levelDataManager_->Draw(levelHandle_, camera_, directionalLight_);

}

void TitleScene::PreDrawPostEffect() {
	if (isDisplayRandomEffect_ == true) {
		//ランダム
		randomEffect_->PreDraw();
	}
	else {
		//背景
		baseTitleBackTexture_->PreDraw();
	}

}

void TitleScene::DrawPostEffect() {
	if (isDisplayRandomEffect_ == true) {
		//ランダム
		randomEffect_->Draw(randomNoise_);
	}
	else {
		//背景
		baseTitleBackTexture_->Draw();
	}
}

void TitleScene::DrawSprite() {
	//細かいシーンの描画
	detailTitleScene_->DrawSprite();

}

void TitleScene::DisplayImGui() {
	ImGui::Begin("TitleFade&Effect");
	ImGui::Checkbox("IsDisplay", &isDisplayRandomEffect_);
	ImGui::End();

	ImGui::Begin("平行光源");
	ImGui::SliderFloat4("色", &directionalLight_.color.x, 0.0f, 1.0f);
	ImGui::SliderFloat3("方向", &directionalLight_.direction.x, -1.0f, 1.0f);
	ImGui::End();

}


void TitleScene::ChangeBackTexture(std::unique_ptr<BaseTitleBackTexture> backTexture) {
	//違った時だけ遷移する
	if (baseTitleBackTexture_ != backTexture) {
		baseTitleBackTexture_ = std::move(backTexture);
		//次に遷移する
		baseTitleBackTexture_->Initialize();
	}
}

void TitleScene::ChangeDetailScene(std::unique_ptr<BaseTitleScene> detailTitleScene){
	//違った時だけ遷移する
	if (detailTitleScene_ != detailTitleScene) {
		detailTitleScene_ = std::move(detailTitleScene);
		//レベルデータハンドルの設定
		detailTitleScene_->SetLevelDataHandle(levelHandle_);
		//次に遷移する
		detailTitleScene_->Initialize();
	}
}

