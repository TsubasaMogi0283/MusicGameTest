#include "ReturnTitleLoseScene.h"

#include <imgui.h>
#include <numbers>

#include "Input.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "LevelDataManager.h"
#include "GlobalVariables.h"
#include "Audio.h"

#include "Easing.h"
#include "VectorCalculation.h"
#include "LoseScene/LoseScene.h"

ReturnTitleLoseScene::ReturnTitleLoseScene() {
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


void ReturnTitleLoseScene::Initialize(){

	//矢印の初期座標を取得
	arrowInitialPosition_ = levelDataManager_->GetInitialTranslate(levelDataHandle_, SELECT_ARROW);
	//「タイトルへ」の座標を取得
	toTitleInitialPosition_ = levelDataManager_->GetInitialTranslate(levelDataHandle_, TO_TITLE);

	//点光源の半径を最大に設定
	pointLight_.radius = MAX_LIGHT_RADIUS_;
}

void ReturnTitleLoseScene::Update(LoseScene* loseScene){

	//新しい座標を設定
	levelDataManager_->SetTranslate(levelDataHandle_, SELECT_ARROW, { .x = toTitleInitialPosition_.x,.y = arrowInitialPosition_.y ,.z = arrowInitialPosition_.z });
	//エフェクトがかかるのを待つ時間
	changeWaitingTime_ += DELTA_TIME_;

	//ピッチダウン
	bgmFreaquencyRatio_ -= DECREASE_FREAQUENCY_RATIO_VALUE_;
	if (bgmFreaquencyRatio_ <= MIN_FREAQUENCY_RATIO_VALUE_) {
		bgmFreaquencyRatio_ = MIN_FREAQUENCY_RATIO_VALUE_;
	}
	audio_->ChangeFrequencyRatio(loseScene->GetBgmHandle(), bgmFreaquencyRatio_);
	
	//音量
	bgmVolume_ -= DECREASE_VOLUME_VALUE_;
	if (bgmVolume_ <= MIN_VOLUME_) {
		bgmVolume_ = MIN_VOLUME_;
	}
	audio_->ChangeVolume(loseScene->GetBgmHandle(), bgmVolume_);

	//指定した時間を超えたらタイトルへ遷移
	if (changeWaitingTime_ > FINISH_WAIT_TIME_) {
		dissolveThresholdT_ += THRESHOLD_INCREASE_AMOUNT_;
		//ディゾルブの値を増やす
		dissolve_.threshold = Easing::EaseInSine(dissolveThresholdT_);
		
		//暗くなったらシーンチェンジ
		fadeTransparency_ += TRANSPARENCY_INTERVAL_;
	}

	//完全に不透明になったら場合
	if (fadeTransparency_ > MAX_T_VALUE_) {
		waitTime_ += DELTA_TIME_;
		
	}

	//指定した時間を超えたら処理終了
	if (waitTime_ > FINISH_WAIT_TIME_) {
		//BGMを止める
		audio_->Stop(loseScene->GetBgmHandle());
		loseScene->SetIsEnd();
	}


#ifdef _DEBUG
	//ImGui
	DisplayImGui();
#endif // _DEBUG


}

void ReturnTitleLoseScene::DisplayImGui(){
	ImGui::Begin("タイトルに戻る(敗北シーン)");
	ImGui::InputFloat("音の高さ", &bgmFreaquencyRatio_);
	ImGui::InputFloat("音量", &bgmVolume_);
	ImGui::End();
}
