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

		//1
		{
			// 1
			{
				{ 1, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 0, 1 },
				{ 1, 0, 0 },
				{ 0, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 1, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 0 },
				{ 0, 1, 0 },
				{ 1, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 0, 1 },
				{ 0, 0, 0 },
			},

			// 2. bpm の値
			170,

		},


		//1
		{
			// 1
			{
				{ 0, 1, 0 },
				{ 1, 0, 1 },
				{ 0, 1, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 1, 0 },
				{ 1, 0, 1 },
				{ 0, 1, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 1 },
				{ 0, 0, 0 },
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
				{ 0, 0, 0 },
				{ 0, 0, 1 },
				{ 1, 0, 0 },
				{ 0, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 1, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 0 },
				{ 0, 1, 0 },
				{ 1, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 0, 1 },
				{ 0, 0, 0 },
			},

			// 2. bpm の値
			170,

		},


		//1
		{
			// 1
			{
				{ 0, 1, 0 },
				{ 1, 0, 1 },
				{ 0, 1, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 1, 0 },
				{ 1, 0, 1 },
				{ 0, 1, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 1 },
				{ 0, 0, 0 },
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
				{ 0, 0, 0 },
				{ 0, 0, 1 },
				{ 1, 0, 0 },
				{ 0, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 1, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 0 },
				{ 0, 1, 0 },
				{ 1, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 0, 1 },
				{ 0, 0, 0 },
			},

			// 2. bpm の値
			170,

		},


		//1
		{
			// 1
			{
				{ 0, 1, 0 },
				{ 1, 0, 1 },
				{ 0, 1, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 1, 0 },
				{ 1, 0, 1 },
				{ 0, 1, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 1 },
				{ 0, 0, 0 },
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
				{ 0, 0, 0 },
				{ 0, 0, 1 },
				{ 1, 0, 0 },
				{ 0, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 1, 0 },
				{ 0, 0, 0 },
				{ 1, 0,  },
				{ 0, 0, 0 },
				{ 1, 0, 0 },
				{ 0, 1, 0 },
				{ 1, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 0, 1 },
				{ 0, 0, 0 },
			},

			// 2. bpm の値
			170,

		},


		//1
		{
			// 1
			{
				{ 0, 1, 0 },
				{ 1, 0, 1 },
				{ 0, 1, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 1, 0 },
				{ 1, 0, 1 },
				{ 0, 1, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 1, 0 },
				{ 0, 0, 0 },
				{ 0, 1, 0 },
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
				{ 0, 0, 0 },
				{ 0, 0, 0 },
				{ 0, 1, 0 },
				{ 0, 1, 0 },
				{ 0, 0, 0 },
				{ 0, 0, 1 },
				{ 0, 0, 0 },
			},

			// 2. bpm の値
			170,

		},
		//1
		{
			// 1
			{
				{ 1, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 0, 1 },
				{ 0, 1, 0 },
				{ 1, 0, 0 },
				{ 0, 0, 0 },
				{ 1, 0, 0 },
				{ 0, 1, 0 },
			},

			// 2. bpm の値
			170,

		},
		//1
		{
			// 1
			{
				{ 0, 1, 1 },
				{ 0, 0, 0 },
				{ 0, 0, 0 },
				{ 0, 0, 1 },
				{ 0, 0, 1 },
				{ 0, 0, 0 },
				{ 1, 0, 0 },
				{ 0, 0, 0 },
			},

			// 2. bpm の値
			170,

		},
		//1
		{
			// 1
			{
				{ 0, 1, 0 },
				{ 0, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 0, 1 },
				{ 0, 0, 0 },
				{ 0, 0, 0 },
				{ 0, 0, 0 },
				{ 0, 0, 0 },
			},

			// 2. bpm の値
			170,

		},

	};

	notesData_=newNotesData;

	uint32_t noteTextureHandle = textureManager_->Load("Resources/Sprite/Note.png");
	noteRadius_ = static_cast<uint32_t>(textureManager_->GetTextureHeight(noteTextureHandle)/2u);

	//ハイスピはここで決めて時間に反映させたらいけそう

	//合計の時間
	float_t totalTime = 0.0f;
	//開始時間の設定
	float_t startTime = START_OFFSET_TIME_;

	for (const auto& bar : notesData_) {
		//1拍の秒数
		float_t beatDuration = 60.0f / bar.bpm;
		//ノーツ間隔（1小節4拍）
		float_t noteInterval = (beatDuration * 4.0f) / bar.notes.size();

		for (size_t i = 0u; i < bar.notes.size(); ++i) {
			const auto& note = bar.notes[i];

			// 3レーン分チェック
			if (note.left == 1) {
				noteInstances_.emplace_back(
					//レーン
					0,
					//判定線到着時間
					totalTime + i * noteInterval,
					totalTime + i * noteInterval - startTime,
					true,
					Vector2{100.0f, WAITING_POSITION_Y_},
					Elysia::Sprite::Create(noteTextureHandle, { 100.0f, WAITING_POSITION_Y_ })
				);

			}
			if (note.middle == 1) {
				noteInstances_.emplace_back(
					//レーン
					1, 
					totalTime + i * noteInterval,
					totalTime + i * noteInterval - startTime,
					true,
					Vector2{ 200.0f, WAITING_POSITION_Y_ },
					Elysia::Sprite::Create(noteTextureHandle, { 200.0f, WAITING_POSITION_Y_ })
				);
			}
			if (note.right == 1) {
				
				noteInstances_.emplace_back(
					//レーン
					2,
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
				musicTime_ = 0.0f;
			}
		}
	}
	else {

		
		// 再生時間を取得
		musicTime_ = audio_->GetPlayCurrentTime(musicHandle_);
		noteMoveTime_ = musicTime_- START_OFFSET_TIME_;


		int closestNoteIndex = -1;
		float_t minAbsJudgementTime = 9999.0f; // 大きな値で初期化
		int hitLaneType = -1; // どのレーンで入力があったかを記録（G:0, H:1, J:2 など）

		// 2. キー入力のチェックと、入力があったレーンの決定
		if (input_->IsTriggerKey(DIK_G) == true) {
			touch_.left++;
			hitLaneType = 0; // 例：レーン0
		}
		if (input_->IsTriggerKey(DIK_H) == true) {
			touch_.middle++;
			hitLaneType = 1; // 例：レーン1
		}
		if (input_->IsTriggerKey(DIK_J) == true) {
			touch_.right++;
			hitLaneType = 2; // 例：レーン2
		}



		for (int i = 0; i < noteInstances_.size(); ++i) {
			auto& note = noteInstances_[i];

				//判定時間
				float_t judgementTime = note.judgementArrivalTime - musicTime_;


				//開始から判定までどのくらいの位置にいるかを計算
				float_t moveRatio = SingleCalculation::InverseLerp(note.startMoveTime, note.judgementArrivalTime, musicTime_);


				

				//座標の計算
				float_t positionY = SingleCalculation::Lerp(WAITING_POSITION_Y_, JUDGEMENT_POSITION_Y_, moveRatio);
				note.noteSprite->SetPositionY(positionY);

#ifdef _DEBUG
				ImGui::Begin("ノーツ");
				ImGui::InputFloat("判定時間", &judgementTime);
				ImGui::InputFloat("割合", &moveRatio);
				ImGui::InputFloat("Y座標", &positionY);
				ImGui::End();

				if (!note.isPlaySE&& moveRatio>=1.0f) {
					audio_->Play(arraiveSEHandle_, false);
					note.isPlaySE = true;
				}

#endif // _DEBUG

				// キー入力があった場合のみ、最も近いノーツを探す
				if (hitLaneType != -1) {
					// 既に判定済み、または画面外（判定許容時間外）のノーツはスキップ
						// note.isJudged のようなフラグが必要です。
					if (note.isJudged || musicTime_ < note.startMoveTime) {
						continue;
					}

					// ノーツのレーンが入力されたレーンと一致するか
					// note.laneType にレーン情報が入っていると仮定
					if (note.lane != hitLaneType) {
						continue;
					}

					// 判定時間を計算
					//float judgementTime = note.judgementArrivalTime - musicTime_;
					float absJudgementTime = std::abs(judgementTime);

					// 判定可能範囲内のチェック（例: -0.5秒〜+0.5秒）
					if (absJudgementTime <= 0.5f) { // 許容判定時間
						// 現在見つかっている中で最も判定時間が近いノーツか？
						if (absJudgementTime < minAbsJudgementTime) {
							minAbsJudgementTime = absJudgementTime;
							closestNoteIndex = 0;
						}
					}
				}




		}
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
	if(ImGui::TreeNode("判定") == true) {
		ImGui::InputInt("Perfect", &result_.perfect);
		ImGui::InputInt("Great", &result_.great);
		ImGui::InputInt("Good", &result_.good);
		ImGui::InputInt("Miss", &result_.miss);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("入力") == true) {
		ImGui::InputInt("左", &touch_.left);
		ImGui::InputInt("中", &touch_.middle);
		ImGui::InputInt("右", &touch_.right);
		ImGui::TreePop();
	}

	ImGui::InputFloat("待機", &waitingTime_);
	ImGui::InputFloat("開始", &startTime_);
	ImGui::InputFloat("再生時間", &musicTime_);
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
	for (const auto& note : noteInstances_) {
		//if (!note.isDisplay) continue;

		if (note.noteSprite != nullptr) {
			
			note.noteSprite->Draw();
		}
		
	}

	//part_->Draw();

}
