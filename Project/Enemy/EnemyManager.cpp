#include "EnemyManager.h"

#include <imgui.h>
#include <random>
#include <cassert>

#include "Player/Player.h"
#include "VectorCalculation.h"
#include "SingleCalculation.h"
#include "CollisionCalculation.h"
#include "PushBackCalculation.h"
#include "Input.h"
#include "Audio.h"
#include "TextureManager.h"
#include "LevelDataManager.h"
#include "WindowsSetup.h"

#include "StrongEnemy/State/StrongEnemyNoneMove.h"
#include "StrongEnemy/State/StrongEnemyMove.h"
#include "StrongEnemy/State/StrongEnemyTracking.h"

#include "NormalEnemy/State/NormalEnemyNoneMove.h"
#include "NormalEnemy/State/NormalEnemyMove.h"
#include "NormalEnemy/State/NormalEnemyPreTracking.h"
#include "NormalEnemy/State/NormalEnemyAttack.h"
#include <Easing.h>

EnemyManager::EnemyManager(){
	//インスタンスの取得
	//レベルデータ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
	//オーディオ
	audio_ = Elysia::Audio::GetInstance();
	//テクスチャ管理クラス
	textureManager_ = Elysia::TextureManager::GetInstance();
	//ウィンドウクラス
	windowSetup_ = Elysia::WindowsSetup::GetInstance();
}


void EnemyManager::Initialize(const uint32_t& normalEnemyModel, const uint32_t& strongEnemyModel, const std::vector<Vector3>& normalEnemyPositions, const std::vector<Vector3>& strongEnemyPositions){
	
	//空だったら引っかかるようにしている
	assert(player_);
	assert(levelDataManager_);

	//モデルハンドルを代入
	//通常
	normalEnemyModelHandle_ = normalEnemyModel;
	//強敵
	strongEnemyModelHandle_ = strongEnemyModel;

	//通常の敵の生成
	for (size_t i = 0u; i < normalEnemyPositions.size(); ++i) {
		//生成
		GenerateNormalEnemy(normalEnemyPositions[i]);
	}

	//プレイ会の時に強敵入れない方が良いとの感想があったので生成はしない
	//でもいつでも生成できるよ
	//レベルエディタ側でやっておく
	for (size_t i = 0u; i < strongEnemyPositions.size(); ++i) {
		//生成
		GenerateStrongEnemy(strongEnemyPositions[i]);
	}


	//警告
	uint32_t warningTextureHandle = textureManager_->Load("Resources/Sprite/Warning/Warning.png");
	//生成
	warningSprite_.reset(Elysia::Sprite::Create(warningTextureHandle, warningTexturePosition_));
	//アンカーポイントの設定
	warningSprite_->SetAnchorPoint(WARNING_TEXTURE_ANCHOR_POINT_);
	//座標の再設定
	warningTexturePosition_ = { .x = static_cast<float_t>(windowSetup_->GetClientWidth())/2.0f,.y=650.0f };
	warningSprite_->SetPosition(warningTexturePosition_);

	//接近BGMの設定
	audioHandle_ = audio_->Load("Resources/Audio/Enemy/TrackingToPlayer.mp3");
}

void EnemyManager::Update(){

	//プレイヤーの座標
	Vector3 playerPosition = player_->GetWorldPosition();

	//通常の敵
	for (const std::unique_ptr <NormalEnemy>& enemy : enemies_) {
		
		//現在の状態
		std::string currentState = enemy->GetCurrentStateName();

		//プレイヤーの位置を設定
		enemy->SetPlayerPosition(playerPosition);
		//更新
		enemy->Update();

		//敵との最短距離を求める
		//プレイヤーと鍵の差分
		Vector3 playerAndKeydifference = VectorCalculation::Subtract(player_->GetWorldPosition(), enemy->GetWorldPosition());
		//距離
		float_t distance = SingleCalculation::Length(playerAndKeydifference);

		// 距離と敵の座標をペアで保存
		ClosestEnemyInformation newClosestEnemyInformation = { .position = enemy->GetWorldPosition(),.direction = enemy->GetMoveDirection() };
		enemyDistancePairs.push_back({ distance,newClosestEnemyInformation });
		
		//レベルエディタから持ってくる
		//AABB
		std::vector<AABB> aabbs = levelDataManager_->GetObjectAABBs(levelDataHandle_, "Stage");
		//コライダーを持っているかどうか
		std::vector<bool> colliders = levelDataManager_->GetIsHavingColliders(levelDataHandle_, "Stage");

		//衝突めり込み防止処理
		for (size_t i = 0u; i < aabbs.size(); ++i) {

			//オブジェクトのAABB
			AABB objectAABB = aabbs[i];
			// AABBを取得
			AABB enemyAABB = enemy->GetAABB();

			//お互いのAABBが接触している場合
			if (CollisionCalculation::IsCollisionAABBPair(enemyAABB, objectAABB) &&
				colliders[i] == true ) {

				// 敵とオブジェクトの中心座標を計算
				Vector3 objectCenter = VectorCalculation::Add(objectAABB.min, objectAABB.max);
				objectCenter.x *= 0.5f;
				objectCenter.y *= 0.5f;
				objectCenter.z *= 0.5f;

				// X軸とZ軸の距離を計算
				float distanceX = std::min<float>(abs(enemyAABB.max.x - objectAABB.min.x), abs(enemyAABB.min.x - objectAABB.max.x));
				float distanceZ = std::min<float>(abs(enemyAABB.max.z - objectAABB.min.z), abs(enemyAABB.min.z - objectAABB.max.z));

				//前の座標
				Vector3 enemyPrePosition = enemy->GetWorldPosition();

				//押し戻し分
				float PUSH_BACK_DISTANCE = 0.1f;

				// 衝突した時の反転処理
				if (distanceX < distanceZ) {
					//X軸の距離が短いとX軸の反転
					//通常の移動時だけ反転させる
					if (currentState == "Move") {
						enemy->GetCurrentState()->InverseDirectionX();
					}
					
					//X軸の押し戻し処理
					//押し戻した後に新しく座標を設定
					if (enemyPrePosition.x < objectCenter.x) {
						enemy->SetTranslate({ enemyAABB.min.x - PUSH_BACK_DISTANCE, enemyPrePosition.y, enemyPrePosition.z });
					}
					else {
						enemy->SetTranslate({ enemyAABB.max.x + PUSH_BACK_DISTANCE, enemyPrePosition.y, enemyPrePosition.z });
					}
				}
				else {
					//Z軸の距離が短いとZ軸の反転
					//通常の移動時だけ反転させる
					if (currentState == "Move") {
						enemy->GetCurrentState()->InverseDirectionZ();
					}
					

					//Z軸の押し戻し処理
					//押し戻した後に新しく座標を設定
					if (enemyPrePosition.z < objectCenter.z) {
						enemy->SetTranslate({ enemyPrePosition.x, enemyPrePosition.y, enemyAABB.min.z - PUSH_BACK_DISTANCE });
					}
					else {
						enemy->SetTranslate({ enemyPrePosition.x, enemyPrePosition.y, enemyAABB.max.z + PUSH_BACK_DISTANCE });
					}
				}




			}
		}
		
		//プレイヤーと敵の差分ベクトル
		Vector3 defference = VectorCalculation::Subtract(playerPosition, enemy->GetWorldPosition());
		//距離
		float_t defferenceDistance = SingleCalculation::Length(defference);

		//通常の動き
		if (currentState == "Move") {
			//近くなったら追跡準備
			if (defferenceDistance < TRACKING_START_DISTANCE_) {
				//追跡準備へ
				enemy->ChengeState(std::make_unique<NormalEnemyPreTracking>());
			}
		}
		//追跡
		else if (currentState == "Tracking") {
			if (defferenceDistance <= ATTACK_START_DISTANCE_) {
				//攻撃
				enemy->ChengeState(std::make_unique<NormalEnemyAttack>());
			}
			//距離が離れたら通常の動きへ
			if (defferenceDistance >= TRACKING_START_DISTANCE_) {
				//追跡準備へ
				enemy->ChengeState(std::make_unique<NormalEnemyMove>());
			}

		}
		//攻撃
		else if (currentState == "Attack") {

			//攻撃中にプレイヤーが離れた時
			if (defferenceDistance > ATTACK_START_DISTANCE_&& enemy->GetIsAttackAnimation() == false) {
				//通常の動き
				enemy->ChengeState(std::make_unique<NormalEnemyMove>());
			}
		}

	}
	
	//警告
	Warning();
	

	//強敵の更新
	for (const std::unique_ptr<StrongEnemy>& strongEnemy : strongEnemies_) {
		//一発アウトの敵の更新
		strongEnemy->Update();
		//プレイヤーの座標を設定
		strongEnemy->SetPlayerPosition(playerPosition);

		//AABBの取得
		//通常
		AABB strongEnemyAABB = strongEnemy->GetAABB();
		//強敵
		AABB enemyAABB = strongEnemy->GetAABB();

		//移動中の時
		if (strongEnemy->GetCondition() == EnemyCondition::Move) {
			//レベルエディタから持ってくる
			//座標
			std::vector<Vector3> positions = levelDataManager_->GetObjectPositions(levelDataHandle_,"Stage");
			//AABB
			std::vector<AABB> aabbs = levelDataManager_->GetObjectAABBs(levelDataHandle_,"Stage");
			//コライダーを持っているかどうか
			std::vector<bool> colliders = levelDataManager_->GetIsHavingColliders(levelDataHandle_, "Stage");
			//衝突判定
			for (size_t i = 0; i < positions.size() && i < aabbs.size() && i < colliders.size(); ++i) {

				//AABBを取得
				AABB objectAABB = aabbs[i];
				//位置を取得
				Vector3 objectPosition = positions[i];


				//お互いのAABBが接触している場合
				if (((enemyAABB.max.x > objectAABB.min.x) && (enemyAABB.min.x < objectAABB.max.x)) &&
					((enemyAABB.max.z > objectAABB.min.z) && (enemyAABB.min.z < objectAABB.max.z))) {
					//オブジェクトとの差分ベクトル
					Vector3 defference = VectorCalculation::Subtract(objectPosition, strongEnemy->GetWorldPosition());
					//正規化
					Vector3 normalizedDefference = VectorCalculation::Normalize(defference);


					//敵の向いている方向
					Vector3 enemyDirection = strongEnemy->GetMoveDirection();

					//前にある場合だけ計算
					float dot = SingleCalculation::Dot(enemyDirection, normalizedDefference);

					//進行方向上にあるときだけ計算する
					if (dot > FRONT_DOT_) {

						//差分ベクトルのXとZの大きさを比べ
						//値が大きい方で反転させる
						float_t defferenceValueX = std::abs(defference.x);
						float_t defferenceValueZ = std::abs(defference.z);


						//X軸反転
						if (defferenceValueX >= defferenceValueZ) {
							strongEnemy->InvertDirectionX();
						}
						//Z軸反転
						else {
							strongEnemy->InvertDirectionZ();
						}
					}
				}

			}
		}

		//差分を求める
		Vector3 playerStrongEnemyDifference = VectorCalculation::Subtract(playerPosition, strongEnemy->GetWorldPosition());
		//距離を求める
		float_t playerStrongEnemyDistance = SingleCalculation::Length(playerStrongEnemyDifference);
		//正規化
		Vector3 directionToPlayer = VectorCalculation::Normalize(playerStrongEnemyDifference);


		//大きさの処理
		//追跡開始する距離と強敵とプレイヤーの距離の割合で音量を決める
		float_t volume = MAX_VOLUME_ - (playerStrongEnemyDistance / STRONG_ENEMY_TRACKING_START_DISTANCE_);

		//0だったら鳴らす意味はないので止めておく
		const float_t MIN_VOLUME = 0.0f;
		if (volume < MIN_VOLUME) {
			audio_->Stop(audioHandle_);
		}
		else {
			//再生
			audio_->Play(audioHandle_, true);
		}

		//音量変化
		audio_->ChangeVolume(audioHandle_, volume);


#ifdef _DEBUG
		ImGui::Begin("強敵");
		ImGui::InputFloat("距離", &volume);
		ImGui::InputFloat3("プレイヤーとの方向", &directionToPlayer.x);
		ImGui::InputFloat3("プレイヤーとの差分", &playerStrongEnemyDifference.x);
		ImGui::InputFloat("プレイヤーとの距離", &playerStrongEnemyDistance);
		ImGui::End();
#endif // _DEBUG


		//設定した距離より小さくなると追跡
		if (playerStrongEnemyDistance <= STRONG_ENEMY_TRACKING_START_DISTANCE_) {
			//追跡に移行
			strongEnemy->ChangeState(std::make_unique<StrongEnemyTracking>());
		}
		else {
			//通常の動きに移行
			strongEnemy->ChangeState(std::make_unique<StrongEnemyMove>());
		}
	}

	//入っているものを全てクリアする
	enemyDistancePairs.clear();

#ifdef _DEBUG
	//ImGui表示用
	DisplayImGui();
#endif // _DEBUG

}

void EnemyManager::DrawObject3D(const Camera& camera,const SpotLight& spotLight){

	//描画(通常)
	for (const std::unique_ptr <NormalEnemy>& enemy : enemies_) {
		enemy->Draw(camera,spotLight);
	}

	//描画(強敵)
	for (const std::unique_ptr<StrongEnemy>&  strongEnemy : strongEnemies_) {
		strongEnemy->Draw(camera, spotLight);
	}

}

void EnemyManager::DrawSprite(){
	//警告
	if (enemies_.size() > 0u) {
		warningSprite_->Draw();
	}
	
}

void EnemyManager::GenerateNormalEnemy(const Vector3& position) {
	//通常の敵の生成
	std::unique_ptr<NormalEnemy> enemy = std::make_unique<NormalEnemy>();

	//方向決め
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
	std::uniform_real_distribution<float> speedDistribute(-1.0f, 1.0f);
	Vector3 direction = { .x = speedDistribute(randomEngine),.y = 0.0f,.z = speedDistribute(randomEngine) };

	//初期化
	enemy->Initialize(normalEnemyModelHandle_, position, direction);
	enemies_.push_back(std::move(enemy));
}

void EnemyManager::GenerateStrongEnemy(const Vector3& position) {
	////強敵の生成
	std::unique_ptr<StrongEnemy> enemy = std::make_unique<StrongEnemy>();

	//方向を決める
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
	std::uniform_real_distribution<float_t> speedDistribute(-1.0f, 1.0f);
	Vector3 direction = { .x = speedDistribute(randomEngine),.y = 0.0f,.z = speedDistribute(randomEngine) };

	//初期化
	enemy->Initialize(strongEnemyModelHandle_, position, direction);
	enemy->SetTrackingStartDistance(STRONG_ENEMY_TRACKING_START_DISTANCE_);
	//挿入
	strongEnemies_.push_back(std::move(enemy));
}

void EnemyManager::DeleteEnemy() {
	//敵が生存していなかったら消す
	enemies_.remove_if([](const std::unique_ptr<NormalEnemy>& enemy) {
		//スマートポインタの場合はこれだけで良いよ
		//勿論こっちもdeleteが無くなってすっきりだね!
		return enemy->GetIsDeleted();
	});
}

void EnemyManager::StopAudio() {
	audio_->Stop(audioHandle_);
}

void EnemyManager::Warning(){

	//最短距離を持つペアを探す
	auto minIt = std::min_element(enemyDistancePairs.begin(), enemyDistancePairs.end(),
		[](const auto& a, const auto& b) {
			return a.first < b.first;
	});

	//最短だった場合を記録
	if (minIt != enemyDistancePairs.end()) {
		closestNormalEnemyDistance_ = minIt->first;
		closestEnemyInformation_.position = minIt->second.position;
		closestEnemyInformation_.direction = minIt->second.direction;
	}

	//内積を計算
	playerAndNormalEnemyDot_ = SingleCalculation::Dot(closestEnemyInformation_.direction, player_->GetLightDirection());
	//プレイヤーの背後にいたら警告表示をするようにする
	if (playerAndNormalEnemyDot_ >= BACK_DOT_VALUE_ && closestNormalEnemyDistance_ <= TRACKING_START_DISTANCE_) {
		warningSprite_->SetInvisible(false);
	}
	else {
		warningSprite_->SetInvisible(true);
	}

	//拡縮
	warningScaleT_ += SCALE_T_INCREASE_VALUE_;
	if (warningScaleT_ > MAX_SCALE_T_) {
		warningScaleT_ = MIN_SCALE_T_;
	}

	//線形補間とイージングを使い警告感を出す
	float_t newWarningScaleT_ = Easing::EaseInOutCubic(warningScaleT_);
	warningTextureScale_ = SingleCalculation::Lerp(MAX_WARNING_SCALE_, MIN_WARNING_SCALE_, newWarningScaleT_);
	warningSprite_->SetScale({ .x = warningTextureScale_ ,.y = warningTextureScale_ });
}

void EnemyManager::DisplayImGui(){

	ImGui::Begin("敵管理クラス");
	ImGui::SliderFloat2("警告座標", &warningTexturePosition_.x, 0.0f, 720.0f);
	ImGui::InputFloat("内積", &playerAndNormalEnemyDot_);
	if (ImGui::TreeNode("最短") == true) {
		ImGui::InputFloat("距離", &closestNormalEnemyDistance_);
		ImGui::InputFloat3("座標", &closestEnemyInformation_.position.x);
		ImGui::InputFloat3("方向", &closestEnemyInformation_.direction.x);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("プレイヤー") == true) {
		Vector3 direction = player_->GetMoveDirection();
		ImGui::InputFloat3("方向", &direction.x);
		
		ImGui::TreePop();
	}
	ImGui::End();
}
