#include "SurpriseEye.h"

#include <TextureManager.h>
#include <Audio.h>
#include <LevelDataManager.h>
#include <VectorCalculation.h>
#include <SingleCalculation.h>
#include <Player/Player.h>

SurpriseEye::SurpriseEye(){
	//インスタンス生成
	//テクスチャ管理クラス
	textureManager_ = Elysia::TextureManager::GetInstance();
	//オーディオ
	audio_ = Elysia::Audio::GetInstance();
	//レベルデータ
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
}


void SurpriseEye::Initialize() {
	assert(player_);

	//テクスチャ読み込み
	uint32_t textureHandle = textureManager_->Load("Resources/Sprite/Suprise/HorrorEye.jpg");
	//初期座標
	const Vector2 INITIAL_POSITION = { .x = 0.0f,.y = 0.0f };

	//生成
	eye_.reset(Elysia::Sprite::Create(textureHandle, INITIAL_POSITION));
	//非表示
	isInvisible_ = true;
	eye_->SetInvisible(isInvisible_);
	//コリジョンの座標を取得
	eventCollisionPosition_ = levelDataManager_->GetInitialTranslate(levelHandle_, "SurprisedEyeCollision");

	//音の読み込み
	surpriseSeHandle_ = audio_->Load("Resources/Audio/SE/SuprisedEye.wav");

#ifdef _DEBUG
	//本当にびっくりしたのでデバッグ時は薄くする
	eye_->SetTransparency(0.1f);
#endif // _DEBUG

	
}

void SurpriseEye::Update() {
	//表示中
	if (isInvisible_ == false) {

		//効果音再生
		if (isPlaySE_ == false) {
			audio_->Play(surpriseSeHandle_, false);
			isPlaySE_ = true;
		}
		//時間を加算
		displayTime_ += DELTA_TIME_;

		//指定した時間を超えたら非表示
		if (displayTime_ > MAX_DISPLAY_TIME_) {
			isInvisible_ = true;
			isEndProcess_ = true;;
		}
	}

	//処理が終わるまで通す
	if (isEndProcess_ == false) {
		CollisionProcess();
	}
	
	//非表示の設定
	eye_->SetInvisible(isInvisible_);
}

void SurpriseEye::DrawSprite() {
	//目
	eye_->Draw();
}

void SurpriseEye::CollisionProcess(){
	//距離を計算
	Vector3 defference = VectorCalculation::Subtract(player_->GetWorldPosition(), eventCollisionPosition_);
	float_t distance = SingleCalculation::Length(defference);

	if (distance <= DISTANCE_) {
		isInvisible_ = false;
	}
}
