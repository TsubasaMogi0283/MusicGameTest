#include "KeyManager.h"

#include <imgui.h>
#include <stdlib.h>
#include <algorithm>

#include "TextureManager.h"
#include "Input.h"
#include "LevelDataManager.h"
#include "VectorCalculation.h"
#include "Player/Player.h"
#include "SingleCalculation.h"
#include "Easing.h"

KeyManager::KeyManager() {
	//インスタンスの取得
	//オーディオ
	audio_ = Elysia::Audio::GetInstance();
	//テクスチャ管理クラス
	textureManager_ = Elysia::TextureManager::GetInstance();
	//入力
	input_ = Elysia::Input::GetInstance();
	//レベルデータ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
}

void KeyManager::Initialize(const uint32_t& modelHandle, const std::vector<Vector3>& positions) {
	//プレイヤーが入っているかどうか
	assert(player_ != nullptr);
	//モデルハンドルの代入
	modelHandle_ = modelHandle;

	//小屋の中にある鍵の座標を取得
	Vector3 keyInHousePosition = levelDataManager_->GetInitialTranslate(levelDataHandle_, "KeyInHouse");
	for (size_t i = 0u; i < positions.size(); ++i) {
		//生成
		const float_t OFFSET_Y = 0.5f;
		bool isAbleToPickUp = true;
		Vector3 newPosition = { .x = positions[i].x,.y = OFFSET_Y ,.z = positions[i].z };
		Genarate(newPosition, isAbleToPickUp);
	}

	//読み込み
	//リスト
	uint32_t keyListSpriteHandle = textureManager_->Load("Resources/Sprite/Item/KeyList.png");
	//生成
	keyListSprite_.reset(Elysia::Sprite::Create(keyListSpriteHandle, initialPosition_));

	uint32_t textureHandle = Elysia::TextureManager::GetInstance()->Load("Resources/Sprite/Item/Key.png");
	//サイズを取得
	keySpriteWidth_ = textureManager_->GetTextureWidth(textureHandle);
	keySpriteHeight_ = textureManager_->GetTextureHeight(textureHandle);

	//鍵
	const Vector2 INITIAL_SCALE = { .x = 0.0f,.y = 0.0f };
	const Vector2 ANCHOR_POINT = { .x = 0.5f,.y = 0.5f };
	initialPositionAddAnchorPoint = { .x = 20.0f + keySpriteWidth_ / 2.0f,.y = 10.0f + keySpriteHeight_ / 2.0f };
	for (uint32_t i = 0u; i < MAX_KEY_QUANTITY_; ++i) {
		Vector2 position = {
			.x = initialPositionAddAnchorPoint.x + keySpriteWidth_ * static_cast<float_t>(i),
			.y = initialPositionAddAnchorPoint.y
		};
		keySprites_[i].reset(Elysia::Sprite::Create(textureHandle, position));
		//アンカーポイントの設定
		keySprites_[i]->SetAnchorPoint(ANCHOR_POINT);
		//初期スケール
		keySprites_[i]->SetScale(INITIAL_SCALE);
	}

	//知らせる音の読み込み
	notificationSEHandle_ = audio_->Load("Resources/External/Audio/Key/Shake.mp3");
	//拾う音の読み込み
	pickUpSEHandle = audio_->Load("Resources/External/Audio/Key/PickUp.mp3");
	//皿の落ちる音の読み込み
	dropPlateSEHandle_ = audio_->Load("Resources/External/Audio/Plate/DropPlateSE.wav");

	//拾う画像の読み込み
	uint32_t pickUpTextureManager = textureManager_->Load("Resources/Sprite/Item/PickUpKey.png");
	//生成
	const Vector2 INITIAL_FADE_POSITION = { .x = 0.0f,.y = 0.0f };
	pickUpKey_.reset(Elysia::Sprite::Create(pickUpTextureManager, INITIAL_FADE_POSITION));

	//再生
	audio_->Play(notificationSEHandle_, true);
	//初期の音の設定
	const float_t INITIAL_VOLUME = 0.0f;
	audio_->ChangeVolume(notificationSEHandle_, INITIAL_VOLUME);
}


void KeyManager::Update() {

	//全ての要素を消す
	keyAndPlayerDistances_.clear();
	Vector3 keyInHousePosition = levelDataManager_->GetInitialTranslate(levelDataHandle_, "KeyInHouse");
	//鍵
	for (const std::unique_ptr<Key>& key : keies_) {

		//更新
		key->Update();
		//終点座標
		Vector2 endPosition = {
			.x = initialPosition_.x + keySpriteWidth_ * static_cast<float_t>(keyQuantity_),
			.y = initialPosition_.y
		};
		key->SetEndPosition(endPosition);
		//プレイヤーと鍵の差分
		Vector3 playerAndKeydifference = VectorCalculation::Subtract(player_->GetWorldPosition(), key->GetWorldPosition());
		//距離
		float_t distance = SingleCalculation::Length(playerAndKeydifference);
		//挿入
		keyAndPlayerDistances_.push_back(distance);
	}

	//全ての要素から一番小さい値を調べる
	auto minIt = std::min_element(keyAndPlayerDistances_.begin(), keyAndPlayerDistances_.end());
	//最短距離を求める
	float_t closestDistance = 0.0f;
	if (minIt != keyAndPlayerDistances_.end()) {
		closestDistance = (*minIt);
	}
	//最大の距離を超えないようにする
	if (closestDistance > MAX_DISTANCE_) {
		closestDistance = MAX_DISTANCE_;
	}
	//求めた値から音量設定をする
	float_t volume = 1.0f - (closestDistance / MAX_DISTANCE_);
	audio_->ChangeVolume(notificationSEHandle_, volume);

	//現在の鍵の数
	if (keies_.size() == 0u) {
		audio_->Stop(notificationSEHandle_);
	}

	//取得処理
	PickUp();
	//消去処理
	Delete();
#ifdef _DEBUG
	//ImGui表示用
	DisplayImGui();
#endif // _DEBUG
}

void KeyManager::DrawObject3D(const Camera& camera, const SpotLight& spotLight) {
	//鍵モデルの描画
	for (const std::unique_ptr<Key>& key : keies_) {
		//key;
		key->DrawModel(camera, spotLight);
	}
}

void KeyManager::DrawSprite() {
	//鍵の画像の描画
	keyListSprite_->Draw();

	//鍵(個別)のスプライト
	for (const std::unique_ptr<Key>& key : keies_) {
		key->DrawSprite();
	}

	//鍵を取得するかどうか
	if (isAbleToPickUpKey_ == true) {
		pickUpKey_->Draw();
	}


	//鍵
	for (uint32_t i = 0u; i < MAX_KEY_QUANTITY_; ++i) {
		keySprites_[i]->Draw();
	}


}

void KeyManager::Genarate(const Vector3& position, const bool& isAbleToPickUp) {
	//生成
	std::unique_ptr<Key> key = std::make_unique<Key>();
	//初期化
	key->Initialize(modelHandle_, position);
	//取得可能かどうかの設定
	key->SetisAbleToPickUp(isAbleToPickUp);
	//リストに入れる
	keies_.push_back(std::move(key));
}

void KeyManager::Delete() {
	//消去処理
	//外部の変数(ここではメンバ変数)とかを持ってきたいときは[]の中に=を入れよう！
	keies_.remove_if([=](const std::unique_ptr<Key>& key) {
		//拾われたら消す
		if (key->GetIsDelete() == true) {

			//墓場用
			Vector3 keyInCementeryPosition = levelDataManager_->GetInitialTranslate(levelDataHandle_, "KeyInCemetery");
			if (key->GetWorldPosition().x == keyInCementeryPosition.x &&
				key->GetWorldPosition().z == keyInCementeryPosition.z) {
				isPickUpKeyInCemetery_ = true;
			}

			//小屋用
			Vector3 keyInHousePosition = levelDataManager_->GetInitialTranslate(levelDataHandle_, "KeyInHouse");
			if (key->GetWorldPosition().x == keyInHousePosition.x &&
				key->GetWorldPosition().z == keyInHousePosition.z) {
				isPickUpKeyInHouse_ = true;
			}
			
			

			spriteTs_[keyQuantity_] += 0.01f;
			//スケール
			float_t newT = Easing::EaseOutBack(spriteTs_[keyQuantity_]);
			keySprites_[keyQuantity_]->SetScale({ .x = newT ,.y = newT });

			//回転
			float_t newTForRotate = Easing::EaseOutCubic(spriteTs_[keyQuantity_]);
			float_t rotate = SingleCalculation::Lerp(0.0f, -std::numbers::pi_v<float_t>*6.0f, newTForRotate);
			keySprites_[keyQuantity_]->SetRotate(rotate);
			if (spriteTs_[keyQuantity_] >= 1.0f) {
				++keyQuantity_;
				return true;
			}
		}
		return false;
	});

	//皿が落ちる音
	if (isPickUpKeyInHouse_ == true) {
		++dropPlateTime_;

		const uint32_t SE_PLAY_TIME = 60u * 1u;
		if (dropPlateTime_ == SE_PLAY_TIME) {
			audio_->Play(dropPlateSEHandle_, false);
		}
	}
}

void KeyManager::PickUp() {
	//鍵
	for (const std::unique_ptr<Key>& key : keies_) {

		//勿論取得されていない時と取得可能な時だけ受け付ける
		if (key->GetIsPickUp() == false &&key->GetIsAbleToPickUp()==true) {
			//判定は円で
			Vector3 distance = {
				.x = std::powf((player_->GetWorldPosition().x - key->GetWorldPosition().x), 2.0f),
				.y = 0.0f,
				.z = std::powf((player_->GetWorldPosition().z - key->GetWorldPosition().z), 2.0f),
			};

			//距離を求める
			float_t collisionDistance = sqrtf(distance.x + distance.y + distance.z);

			//範囲内にいれば入力を受け付ける
			if (collisionDistance <= player_->GetSideSize() + key->GetRadius()) {
				//取得可能
				key->SetIsPrePickUp(true);
				
				//SPACEキーまたはBボタンでで取得
				if (input_->IsPushKey(DIK_SPACE) == true|| input_->IsTriggerButton(XINPUT_GAMEPAD_B) == true) {
					//プレイヤーの持っているか鍵の数が増える
					player_->AddHaveKeyQuantity();
					//鍵が取得される
					key->PickedUp();
					//取得の音が鳴る
					audio_->Play(pickUpSEHandle, false);
					
				}
			}
			else {
				//取得可能を表示しない
				key->SetIsPrePickUp(false);
			}


		}
		else {
			//取得可能を表示しない
			key->SetIsPrePickUp(false);
		}
	}

	//listにあるKeyの中にある「isPrePickUp_」のどれか一つでもtrueになっていたらtrueを返す
	//trueの時に取得するかどうかのUIが出る
	isAbleToPickUpKey_ = std::any_of(keies_.begin(), keies_.end(), [](const std::unique_ptr<Key>& key) {
		return key->GetIsPrePickUp() == true;
	});



}

void KeyManager::DisplayImGui(){

	ImGui::Begin("鍵管理クラス");
	ImGui::Checkbox("墓場用", &isPickUpKeyInCemetery_);
	int newQuantity = static_cast<int32_t>(keyQuantity_);
	ImGui::InputInt("鍵の数", &newQuantity);
	ImGui::Checkbox("取得可能か", &isAbleToPickUpKey_);
	ImGui::End();
}


void KeyManager::StopAudio() {
	audio_->ChangeVolume(notificationSEHandle_, 0.0f);
}