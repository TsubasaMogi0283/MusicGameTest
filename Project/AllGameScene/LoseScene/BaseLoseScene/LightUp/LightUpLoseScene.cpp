#include "LightUpLoseScene.h"

#include <imgui.h>

#include "Input.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "LevelDataManager.h"
#include "GlobalVariables.h"

#include "Easing.h"
#include "VectorCalculation.h"
#include "Camera.h"
#include "PointLight.h"
#include "LoseScene/LoseScene.h"
#include "LoseScene/BaseLoseScene/SelectLoseScene/SelectLoseScene.h"

LightUpLoseScene::LightUpLoseScene(){
	//インスタンスの取得
	//入力クラス
	input_ = Elysia::Input::GetInstance();
	//テクスチャ管理クラス
	textureManager_ = Elysia::TextureManager::GetInstance();
	//レベルデータ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
	//モデル管理クラス
	modelManager_ = Elysia::ModelManager::GetInstance();
	//グローバル変数クラス
	globalVariables_ = Elysia::GlobalVariables::GetInstance();
}

void LightUpLoseScene::Initialize(){
	
}

void LightUpLoseScene::Update(LoseScene* loseScene){

	//ライトアップ
	if(startLightUpT_ < MAX_T_VALUE_) {
		//増える間隔
		const float INTERVAL = 0.008f;
		startLightUpT_ += INTERVAL;
		//点光源の半径を設定
		pointLight_.radius = Easing::EaseOutSine(startLightUpT_) * MAX_LIGHT_RADIUS_;
		//テキストは非表示にする
		levelDataManager_->SetTransparency(levelDataHandle_, TO_GAME, PERFECT_TRANSPARENT_);
		levelDataManager_->SetTransparency(levelDataHandle_, TO_TITLE, PERFECT_TRANSPARENT_);
		//矢印も非表示にしておく
		levelDataManager_->SetInvisible(levelDataHandle_, SELECT_ARROW, true);

	}
	//ライトアップ終了する
	else {
		
		//不透明にしていく
		transparencyT_ += INTERVAL_;
		textTransparency_ = Easing::EaseInQuart(transparencyT_);
		levelDataManager_->SetTransparency(levelDataHandle_, TO_GAME, textTransparency_);
		levelDataManager_->SetTransparency(levelDataHandle_, TO_TITLE, textTransparency_);

		//完全に不透明になったら待ち
		if (transparencyT_ >= PERFECT_NO_TRANSPARENT_) {
			waitForNextSceneTime_ += DELTA_TIME_;
		}

		
	}

	//指定した移管になったら選択へ遷移
	if (waitForNextSceneTime_ > CHANGE_NEXTSCENE_TIME_) {
		//少しだけ待ってから選択できるようにする
		loseScene->ChangeDetailScene(std::make_unique<SelectLoseScene>());
		return;
	}

#ifdef _DEBUG
	//ImGui表示用
	DisplayImGui();
#endif // _DEBUG
}

void LightUpLoseScene::DisplayImGui(){
	ImGui::Begin("ライトアップシーン(敗北シーン)");
	if (ImGui::TreeNode("時間") == true) {
		ImGui::InputFloat("次のシーンに遷移までの時間", &waitForNextSceneTime_);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("透明度") == true) {
		ImGui::InputFloat("テキスト", &textTransparency_);
		ImGui::TreePop();

	}
	ImGui::End();
}
