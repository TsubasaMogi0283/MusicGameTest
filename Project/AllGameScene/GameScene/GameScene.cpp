#include "GameScene.h"

#include <imgui.h>
#include <numbers>
#include <algorithm>
#include <WindowsSetup.h>
#include "Input.h"
#include "SingleCalculation.h"
#include "VectorCalculation.h"
#include "PushBackCalculation.h"
#include "AnimationManager.h"
#include "TextureManager.h"
#include "GameManager.h"
#include "ModelManager.h"
#include "LevelDataManager.h"
#include "GlobalVariables.h"
#include "Audio.h"

#include "BaseGameScene/Start/StartGameScene.h"
#include "BaseGameScene/Play/PlayGameScene.h"

GameScene::GameScene() {
	//インスタンスの取得
	//ウィンドウクラス
	windowsSetup_ = Elysia::WindowsSetup::GetInstance();
	//入力	
	input_ = Elysia::Input::GetInstance();
	//テクスチャ管理クラス
	textureManager_ = Elysia::TextureManager::GetInstance();
	//モデル管理クラス
	modelManager_ = Elysia::ModelManager::GetInstance();
	//レベルエディタ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
	//グローバル変数クラス
	globalVariables_ = Elysia::GlobalVariables::GetInstance();
	//オーディオ
	audio_ = Elysia::Audio::GetInstance();
}

void GameScene::Initialize() {

	backTexture_ = std::make_unique<Elysia::BackTexture>();
	backTexture_->Initialize();
	backTexture_->SetClearColour({ 0.0f, 0.5f, 0.5f, 1.0f });

	//カメラの初期化
	camera_.Initialize();
	camera_.translate = { 0.0f,0.0f,-10.0f };
	//平行光源
	directionalLight_.Initialize();

	
	//ボス
	uint32_t bossModeHandle = modelManager_->Load("Resources/LevelData/GameStage/Ghost","Ghost.gltf");
	boss_.reset(Elysia::Model::Create(bossModeHandle));
	bossWorldTransform_.Initialize();
	bossWorldTransform_.scale = { 0.6f,0.6f,0.6f };
	bossWorldTransform_.translate = { 0.0f,0.0f,0.0f };

	bossMaterial_.Initialize();;
	bossMaterial_.lightingKinds = LightingType::DirectionalLighting;
	//レーン
	uint32_t baseTextureHandle = textureManager_->Load("Resources/Sprite/Base/Base.png");
	base_=Elysia::Sprite::Create(baseTextureHandle, { 100.0f,0.0f });
	//判定線
	uint32_t judgementTextureHandle = textureManager_->Load("Resources/Sprite/Base/Judgement.png");
	judgement_=Elysia::Sprite::Create(judgementTextureHandle, { 100.0f,0.0f });

	attackPartTextureHandle_ = textureManager_->Load("Resources/Sprite/Part/Attack.png");
	deffencePartTextureHandle_ = textureManager_->Load("Resources/Sprite/Part/Deffence.png");
	partTextureHandle_ = deffencePartTextureHandle_;
	part_=Elysia::Sprite::Create(partTextureHandle_, { 0.0f,0.0f });
	part_->SetInvisible(true);


	//CSVに移行する
	std::vector<NoteBar> newNotesData = {
		//1
		{
			// 1
			{
				{ 0, 0, 0 },
				{ 0, 0, 0 },
				{ 0, 0, 0 },
				{ 0, 0, 0 },
			},

			// 2. bpm の値
			170,

		},
		//1
		{
			// 1
			{
				{ 0, 0, 0 },
				{ 0, 0, 0 },
				{ 0, 0, 0 },
				{ 0, 0, 0 },
			},

			// 2. bpm の値
			170,

		},
		//1
		{
			// 1
			{
				{ 0, 0, 0 },
				{ 0, 0, 0 },
				{ 0, 0, 0 },
				{ 0, 0, 0 },
			},

			// 2. bpm の値
			170,

		},
		//1
		{
			// 1
			{
				{ 0, 0, 0 },
				{ 0, 0, 0 },
				{ 0, 0, 0 },
				{ 0, 0, 0 },
			},

			// 2. bpm の値
			170,

		},

		//1
		{
			// 1
			{
				{ 1, 0, 0 },
				{ 0, 1, 0 },
				{ 0, 0, 1 },
				{ 0, 1, 0 },
			},

			// 2. bpm の値
			170,

		},
		
		//1
		{
			// 1
			{
				{ 1, 0, 1 },
				{ 0, 1, 0 },
				{ 1, 0, 1 },
				{ 0, 1, 0 },
			},

			// 2. bpm の値
			170,

		},


		//1
		{
			// 1
			{
				{ 1, 0, 1 },
				{ 0, 1, 0 },
				{ 1, 0, 1 },
				{ 0, 1, 0 },
			},

			// 2. bpm の値
			170,

		},


		//1
		{
			// 1
			{
				{ 1, 0, 1 },
				{ 0, 1, 0 },
				{ 0, 0, 1 },
				{ 0, 1, 0 },
			},

			// 2. bpm の値
			170,

		},
		//1
		{
			// 1
			{
				{ 0, 1, 0 },
				{ 0, 1, 1 },
				{ 0, 1, 0 },
				{ 1, 1, 0 },
			},

			// 2. bpm の値
			170,

		},


		//1
		{
			// 1
			{
				{ 1, 0, 1 },
				{ 0, 1, 0 },
				{ 0, 0, 1 },
				{ 0, 1, 0 },
			},

			// 2. bpm の値
			170,

		},
		//1
		{
			// 1
			{
				{ 0, 1, 0 },
				{ 0, 1, 1 },
				{ 0, 1, 0 },
				{ 1, 1, 0 },
			},

			// 2. bpm の値
			170,

		},
		//1
		{
			// 1
			{
				{ 0, 1, 0 },
				{ 1, 1, 0 },
				{ 0, 1, 0 },
				{ 0, 1, 1 },
			},

			// 2. bpm の値
			170,

		},

	};

	uint32_t noteTextureHandle = textureManager_->Load("Resources/Sprite/Note.png");
	noteRadius_ = static_cast<uint32_t>(textureManager_->GetTextureHeight(noteTextureHandle)/2u);

	//ハイスピはここで決めて時間に反映させたらいけそう

	//合計の時間
	float_t totalTime = 0.0f;
	//開始時間の設定
	float_t startTime = START_OFFSET_TIME_;

	for (const auto& bar : newNotesData) {
		//1拍の秒数
		float_t beatDuration = 60.0f / bar.bpm;
		//ノーツ間隔（1小節4拍）
		float_t noteInterval = (beatDuration * 4.0f) / bar.notes.size();

		for (size_t i = 0u; i < bar.notes.size(); ++i) {
			const auto& note = bar.notes[i];

			// 3レーン分チェック
			if (note.left == 1) {


				leftNoteInstances_.emplace_back(
					//レーン
					LanePosition::Left,
					//判定線到着時間
					totalTime + i * noteInterval,
					totalTime + i * noteInterval - startTime,
					true,
					Vector2{100.0f, WAITING_POSITION_Y_},
					Elysia::Sprite::Create(noteTextureHandle, { 100.0f, WAITING_POSITION_Y_ })
				);

			}
			if (note.middle == 1) {



				middleNoteInstances_.emplace_back(
					//レーン
					LanePosition::Middle,
					totalTime + i * noteInterval,
					totalTime + i * noteInterval - startTime,
					true,
					Vector2{ 200.0f, WAITING_POSITION_Y_ },
					Elysia::Sprite::Create(noteTextureHandle, { 200.0f, WAITING_POSITION_Y_ })
				);
			}
			if (note.right == 1) {


				rightNoteInstances_.emplace_back(
					//レーン
					LanePosition::Right,
					totalTime + i * noteInterval,
					totalTime + i * noteInterval - startTime,
					true,
					Vector2{ 300.0f, WAITING_POSITION_Y_ },
					Elysia::Sprite::Create(noteTextureHandle, { 300.0f, WAITING_POSITION_Y_ })
				);
			}
		}
		totalTime += 4.0f * beatDuration; // 小節分進める
	}


	//読み込み
	musicHandle_ = audio_->Load("Resources/Audio/Sample2.mp3");
	audio_->ChangeVolume(musicHandle_, 0.5f);
	arraiveSEHandle_ = audio_->Load("Resources/Audio/SE/Arrive.wav");

}

void GameScene::FlowProcessLeft() {

	int32_t leftClosestNoteIndex = -1;
	for (int32_t i = 0; i < leftNoteInstances_.size(); ++i) {
		auto& note = leftNoteInstances_[i];

		if (note.isJudged) {
			continue;
		}

		//動きの割合
		float_t moveRatio = SingleCalculation::InverseLerp(note.startMoveTime, note.judgementArrivalTime, musicTime_);
		//座標の計算
		float_t positionY = SingleCalculation::Lerp(WAITING_POSITION_Y_, JUDGEMENT_POSITION_Y_, moveRatio);
		//設定
		note.noteSprite->SetPositionY(positionY);

#ifdef _DEBUG
		ImGui::Begin("左ノーツ");
		ImGui::InputFloat("割合", &moveRatio);
		ImGui::InputFloat("Y座標", &positionY);
		ImGui::End();

		if (!note.isPlaySE && moveRatio >= 1.0f) {
			audio_->Play(arraiveSEHandle_, false);
			note.isPlaySE = true;
		}

#endif // _DEBUG

		//入力されたとき
		if (isHitLeft_) {
			leftClosestNoteIndex = i; // ★ 最も近いノーツのインデックスを記録
			break;
		}

		//見過ごし用
		float_t overlookValue = musicTime_ - note.judgementArrivalTime;
		//見過ごし(Miss)用
		if (overlookValue > MISS_TAP_) {
			leftResult_.miss++;
			note.judgement = NoteJudgementSelection::Miss;
			// 判定が確定したらフラグを立てる
			note.isJudged = true;
		}


	}

	if (leftClosestNoteIndex != -1) {
		auto& closestNote = leftNoteInstances_[leftClosestNoteIndex];
		//絶対値版
		float_t absJudgementTime = std::abs(leftTouchTime_ - closestNote.judgementArrivalTime);
		
		//Perfect用
		if (absJudgementTime >= 0.0f &&
			absJudgementTime < PERFECT_TAP_) {
			leftResult_.perfect++;
			closestNote.judgement = NoteJudgementSelection::Perfect;
			// 判定が確定したらフラグを立てる
			closestNote.isJudged = true;
		}
		//Great用
		if (absJudgementTime >= PERFECT_TAP_ &&
			absJudgementTime < GREAT_TAP_) {
			leftResult_.great++;
			closestNote.judgement = NoteJudgementSelection::Great;
			// 判定が確定したらフラグを立てる
			closestNote.isJudged = true;
		}
		//Good用
		else if (absJudgementTime >= GREAT_TAP_ &&
			absJudgementTime < GOOD_TAP_) {
			leftResult_.good;
			closestNote.judgement = NoteJudgementSelection::Good;
			// 判定が確定したらフラグを立てる
			closestNote.isJudged = true;
		}
		//Miss用
		else if (absJudgementTime >= GOOD_TAP_ &&
			absJudgementTime < MISS_TAP_) {
			leftResult_.miss++;
			closestNote.judgement = NoteJudgementSelection::Miss;
			// 判定が確定したらフラグを立てる
			closestNote.isJudged = true;
		}
	}

}

void GameScene::FlowProcessMiddle() {

	int32_t	middleClosestNoteIndex = -1;

	for (int32_t i = 0; i < middleNoteInstances_.size(); ++i) {
		auto& note = middleNoteInstances_[i];

		if (note.isJudged) {
			continue;
		}

		//動きの割合
		float_t moveRatio = SingleCalculation::InverseLerp(note.startMoveTime, note.judgementArrivalTime, musicTime_);
		//座標の計算
		float_t positionY = SingleCalculation::Lerp(WAITING_POSITION_Y_, JUDGEMENT_POSITION_Y_, moveRatio);
		//設定
		note.noteSprite->SetPositionY(positionY);

#ifdef _DEBUG
		ImGui::Begin("中ノーツ");
		ImGui::InputFloat("割合", &moveRatio);
		ImGui::InputFloat("Y座標", &positionY);
		ImGui::End();


#endif // _DEBUG

		//入力されたとき
		if (isHitMiddle_) {
			middleClosestNoteIndex = i; // ★ 最も近いノーツのインデックスを記録
			break;
		}

		//見過ごし用
		float_t overlookValue = musicTime_ - note.judgementArrivalTime;
		//見過ごし(Miss)用
		if (overlookValue > MISS_TAP_) {
			middleResult_.miss++;
			note.judgement = NoteJudgementSelection::Miss;
			// 判定が確定したらフラグを立てる
			note.isJudged = true;
		}


	}

	if (middleClosestNoteIndex != -1) {
		auto& closestNote = middleNoteInstances_[middleClosestNoteIndex];
		//絶対値版
		float_t absJudgementTime = std::abs(middleTouchTime_ - closestNote.judgementArrivalTime);

		//Perfect用
		if (absJudgementTime >= 0.0f &&
			absJudgementTime < PERFECT_TAP_) {
			middleResult_.perfect++;
			closestNote.judgement = NoteJudgementSelection::Perfect;
			// 判定が確定したらフラグを立てる
			closestNote.isJudged = true;
		}
		//Great用
		if (absJudgementTime >= PERFECT_TAP_ &&
			absJudgementTime < GREAT_TAP_) {
			middleResult_.great++;
			closestNote.judgement = NoteJudgementSelection::Great;
			// 判定が確定したらフラグを立てる
			closestNote.isJudged = true;
		}
		//Good用
		else if (absJudgementTime >= GREAT_TAP_ &&
			absJudgementTime < GOOD_TAP_) {
			middleResult_.good;
			closestNote.judgement = NoteJudgementSelection::Good;
			// 判定が確定したらフラグを立てる
			closestNote.isJudged = true;
		}
		//Miss用
		else if (absJudgementTime >= GOOD_TAP_ &&
			absJudgementTime < MISS_TAP_) {
			middleResult_.miss++;
			closestNote.judgement = NoteJudgementSelection::Miss;
			// 判定が確定したらフラグを立てる
			closestNote.isJudged = true;
		}
	}
}

void GameScene::FlowProcessRight() {

	int32_t rightClosestNoteIndex = -1;
	for (int32_t i = 0; i < rightNoteInstances_.size(); ++i) {
		auto& note = rightNoteInstances_[i];

		if (note.isJudged) {
			continue;
		}

		//動きの割合
		float_t moveRatio = SingleCalculation::InverseLerp(note.startMoveTime, note.judgementArrivalTime, musicTime_);
		//座標の計算
		float_t positionY = SingleCalculation::Lerp(WAITING_POSITION_Y_, JUDGEMENT_POSITION_Y_, moveRatio);
		//設定
		note.noteSprite->SetPositionY(positionY);

#ifdef _DEBUG
		ImGui::Begin("右ノーツ");
		ImGui::InputFloat("割合", &moveRatio);
		ImGui::InputFloat("Y座標", &positionY);
		ImGui::End();

#endif // _DEBUG

		//入力されたとき
		if (isHitRight_) {
			rightClosestNoteIndex = i; // ★ 最も近いノーツのインデックスを記録
			break;
		}

		//見過ごし用
		float_t overlookValue = musicTime_ - note.judgementArrivalTime;
		//見過ごし(Miss)用
		if (overlookValue > MISS_TAP_) {
			rightResult_.miss++;
			note.judgement = NoteJudgementSelection::Miss;
			// 判定が確定したらフラグを立てる
			note.isJudged = true;
		}


	}

	if (rightClosestNoteIndex != -1) {
		auto& closestNote = rightNoteInstances_[rightClosestNoteIndex];
		//絶対値版
		float_t absJudgementTime = std::abs(rightTouchTime_ - closestNote.judgementArrivalTime);

		//Perfect用
		if (absJudgementTime >= 0.0f &&
			absJudgementTime < PERFECT_TAP_) {
			rightResult_.perfect++;
			closestNote.judgement = NoteJudgementSelection::Perfect;
			// 判定が確定したらフラグを立てる
			closestNote.isJudged = true;
		}
		//Great用
		if (absJudgementTime >= PERFECT_TAP_ &&
			absJudgementTime < GREAT_TAP_) {
			rightResult_.great++;
			closestNote.judgement = NoteJudgementSelection::Great;
			// 判定が確定したらフラグを立てる
			closestNote.isJudged = true;
		}
		//Good用
		else if (absJudgementTime >= GREAT_TAP_ &&
			absJudgementTime < GOOD_TAP_) {
			rightResult_.good;
			closestNote.judgement = NoteJudgementSelection::Good;
			// 判定が確定したらフラグを立てる
			closestNote.isJudged = true;
		}
		//Miss用
		else if (absJudgementTime >= GOOD_TAP_ &&
			absJudgementTime < MISS_TAP_) {
			rightResult_.miss++;
			closestNote.judgement = NoteJudgementSelection::Miss;
			// 判定が確定したらフラグを立てる
			closestNote.isJudged = true;
		}
	}
}



void GameScene::Update(Elysia::GameManager* gameManager) {
	//開始までの時間を計測
	if (!isStart_) {
		//パート非表示
		part_->SetInvisible(true);

		waitingTime_ += DELTA_TIME_;
		if (waitingTime_>WAIT_LIMIT_TIME_) {
			startTime_ += DELTA_TIME_;
			if (startTime_ > 3.0f) {
				isStart_ = true;
				audio_->Play(musicHandle_, false);
			}
		}
	}
	else {

		
		// 再生時間を取得
		musicTime_ = audio_->GetPlayCurrentTime(musicHandle_);
		//オフセットも踏まえたうえでの値
		//こっちを使う
		noteMoveTime_ = musicTime_ +  START_OFFSET_TIME_;


		//入力状態を記録
		isHitLeft_ = false;
		isHitMiddle_ = false;
		isHitRight_ = false;

		leftTouchTime_ = 0.0f;
		middleTouchTime_ = 0.0f;
		rightTouchTime_ = 0.0f;

		//タッチした
		InputCondition inputCondition = {};

		// 2. キー入力のチェックと、入力があったレーンの決定
		if (input_->IsTriggerKey(DIK_G) == true) {
			touch_.left++;
			inputCondition.isInputLeft = true;
			leftTouchTime_ = musicTime_;
			isHitLeft_ = true;
		}
		if (input_->IsTriggerKey(DIK_H) == true) {
			touch_.middle++;
			inputCondition.isInputMiddle = true;
			middleTouchTime_ = musicTime_;
			isHitMiddle_ = true;
		}
		if (input_->IsTriggerKey(DIK_J) == true) {
			touch_.right++;
			inputCondition.isInputRight = true;
			rightTouchTime_ = musicTime_;
			isHitRight_ = true;
		}
		//左
		FlowProcessLeft();
		//中
		FlowProcessMiddle();
		//右
		FlowProcessRight();


	}

	gameManager;

	//ボス関係の更新
	bossWorldTransform_.Update();
	bossMaterial_.Update();
	//ライトの更新
	directionalLight_.Update();
	//カメラの更新
	camera_.Update();
	
#ifdef _DEBUG 

	//ImGuiの表示
	DisplayImGui();

#endif // _DEBUG
}



void GameScene::DisplayImGui() {
	ImGui::Begin("ゲームシーン");

	ImGui::InputInt("左タッチ", &touch_.left);
	ImGui::InputInt("中タッチ", &touch_.middle);
	ImGui::InputInt("右タッチ", &touch_.right);
	ImGui::Text("判定左");
	ImGui::InputInt("Perfect", &leftResult_.perfect);
	ImGui::InputInt("Great", &leftResult_.great);
	ImGui::InputInt("Good", &leftResult_.good);
	ImGui::InputInt("Miss", &leftResult_.miss);
	ImGui::Text("判定中");
	ImGui::InputInt("Perfect", &middleResult_.perfect);
	ImGui::InputInt("Great", &middleResult_.great);
	ImGui::InputInt("Good", &middleResult_.good);
	ImGui::InputInt("Miss", &middleResult_.miss);
	ImGui::Text("判定右");
	ImGui::InputInt("Perfect", &rightResult_.perfect);
	ImGui::InputInt("Great", &rightResult_.great);
	ImGui::InputInt("Good", &rightResult_.good);
	ImGui::InputInt("Miss", &rightResult_.miss);
	ImGui::Text("総合判定");
	int32_t perfect = leftResult_.perfect + middleResult_.perfect + rightResult_.perfect;
	int32_t great = leftResult_.great + middleResult_.great + rightResult_.great;
	int32_t good = leftResult_.good + middleResult_.good + rightResult_.good;
	int32_t miss = leftResult_.miss + middleResult_.miss + rightResult_.miss;

	ImGui::InputInt("Perfect", &perfect);
	ImGui::InputInt("Great", &great);
	ImGui::InputInt("Good", &good);
	ImGui::InputInt("Miss", &miss);


	ImGui::InputFloat("待機", &waitingTime_);
	ImGui::InputFloat("開始", &startTime_);
	ImGui::InputFloat("再生時間", &noteMoveTime_);
	ImGui::End();
}

void GameScene::PreDrawPostEffect() {
	//ビネット描画処理前
	backTexture_->PreDraw();
}

void GameScene::DrawObject3D() {
	//boss_->Draw(bossWorldTransform_,camera_,bossMaterial_,directionalLight_);

}

void GameScene::DrawPostEffect() {
	//ビネット描画
	backTexture_->Draw();
}

void GameScene::DrawSprite() {
	//レーン
	base_->Draw();
	//判定線
	judgement_->Draw();
	//ノーツ
	for (const auto& note : leftNoteInstances_) {


		if (note.noteSprite != nullptr&&note.isJudged==false) {
			
			note.noteSprite->Draw();
		}
		
	}
	for (const auto& note : middleNoteInstances_) {


		if (note.noteSprite != nullptr && note.isJudged == false) {

			note.noteSprite->Draw();
		}

	}
	for (const auto& note : rightNoteInstances_) {


		if (note.noteSprite != nullptr && note.isJudged == false) {

			note.noteSprite->Draw();
		}

	}

	//part_->Draw();

}
