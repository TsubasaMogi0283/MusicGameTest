#include "PoltergeistBone.h"
#include <LevelDataManager.h>
#include <Audio.h>
#include <ModelManager.h>
#include <VectorCalculation.h>
#include <SingleCalculation.h>
#include <imgui.h>
#include <Player/Player.h>
#include <Key/KeyManager.h>

PoltergeistBone::PoltergeistBone(){
	//インスタンスの取得
	//レベルデータ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
	//モデル管理クラス
	modelManager_ = Elysia::ModelManager::GetInstance();
	//オーディオ
	audio_ = Elysia::Audio::GetInstance();
}

void PoltergeistBone::Initialize() {
	//骨の欠片のモデルハンドル
	bonePieceParticleHandle_ = modelManager_->Load("Resources/Model/Particle/BonePiece", "BonePiece.obj");

	//骨の初期座標を取得
	bonePosition_ = levelDataManager_->GetInitialTranslate(levelDataHandle_, boneString_);
	//マテリアルの初期化
	bonePieceMaterial_.Initialize();
	bonePieceMaterial_.lightingKinds = LightingType::SpotLighting;

	//骨が壊れる音
	boneBreakAudioHandle_ = audio_->Load("Resources/Audio/Action/BoneBreak.mp3");
	//警告音
	warningBoneAudioHandle_ = audio_->Load("Resources/Audio/SE/BoneWarning.wav");
}

void PoltergeistBone::Update() {

	//状態遷移
	switch (movementState_) {
	case PoltergeistBoneMovement::BoneRise:
		//上昇
		//警告音再生
		if (isPlayWarningSE_ == false) {
			audio_->Play(warningBoneAudioHandle_, true);
			isPlayWarningSE_ = true;
		}

		//ふわふわ浮き上がる
		bonePosition_.y += FLOATING_AMOUNT_;

		//上がり切る
		if (bonePosition_.y > FLOATING_HEIGHT_) {
			//浮遊へ
			bonePosition_.y = FLOATING_HEIGHT_;
			movementState_ = PoltergeistBoneMovement::BoneFloat;

			isFinishRiseBone_ = true;
		}

		//浮遊状態の時はsin,cosを使ってふわふわ浮いている感じを出す
		//プレイヤーへの方向
		boneDirectionToPlayer_ = VectorCalculation::Subtract(player_->GetWorldPosition(), bonePosition_);

		//回転
		floatingTheta_ += ROTATE_THETA_VALUE_;
		//座標の更新
		bonePosition_.x += std::cosf(floatingTheta_) * ROTATE_VALUE_OFFSET_ + boneDirectionToPlayer_.x * BONE_SPEED_;
		bonePosition_.z += std::sinf(floatingTheta_) * ROTATE_VALUE_OFFSET_ + boneDirectionToPlayer_.z * BONE_SPEED_;

		break;

	case PoltergeistBoneMovement::BoneFloat:
		//上がり切った時に浮遊
		if (isFinishRiseBone_ == true) {
			//浮遊時間の加算
			floatingBoneTime_ += DELTA_TIME_;
			if (floatingBoneTime_ > FLOATING_TIME_) {
				isFinishRiseBone_ = false;
				movementState_ = PoltergeistBoneMovement::BoneReadyForDrop;
			}
		}
		//浮遊状態の時はsin,cosを使ってふわふわ浮いている感じを出す
		//プレイヤーへの方向
		boneDirectionToPlayer_ = VectorCalculation::Subtract(player_->GetWorldPosition(), bonePosition_);

		//回転
		floatingTheta_ += ROTATE_THETA_VALUE_;
		//座標の更新
		bonePosition_.x += std::cosf(floatingTheta_) * ROTATE_VALUE_OFFSET_ + boneDirectionToPlayer_.x * BONE_SPEED_;
		bonePosition_.z += std::sinf(floatingTheta_) * ROTATE_VALUE_OFFSET_ + boneDirectionToPlayer_.z * BONE_SPEED_;
		break;

	case PoltergeistBoneMovement::BoneReadyForDrop:
		//落下準備
		//高さの設定
		warningFrequencyRatio_ += DELTA_INCREASE_FREAQUENCY_RATION_VALUE_;
		audio_->ChangeFrequencyRatio(warningBoneAudioHandle_, warningFrequencyRatio_);

		//高速回転
		floatingTheta_ += RAPID_ROTATE_THETA_VALUE_;
		//準備時間を加算
		readyForDropTime_ += DELTA_TIME_;
		//指定した時間を過ぎたら落下へ
		if (readyForDropTime_ > READY_FOR_DROP_TIME_) {
			//落下へ
			movementState_ = PoltergeistBoneMovement::BoneDrop;
		}

		//プレイヤーの座標ロックオン
		if (isLockOn_ == false) {
			//プレイヤーの座標を取得
			loclOnPlayerPosition_ = player_->GetWorldPosition();
			//骨の座標を取得
			beforeBoneDropPosition_ = bonePosition_;
			isLockOn_ = true;
		}

		break;

	case PoltergeistBoneMovement::BoneDrop:
	{
		//骨が落下する
		//高速回転
		floatingTheta_ += RAPID_ROTATE_THETA_VALUE_;
		dropT_ += 0.03f;
		dropT_ = std::clamp(dropT_, 0.0f, 1.0f);
		//新しく変数を作って分かりやすくする
		Vector3 startPosition = beforeBoneDropPosition_;
		Vector3 endPosition = loclOnPlayerPosition_;
		//XZ軸は線形補間
		bonePosition_ = VectorCalculation::Lerp(startPosition, endPosition, dropT_);

		//骨との衝突判定
		Vector3 defference = VectorCalculation::Subtract(player_->GetWorldPosition(), bonePosition_);
		float_t distance = SingleCalculation::Length(defference);

		if (distance < 2.0f) {
			if (isTouchOnce_ == false) {
				//当たった
				player_->SetIsBoneTouch(true);
				isTouchOnce_ = true;
			}
			else {
				//一度当たったらダメージ処理は終わり
				player_->SetIsBoneTouch(false);
			}
		}
		else {
			player_->SetIsBoneTouch(false);
		}

		//地面に着いたら破片のパーティクルが出る
		if (bonePosition_.y <= GROUND_POSITION_Y) {
			bonePosition_.y = GROUND_POSITION_Y;
			//非表示にする
			levelDataManager_->SetInvisible(levelDataHandle_, boneString_, true);
			//パーティクルの生成
			if (bonePieceParticle_ == nullptr) {
				//生成
				bonePieceParticle_ = Elysia::Particle3D::Create(bonePieceParticleHandle_, ParticleMoveType::ThrowUp);
				//座標
				bonePieceParticle_->SetTranslate(bonePosition_);
				//数
				bonePieceParticle_->SetCount(bonePieceCount_);
				//一度きり
				bonePieceParticle_->SetIsReleaseOnceMode(true);
				//スケール
				bonePieceParticle_->SetScale({ BONE_PIECE_SCALE_,BONE_PIECE_SCALE_,BONE_PIECE_SCALE_ });
				//ベロシティの設定
				bonePieceParticle_->SetThrowUpVeloityY(THROW_UP_VELOCITY_Y_);

				//壊れる音
				audio_->Play(boneBreakAudioHandle_, false);
				//警告音停止
				audio_->Stop(warningBoneAudioHandle_);
			}
		}

		//全て消えたらtrue
		if (bonePieceParticle_ != nullptr && bonePieceParticle_->GetIsAllInvisible() == true) {
			movementState_ = PoltergeistBoneMovement::BoneProcessEnd;
		}
	}
		break;

	case PoltergeistBoneMovement::BoneProcessEnd:
		//処理終了
		break;
	}

	//回転の変更
	levelDataManager_->SetRotate(levelDataHandle_, boneString_, { .x = floatingTheta_,.y = floatingTheta_,.z = floatingTheta_ });
	//座標の変更
	levelDataManager_->SetTranslate(levelDataHandle_, boneString_, bonePosition_);
	//パーティクルのマテリアルを更新
	bonePieceMaterial_.Update();

#ifdef _DEBUG
	ImGui::Begin("ポルターガイスト(骨)");
	ImGui::SliderFloat("T", &dropT_, 0.0f, 1.0f);
	ImGui::InputFloat3("骨の座標", &bonePosition_.x);
	ImGui::InputFloat3("ロックオン座標(プレイヤー)", &loclOnPlayerPosition_.x);
	ImGui::InputFloat3("落下前の座標", &beforeBoneDropPosition_.x);
	ImGui::End();
#endif // _DEBUG

}

void PoltergeistBone::Draw(const Camera& camera, const SpotLight& spotlight) {
	//破片のパーティクル
	if(bonePieceParticle_ != nullptr) {
		bonePieceParticle_->Draw(camera, bonePieceMaterial_, spotlight);
	}
	
}