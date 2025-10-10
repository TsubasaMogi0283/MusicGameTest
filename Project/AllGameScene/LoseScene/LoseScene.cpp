#include "LoseScene.h"

#include <imgui.h>

#include "Input.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "LevelDataManager.h"
#include "Audio.h"
#include "GameManager.h"
#include "VectorCalculation.h"
#include "GlobalVariables.h"
#include "Easing.h"
#include "BaseLoseScene/LightUp/LightUpLoseScene.h"


LoseScene::LoseScene(){

	//インスタンスの取得
	//入力クラス
	input_ = Elysia::Input::GetInstance();
	//レベルデータ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
	//グローバル変数クラス
	globalVariables_ = Elysia::GlobalVariables::GetInstance();
	//オーディオ
	audio_ = Elysia::Audio::GetInstance();
}

void LoseScene::Initialize(){

	//負けシーン用のレベルデータを入れる
	levelDataHandle_ = levelDataManager_->Load("LoseStage/LoseStage.json");
	

	//背景(ポストエフェクト)
	backTexture_ = std::make_unique<Elysia::BackTexture>();
	const Vector4 CLEAR_COLOR = { .x = 0.0f,.y = 0.0f,.z = 0.0f,.w = 1.0f };
	backTexture_->SetClearColour(CLEAR_COLOR);
	backTexture_->Initialize();

	//ディゾルブ
	dissolveEffect_ = std::make_unique<Elysia::DissolvePostEffect>();
	dissolveEffect_->Initialize(CLEAR_COLOR);
	//マスクテクスチャ
	uint32_t maskTexture = Elysia::TextureManager::GetInstance()->Load("Resources/External/Texture/Dissolve/noise0.png");


	//調整項目として記録
	globalVariables_->CreateGroup(DISSOLVE_NAME_);
	globalVariables_->AddItem(DISSOLVE_NAME_, "Thinkness", dissolve_.edgeThinkness);

	//初期化
	dissolve_.Initialize();
	dissolve_.maskTextureHandle = maskTexture;
	dissolve_.edgeThinkness = globalVariables_->GetFloatValue(DISSOLVE_NAME_, "Thinkness");
	dissolve_.threshold = 0.0f;
	//カメラの初期化
	camera_.Initialize();
	camera_.translate = { .x = 0.0f,.y = 2.8f,.z = -23.0f };

	//点光源
	//調整項目として記録
	globalVariables_->CreateGroup(POINT_LIGHT_NAME_);
	globalVariables_->AddItem(POINT_LIGHT_NAME_, "Translate", pointLight_.position);
	globalVariables_->AddItem(POINT_LIGHT_NAME_, "Decay", pointLight_.decay);

	//初期化
	pointLight_.Initialize();
	pointLight_.position = globalVariables_->GetVector3Value(POINT_LIGHT_NAME_, "Translate");
	pointLight_.decay = globalVariables_->GetFloatValue(POINT_LIGHT_NAME_,"Decay");
	pointLight_.radius = 0.0f;

	//読み込み
	bgmHandle_ = audio_->Load("Resources/Audio/Lose/LoseBGM.wav");

	//細かいシーン
	detailLoseScene_ = std::make_unique<LightUpLoseScene>();
	//レベルデータハンドルの設定
	detailLoseScene_->SetLevelDataHandle(levelDataHandle_);
	//初期化
	detailLoseScene_->Initialize();
	
	//再生
	audio_->Play(bgmHandle_, true);
}

void LoseScene::Update(Elysia::GameManager* gameManager){

	//細かいシーンの更新
	detailLoseScene_->Update(this);


	//処理が終わった場合
	if (isEnd_ == true) {
		//ゲームへ
		if (isContinue_ == true) {
			gameManager->ChangeScene("Game");
			return;
		}
		//タイトルへ
		else {
			gameManager->ChangeScene("Title");
			return;
		}
	}

	//レベルデータの更新
	levelDataManager_->Update(levelDataHandle_);

	//カメラの更新
	camera_.translate = VectorCalculation::Add(camera_.translate, detailLoseScene_->GetCameraVelocity());
	camera_.Update();
	//点光源の更新
	pointLight_.position = globalVariables_->GetVector3Value(POINT_LIGHT_NAME_, "Translate");
	pointLight_.decay = globalVariables_->GetFloatValue(POINT_LIGHT_NAME_, "Decay");
	pointLight_.radius = detailLoseScene_->GetPointLight().radius;
	pointLight_.Update();
	//ディゾルブの更新
	dissolve_.threshold = detailLoseScene_->GetDissolve().threshold;
	dissolve_.edgeThinkness = globalVariables_->GetFloatValue(DISSOLVE_NAME_, "Thinkness");
	dissolve_.Update();

	//調整
	Adjustment();
	
#ifdef _DEBUG
	//ImGui
	DisplayImGui();
	
	//再読み込み
	if (input_->IsTriggerKey(DIK_L) == true) {
		levelDataManager_->Reload(levelDataHandle_);
	}

#endif // _DEBUG
}


void LoseScene::DrawObject3D(){
	//レベルデータ
	levelDataManager_->Draw(levelDataHandle_, camera_, pointLight_);
	
}

void LoseScene::PreDrawPostEffect(){
	dissolveEffect_->PreDraw();
}

void LoseScene::DrawPostEffect(){
	dissolveEffect_->Draw(dissolve_);
}

void LoseScene::DrawSprite(){

}

void LoseScene::ChangeDetailScene(std::unique_ptr<BaseLoseScene>detailScene){
	//違った時だけ遷移する
	if (detailLoseScene_ != detailScene) {
		detailLoseScene_ = std::move(detailScene);
		//次に遷移する
		detailLoseScene_->SetLevelDataHandle(levelDataHandle_);
		detailLoseScene_->Initialize();
	}
}


void LoseScene::DisplayImGui(){

	ImGui::Begin("脱出失敗シーン");
	if (ImGui::TreeNode("点光源")) {
		ImGui::SliderFloat3("座標", &pointLight_.position.x, -40.0f, 40.0f);
		ImGui::SliderFloat("Decay", &pointLight_.decay, 0.0f, 20.0f);
		ImGui::SliderFloat("半径", &pointLight_.radius, 0.0f, 20.0f);
		ImGui::TreePop();
		
	}
	if (ImGui::TreeNode("ディゾルブ")) {
		ImGui::SliderFloat("しきい値", &dissolve_.threshold, 0.0f, 2.0f);
		ImGui::SliderFloat("厚さ", &dissolve_.edgeThinkness, 0.0f, 2.0f);
		ImGui::TreePop();
		
	}
	if (ImGui::TreeNode("カメラ")) {
		ImGui::SliderFloat3("座標", &camera_.translate.x, -40.0f, 40.0f);
		ImGui::TreePop();
	}

	ImGui::End();
	

}

void LoseScene::Adjustment(){
	globalVariables_->SaveFile(DISSOLVE_NAME_);
	globalVariables_->SaveFile(POINT_LIGHT_NAME_);

}

