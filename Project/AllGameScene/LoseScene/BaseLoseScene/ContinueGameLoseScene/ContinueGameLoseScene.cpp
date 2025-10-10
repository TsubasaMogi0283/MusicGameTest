#include "ContinueGameLoseScene.h"

#include <numbers>
#include <imgui.h>

#include "Input.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "LevelDataManager.h"
#include "GlobalVariables.h"
#include "Audio.h"

#include "Easing.h"
#include "VectorCalculation.h"
#include "LoseScene/LoseScene.h"

ContinueGameLoseScene::ContinueGameLoseScene() {
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
	//オーディオ
	audio_ = Elysia::Audio::GetInstance();
}


void ContinueGameLoseScene::Initialize(){
	//点光源の半径を最大に設定
	pointLight_.radius = MAX_LIGHT_RADIUS_;
}

void ContinueGameLoseScene::Update(LoseScene* loseScene){

	//矢印の初期座標を取得
	Vector3 arrowInitialPosition = levelDataManager_->GetInitialTranslate(levelDataHandle_, SELECT_ARROW);
	//矢印の回転
	arrowMoveTheta_ += ROTATE_VALUE_;
	//座標を設定
	levelDataManager_->SetTranslate(levelDataHandle_, SELECT_ARROW, arrowInitialPosition);

	//ライトの半径を小さくしていく
	endLightUpT_ += NARROW_VALUE;
	pointLight_.radius = std::clamp((1.0f - Easing::EaseOutSine(endLightUpT_)), 0.0f, 1.0f) * MAX_LIGHT_RADIUS_;
	//加速
	cameraVelocity_ = VectorCalculation::Add(cameraVelocity_, cameraAccelation_);


	//音量
	bgmVolume_ -= DECREASE_VOLUME_VALUE_;
	if (bgmVolume_ <= MIN_VOLUME_) {
		bgmVolume_ = MIN_VOLUME_;
	}
	audio_->ChangeVolume(loseScene->GetBgmHandle(), bgmVolume_);

	//半径が0になったら待ち
	if (endLightUpT_ >= MAX_T_VALUE_) {
		waitForCameraMoveTime_ += DELTA_TIME_;
		//BGMを止める
		audio_->Stop(loseScene->GetBgmHandle());
	}
	//指定した時間を超えたら敗北シーンを終了する
	if (waitForCameraMoveTime_ > FINISH_WAIT_TIME_) {
		loseScene->SetIsEnd();
		return;
	}


#ifdef _DEBUG
	//ImGui表示用
	DisplayImGui();
#endif // _DEBUG
}

void ContinueGameLoseScene::DisplayImGui(){

	ImGui::Begin("続ける(敗北シーン)");
	ImGui::InputFloat("待ち時間", &waitForCameraMoveTime_);
	ImGui::End();
}
