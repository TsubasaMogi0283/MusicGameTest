#include "SelectLoseScene.h"

#include <numbers>
#include <imgui.h>

#include "Input.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "LevelDataManager.h"
#include "GlobalVariables.h"
#include "Audio.h"
#include "LoseScene/LoseScene.h"
#include "LoseScene/BaseLoseScene/ContinueGameLoseScene/ContinueGameLoseScene.h"
#include "LoseScene/BaseLoseScene/ReturnTitleLoseScene/ReturnTitleLoseScene.h"

SelectLoseScene::SelectLoseScene() {
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


void SelectLoseScene::Initialize(){
	//矢印を表示させる
	levelDataManager_->SetInvisible(levelDataHandle_, SELECT_ARROW, false);
	//矢印の初期座標を取得
	arrowInitialPosition = levelDataManager_->GetInitialTranslate(levelDataHandle_, SELECT_ARROW);
	//「ゲームへ」の座標を取得
	toGameInitialPosition = levelDataManager_->GetInitialTranslate(levelDataHandle_, TO_GAME);
	//「タイトルへ」の座標を取得
	toTitleInitialPosition = levelDataManager_->GetInitialTranslate(levelDataHandle_, TO_TITLE);
	//非選択時
	noSelectedScale = levelDataManager_->GetInitiaScale(levelDataHandle_, TO_TITLE);
	
	//点光源の半径を最大に設定
	pointLight_.radius = MAX_LIGHT_RADIUS_;


	//決定音の読み込み
	desideSeHandle_ = audio_->Load("Resources/Audio/SE/Deside.wav");
	//選択音
	selectSeHandle_ = audio_->Load("Resources/Audio/SE/Select.wav");
}

void SelectLoseScene::Update(LoseScene* loseScene){
	
	//矢印の回転
	arrowRotate_ += ROTATE_VALUE_;
	levelDataManager_->SetRotate(levelDataHandle_, SELECT_ARROW, { .x = 0.0f,.y = arrowRotate_ ,.z = 0.0f });

	//「ゲームへ」つまり続けるを選択時
	if (isContinue_==true) {

		//選択されている方である「ゲーム」は大きくする
		levelDataManager_->SetScale(levelDataHandle_, TO_GAME, { .x = selectedScale_,.y = selectedScale_ ,.z = selectedScale_ });
		levelDataManager_->SetTranslate(levelDataHandle_, TO_GAME, { .x = toGameInitialPosition.x,.y = selectedHeight ,.z = toGameInitialPosition.x });

		//選択されていない方である「タイトル」は初期位置で
		levelDataManager_->SetScale(levelDataHandle_, TO_TITLE, noSelectedScale);
		levelDataManager_->SetTranslate(levelDataHandle_, TO_TITLE, toTitleInitialPosition);

		//右・D押したとき「タイトルへ」に移動
		if ((input_->IsTriggerKey(DIK_RIGHT) == true) || (input_->IsTriggerKey(DIK_D) == true) || (input_->IsTriggerButton(XINPUT_GAMEPAD_DPAD_RIGHT) == true)) {
			levelDataManager_->SetTranslate(levelDataHandle_, SELECT_ARROW, { .x = toTitleInitialPosition.x,.y = arrowInitialPosition.y ,.z = toTitleInitialPosition.z });
			isContinue_ = false;
			//選択音の再生
			audio_->Play(selectSeHandle_, false);
		}

	}
	//「タイトルへ」を選択時
	else {

		//選択されている方である「タイトル」は大きくする
		levelDataManager_->SetScale(levelDataHandle_, TO_TITLE, { .x = selectedScale_,.y = selectedScale_ ,.z = selectedScale_ });
		levelDataManager_->SetTranslate(levelDataHandle_, TO_TITLE, { .x = toTitleInitialPosition.x,.y = selectedHeight ,.z = toTitleInitialPosition.z });

		//選択されていない方である「ゲーム」は初期位置で
		levelDataManager_->SetScale(levelDataHandle_, TO_GAME, noSelectedScale);
		levelDataManager_->SetTranslate(levelDataHandle_, TO_GAME, toGameInitialPosition);
		//左・A押したとき「タイトルへ」に移動
		if ((input_->IsTriggerKey(DIK_LEFT) == true) || (input_->IsTriggerKey(DIK_A) == true) || input_->IsTriggerButton(XINPUT_GAMEPAD_DPAD_LEFT) == true) {
			levelDataManager_->SetTranslate(levelDataHandle_, SELECT_ARROW, { .x = toGameInitialPosition.x,.y = arrowInitialPosition.y ,.z = toGameInitialPosition.z });
			isContinue_ = true;
			//選択音の再生
			audio_->Play(selectSeHandle_,false);
		}
	}

	//Bボタン、スペースキーを押したとき
	if (input_->IsTriggerButton(XINPUT_GAMEPAD_B) == true || input_->IsTriggerKey(DIK_SPACE) == true) {
		isChangeNextScene_ = true;
		//決定音の再生
		audio_->Play(desideSeHandle_,false);
	}

	//決定後
	if (isChangeNextScene_ == true) {

		//決定されたら跳ねるような動きをする
		decideArrowMoveTheta_ += MOVE_AMOUNT;
		float_t newTheta = std::clamp(decideArrowMoveTheta_, 0.0f, std::numbers::pi_v<float_t>);
		if (decideArrowMoveTheta_ < std::numbers::pi_v<float_t>) {
			//回転
			arrowRotate_ += FAST_ROTATE_VALUE;
			//新しい回転を設定
			levelDataManager_->SetRotate(levelDataHandle_, SELECT_ARROW, { .x = 0.0f,.y = arrowRotate_ ,.z = 0.0f });
		}
		else {
			//回転を止める
			levelDataManager_->SetRotate(levelDataHandle_, SELECT_ARROW, { .x = 0.0f,.y = std::numbers::pi_v <float_t> / 2.0f ,.z = 0.0f });
			//遷移まで待つ時間
			waitForNextSceneTime_ += DELTA_TIME_;
		}

		//高さ
		const float_t HEIGHT = 1.0f;
		float_t newArrowPositionY = arrowInitialPosition.y + std::sinf(newTheta) * HEIGHT;
		float_t newArrowPositionX = 0.0f;
		//新しい座標を設定
		if (isContinue_ == true) {
			newArrowPositionX = toGameInitialPosition.x;
		}
		else {
			newArrowPositionX = toTitleInitialPosition.x;
		}
		levelDataManager_->SetTranslate(levelDataHandle_, SELECT_ARROW, { .x = newArrowPositionX,.y = newArrowPositionY ,.z = arrowInitialPosition.z });


	}

	//次のシーンへ
	if (waitForNextSceneTime_ > CHANGE_NEXTSCENE_TIME_) {
		//続けるかどうかを設定
		loseScene->SetIsContinue(isContinue_);
		
		//続けるシーンへ
		if (isContinue_ == true) {
			loseScene->ChangeDetailScene(std::make_unique<ContinueGameLoseScene>());
			return;
		}
		//タイトルに戻る
		else {
			loseScene->ChangeDetailScene(std::make_unique<ReturnTitleLoseScene>());
			return;
		}
	}

#ifdef _DEBUG
	//ImGui表示用
	DisplayImGui();
#endif // _DEBUG
}

void SelectLoseScene::DisplayImGui(){

	ImGui::Begin("選択(敗北シーン)");
	ImGui::InputFloat("矢印の回転", &arrowRotate_);
	ImGui::End();

}
