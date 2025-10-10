#include "Player.h"

#include <imgui.h>
#include <numbers>

#include "Input.h"
#include "VectorCalculation.h"
#include "SingleCalculation.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "SpotLight.h"
#include "LevelDataManager.h"
#include "PushBackCalculation.h"

Player::Player(){
	//インスタンスの取得
	//入力クラス
	input_ = Elysia::Input::GetInstance();
	//モデル管理クラス
	modelManager_ = Elysia::ModelManager::GetInstance();
	//テクスチャ管理クラス
	textureManager_ = Elysia::TextureManager::GetInstance();
	//レベルエディタ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
	//オーディオ
	audio_ = Elysia::Audio::GetInstance();
}

void Player::Initialize(){

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	const Vector3 INITIAL_POSITION = { .x=0.0f,.y=0.0f,.z=-15.0f };
	worldTransform_.translate = INITIAL_POSITION;

	//通常の敵
	std::unique_ptr<PlayerCollisionToNormalEnemyAttack> colliderToNormalEnemy = std::make_unique<PlayerCollisionToNormalEnemyAttack>();
	colliderToNormalEnemy->Initialize();
	colliders_.push_back(std::move(colliderToNormalEnemy));
	//強敵
	std::unique_ptr<PlayerCollisionToStrongEnemy> collisionToStrongEnemy = std::make_unique<PlayerCollisionToStrongEnemy>();
	collisionToStrongEnemy->Initialize();
	colliders_.push_back(std::move(collisionToStrongEnemy));
	//オーディオオブジェクト
	std::unique_ptr<PlayerCollisionToAudioObject> collosionToAudioObject= std::make_unique<PlayerCollisionToAudioObject>();
	collosionToAudioObject->Initialize();
	colliders_.push_back(std::move(collosionToAudioObject));

	//懐中電灯
	flashLight_ = std::make_unique<FlashLight>();
	flashLight_->Initialize();

	//カメラ
	eyeCamera_ = std::make_unique<PlayerEyeCamera>();
	eyeCamera_->Initialize();

	//マテリアル
	material_.Initialize();
	material_.lightingKinds = LightingType::SpotLighting;
	material_.color = { .x = 1.0f,.y = 1.0f,.z = 1.0f,.w = 1.0f };

	//UI
	uint32_t playerHPTextureHandle = textureManager_->Load("Resources/Sprite/Player/PlayerHP.png");
	uint32_t playerHPBackFrameTextureHandle = textureManager_->Load("Resources/Sprite/Player/PlayerHPBack.png");
	const Vector2 FRAME_INITIAL_POSITION = { .x = 20.0f,.y = 80.0f };
	//HPを生成
	for (uint32_t i = 0u; i < PLAYER_HP_MAX_QUANTITY_; ++i) {
		playerHpSprite_[i].reset(Elysia::Sprite::Create(playerHPTextureHandle, { .x = static_cast<float_t>(i) * 64 + FRAME_INITIAL_POSITION.x,.y = FRAME_INITIAL_POSITION.y }));
	}
	//フレームを生成
	playerHPBackFrameSprite_.reset(Elysia::Sprite::Create(playerHPBackFrameTextureHandle, FRAME_INITIAL_POSITION));

	//ダメージの効果音
	damagedSEHanle_ = audio_->Load("Resources/Audio/Action/DamagedSE.wav");

}

void Player::Update(){
	
	//移動処理
	Move();
	//攻撃を受ける
	Damaged();
	//体力
	for (uint32_t i = hp_; i < PLAYER_HP_MAX_QUANTITY_; ++i) {
		//非表示にする
		playerHpSprite_[i]->SetInvisible(true);
	}

	//ワールドトランスフォームの更新
	worldTransform_.translate = playerCenterPosition_;
	worldTransform_.translate.y = HEIGHT_;
	worldTransform_.Update();

	//AABBの計算
	aabb_.min = VectorCalculation::Subtract(worldTransform_.GetWorldPosition(), { SIDE_SIZE ,SIDE_SIZE ,SIDE_SIZE });
	aabb_.max = VectorCalculation::Add(worldTransform_.GetWorldPosition(), { SIDE_SIZE ,SIDE_SIZE ,SIDE_SIZE });

	//コリジョンの更新
	for (std::unique_ptr<BasePlayerCollision> &collision : colliders_) {
		//プレイヤーの座標を設定
		collision->SetPlayerPosition(worldTransform_.GetWorldPosition());
		//更新
		collision->Update();
	}

	//懐中電灯
	//角度はゲームシーンで取得する
	flashLight_->SetPlayerPosition(worldTransform_.GetWorldPosition());
	//目線の角度の設定
	flashLight_->SetTheta(theta_);
	flashLight_->SetPhi(-phi_);
	//更新
	flashLight_->Update();

	//カメラ(目)
	//座標の設定
	eyeCamera_->SetPlayerPosition(GetWorldPosition());
	//角度の設定
	eyeCamera_->SetTheta(theta_);
	eyeCamera_->SetPhi(phi_);
	//カメラ(目)の更新
	eyeCamera_->Update();
	//マテリアルの更新
	material_.Update();

	
#ifdef _DEBUG
	//ImGui表示
	DisplayImGui();
#endif
}

void Player::DrawObject3D(const Camera& camera, const SpotLight& spotLight){

#ifdef _DEBUG
	//コリジョンの描画
	for (std::unique_ptr<BasePlayerCollision>& collision : colliders_) {
		collision->Draw(camera, material_, spotLight);
	}

#endif // _DEBUG
	//懐中電灯
	flashLight_->DrawObject3D(camera);

}

void Player::DrawSprite(){
	//懐中電灯
	flashLight_->DrawSprite();
	//体力の枠
	playerHPBackFrameSprite_->Draw();
	//体力(アイコン型)
	for (uint32_t i = 0u; i < PLAYER_HP_MAX_QUANTITY_; ++i) {
		playerHpSprite_[i]->Draw();
	}
}

Player::~Player() {
	//振動を止める
	input_->StopVibration();
}

void Player::Damaged() {

	bool isAttacked = false;
	for (std::unique_ptr<BasePlayerCollision>& collision : colliders_) {
		if (collision->GetName() == "ToNormalEnemyAttack") {
			isAttacked = collision->GetIsTouch();
			break;
		}
	}

	//通常の敵に当たった場合
	if ((isAttacked == true && isDameged_ == false&& 
		isAcceptDamegeFromNoemalEnemy_==true)|| isBoneTouch_ == true) {
		//体力を減らす
		--hp_;
		//ダメージを受ける	
		isDameged_ = true;
	}

	//ダメージを受けた時
	if (isDameged_ == true) {
		//攻撃を受け付けないようにする
		isAcceptDamegeFromNoemalEnemy_ = false;
		//一時的にコントロールを失う
		isControll_ = false;
		//線形補間で振動処理をする
		vibeTime_ += DELTA_TIME;
		//線形補間を使い振動を減衰させる
		//振動の強さ
		float_t vibeStrength =  SingleCalculation::Lerp(MAX_VIBE_, MIN_VIBE_, vibeTime_);
		//振動の設定
		input_->SetVibration(vibeStrength, vibeStrength);

		//ダメージの効果音を流す
		audio_->Play(damagedSEHanle_, false);

		//振動を止める
		if (vibeStrength <= MIN_VIBE_) {
			//振動が止まる
			input_->StopVibration();
			//時間を戻す
			vibeTime_ = RESTART_TIME_;
			//ダメージを受けていないようにする
			isDameged_ = false;
			//コントロールを戻す
			isControll_ = true;
		}
	}

	//体力が0になったら死亡
	if (hp_ == 0u) {
		isAlive_ = false;
	}

}

void Player::Move() {
	//動けるときだけ加算
	if (isControll_ == true ) {
		float_t moveSpeed = 0.0f;
		//走っている時
		if (isDash_ == true) {
			moveSpeed = DASH_MOVE_SPEED_;
		}
		//通常の動きの時
		else {
			moveSpeed = NORMAL_MOVE_SPEED_;
		}
		//加算
		playerCenterPosition_ = VectorCalculation::Add(playerCenterPosition_, VectorCalculation::Multiply(moveDirection_, moveSpeed));
		
		//AABB
		std::vector<AABB> aabbs = levelDataManager_->GetObjectAABBs(levelHandle_,"Stage");
		//コライダーを持っているかどうか
		std::vector<bool> colliders = levelDataManager_->GetIsHavingColliders(levelHandle_, "Stage");
		//衝突判定
		for (size_t i = 0u; i < aabbs.size(); ++i) {
			//コライダーを持っているときだけ
			if (colliders[i] == true) {
				//押し戻し処理
				PushBackCalculation::FixPosition(playerCenterPosition_, aabb_, aabbs[i]);
			}
		}

	}
}

void Player::DisplayImGui() {

	//それぞれintに変換
	int32_t keyQuantity = static_cast<int32_t>(haveKeyQuantity_);
	int32_t hpQuantity = static_cast<int32_t>(hp_);


	ImGui::Begin("プレイヤー");
	if (ImGui::TreeNode("状態")==true) {
		ImGui::InputFloat3("座標", &worldTransform_.translate.x);
		ImGui::InputFloat3("方向", &moveDirection_.x);
		ImGui::InputInt("鍵の数", &keyQuantity);
		ImGui::InputInt("体力", &hpQuantity);
		ImGui::Checkbox("振動", &isDameged_);
		ImGui::Checkbox("ダメージを受けたかどうか", &isDamage_);
		ImGui::Checkbox("敵からの攻撃を受け入れるか", &isAcceptDamegeFromNoemalEnemy_);
		ImGui::TreePop();
	}

	
	
	ImGui::End();

}

