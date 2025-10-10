#include "NormalEnemy.h"

#include <imgui.h>
#include <numbers>

#include "CollisionConfig.h"
#include "VectorCalculation.h"
#include "SpotLight.h"
#include "SingleCalculation.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "AnimationManager.h"
#include "GlobalVariables.h"
#include "State/NormalEnemyMove.h"

NormalEnemy::NormalEnemy() {
	//通常
	typeName_ = "Normal";

	//インスタンスの取得
	//グローバル変数クラス
	globalVariables_ = Elysia::GlobalVariables::GetInstance();
}

void NormalEnemy::Initialize(const uint32_t& modelHandle, const Vector3& position, const Vector3& speed) {

	//モデルの生成
	animationmodel_.reset(Elysia::AnimationModel::Create(modelHandle));

	//スキニングアニメーション読み込み
	skeleton_.Create(Elysia::ModelManager::GetInstance()->GetModelData(modelHandle).rootNode);
	skinCluster_.Create(skeleton_, Elysia::ModelManager::GetInstance()->GetModelData(modelHandle));
	animationHandle_ = Elysia::AnimationManager::GetInstance()->LoadFile("Resources/LevelData/GameStage/Ghost","Ghost.gltf");

	//感電パーティクル用
	thunderTextureHandle_ = Elysia::TextureManager::Load("Resources/Sprite/Particle/Thunder.png");


	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	//スケールサイズ
	const float SCALE_SIZE = 0.65f;
	worldTransform_.scale = { .x = SCALE_SIZE,.y = SCALE_SIZE ,.z = SCALE_SIZE };

	//座標の代入
	worldTransform_.translate = position;

	//マテリアルの初期化
	material_.Initialize();
	material_.lightingKinds = SpotLighting;
	//パーティクル
	particleMaterial_.Initialize();
	particleMaterial_.lightingKinds = NoneLighting;


	//生存か死亡
	isAlive_ = true;
	deleteTime_ = 180;

	//追跡かどうか
	isTracking_ = false;

	//スピード
	speed_ = speed;

	//状態
	preCondition_ = EnemyCondition::NoneMove;
	condition_ = EnemyCondition::Move;

	//体力
	hp_ = HPCondition::Normal;


	//デバッグ用のモデル
	debugModelHandle = Elysia::ModelManager::GetInstance()->Load("Resources/Model/Sample/Sphere", "Sphere.obj");

	//攻撃の当たり判定
	attackCollision_ = std::make_unique<EnemyAttackCollision>();
	attackCollision_->Initialize(debugModelHandle);

	//懐中電灯に対する当たり判定
	enemyFlashLightCollision_ = std::make_unique<EnemyFlashLightCollision>();
	enemyFlashLightCollision_->Initialize();


	//グローバル変数
	globalVariables_->CreateGroup(GROUNP_NAME_);
	//振動
	globalVariables_->AddItem(GROUNP_NAME_, "ShakeOffset", shakeOffset_);
	shakeOffset_ = globalVariables_->GetFloatValue(GROUNP_NAME_, "ShakeOffset");

	//状態
	currentState_ = std::make_unique<NormalEnemyMove>();
	currentState_->Initialize();
	currentStateName_ = currentState_->GetStateName();


}

void NormalEnemy::Update() {

	//方向を取得
	direction_ = currentState_->GetMoveDirection();

	//向きを計算しモデルを回転させる
	float_t directionToRotateY = std::atan2f(-direction_.z, direction_.x);
	//回転のオフセット
	//元々のモデルの回転が変だったのでこれを足している
	const float_t ROTATE_OFFSET = -std::numbers::pi_v<float_t> / 2.0f;
	worldTransform_.rotate.y = directionToRotateY + ROTATE_OFFSET;


	//ワールドトランスフォームの更新
	worldTransform_.Update();
	//マテリアルの更新
	material_.Update();
	particleMaterial_.Update();


	//AABBの計算
	aabb_.max = VectorCalculation::Add(GetWorldPosition(), RADIUS_INTERVAL_);
	aabb_.min = VectorCalculation::Subtract(GetWorldPosition(), RADIUS_INTERVAL_);


	//当たり判定
	//懐中電灯
	enemyFlashLightCollision_->SetEnemyPosition(GetWorldPosition());
	enemyFlashLightCollision_->Update();

	//攻撃
	attackCollision_->SetEnemyPosition(GetWorldPosition());
	attackCollision_->SetEnemyDirection(direction_);
	attackCollision_->Update();


	//アニメーション適用
	Elysia::AnimationManager::GetInstance()->ApplyAnimation(skeleton_, animationHandle_, animationTime_);
	
	//スケルトン、クラスターを更新
	skeleton_.Update();
	skinCluster_.Update(skeleton_);

	//ダメージ演出
	Damaged();

	//生存している時だけ行動するよ
	if (isAlive_ == true) {
		//状態の更新
		currentState_->Update(this);
	}
	else {
		//生成中止
		if (electricShockParticle_ != nullptr) {
			electricShockParticle_->SetIsStopGenerate(true);

		}
		
		//死亡したらパーティクルを出して消える
		Delete();
	}

	//ImGui
	DisplayImGui();
}

void NormalEnemy::Draw(const Camera& camera, const SpotLight& spotLight) {

#ifdef _DEBUG
	//あたり判定
	attackCollision_->Draw(camera, spotLight);
#endif // _DEBUG


	//本体
	animationmodel_->Draw(worldTransform_, camera, skinCluster_, material_, spotLight);

	//感電パーティクル
	if (electricShockParticle_ != nullptr) {
		electricShockParticle_->Draw(camera, particleMaterial_);
	}

	//死亡パーティクル
	if (deadParticle_ != nullptr) {
		deadParticle_->Draw(camera, particleMaterial_);
	}
}

NormalEnemy::~NormalEnemy() {
	globalVariables_->SaveFile(GROUNP_NAME_);
}

void NormalEnemy::ChengeState(std::unique_ptr<BaseNormalEnemyState> newState){
	//前回と違った場合だけ通す
	if (currentState_->GetStateName() != newState->GetStateName()) {
		//さっきまで入っていたものを入れる
		preStateName_ = currentState_->GetStateName();
		//新しく入る
		currentStateName_ = newState->GetStateName();
		//それぞれに新しく入れる
		currentState_ = std::move(newState);
		//初期化
		currentState_->Initialize();
	}
}

void NormalEnemy::Damaged() {

	//ダメージを受ける
	if (enemyFlashLightCollision_->GetIsTouched() == true&& isAcceptDamage_==true) {
		///体力を減らす
		hp_ -= damagedValue_;
		
	}

	//0になったら絶命
	if (hp_ <= HPCondition::Dead) {
		isAlive_ = false;
	}
	//瀕死
	else if (hp_ == HPCondition::Dangerous) {
		//ランダムエンジン
		std::random_device seedGenerator;
		std::mt19937 randomEngine(seedGenerator());
		std::uniform_real_distribution<float_t> distribute(-0.01f, 0.01f);
		
		//最初の感電。
		if (IsFirstElectricShock_ == false) {
			//線形補間のように加算しその値を振動の倍数として使う
			firstElectricShockT_ += FIRST_ELECTRIC_SHOCK_DELTA_TIME_;
			//感電状態になる
			isElectricShock_ = true;
			//振動の値
			Vector3 randomTranslate = { .x = distribute(randomEngine),.y = 0.0f,.z = distribute(randomEngine) };
			//振動の値をかける
			electricDamageShakeValue_ = VectorCalculation::Multiply(randomTranslate, (MAX_T_VALUE_ - firstElectricShockT_));
			if (firstElectricShockT_ > MAX_T_VALUE_) {
				//最初の振動済み
				IsFirstElectricShock_ = true;
				//感電状態になる
				isElectricShock_ = false;
			}
		}

		//生成
		if (electricShockParticle_ == nullptr) {
			//生成
			electricShockParticle_ = std::move(Elysia::Particle3D::Create(ParticleMoveType::Stay));
			//テクスチャの上書き
			electricShockParticle_->TextureOverride(thunderTextureHandle_);
			//パーティクルの細かい設定
			const float_t SCALE_SIZE = 0.7f;
			electricShockParticle_->SetScale({ .x = SCALE_SIZE,.y = SCALE_SIZE,.z = SCALE_SIZE });
			electricShockParticle_->SetCount(6u);
			const float_t FREQUENCY = 4.0f;
			electricShockParticle_->SetIsReleaseOnceMode(false);
			electricShockParticle_->SetFrequency(FREQUENCY);
			electricShockParticle_->SetIsToTransparent(true);
		}
		else {

			//感電パーティクルの放出している時
			if(electricShockParticle_->GetIsRelease() == true){
				//感電状態になる
				isElectricShock_ = true;
			}

			//感電
			if (isElectricShock_==true&& IsFirstElectricShock_==true) {
				//振動させる
				//線形補間のように加算しその値を振動の倍数として使う
				electricShockT_ += ELECTRIC_SHOCK_DELTA_TIME_;

				//振動の値
				Vector3 randomTranslate = { .x = distribute(randomEngine),.y = 0.0f,.z = distribute(randomEngine) };
				//振動の値をかける
				electricDamageShakeValue_ = VectorCalculation::Multiply(randomTranslate, (MAX_T_VALUE_ - electricShockT_));
				if (electricShockT_ > MAX_T_VALUE_) {
					//一旦振動を解除
					isElectricShock_ = false;
					electricShockT_ = 0.0f;
				}
			}
			
		}
		//振動分の値を座標に加算
		worldTransform_.translate = VectorCalculation::Add(GetWorldPosition(), electricDamageShakeValue_);
		//座標の設定
		electricShockParticle_->SetTrackingPosition(GetWorldPosition());
	}

}

void NormalEnemy::Delete() {
	
	//消えていくよ
	const float_t DELETE_INTERVAL = 0.01f;
	material_.color.w -= DELETE_INTERVAL;

	//縮小
	const float_t SCALE_DOWN_VALUE = -0.1f;
	worldTransform_.scale.x += SCALE_DOWN_VALUE;
	worldTransform_.scale.y += SCALE_DOWN_VALUE;
	worldTransform_.scale.z += SCALE_DOWN_VALUE;

	//反転しないようにする
	if (worldTransform_.scale.z < 0.0f) {
		worldTransform_.scale.x = 0.0f;
		worldTransform_.scale.y = 0.0f;
		worldTransform_.scale.z = 0.0f;
	}

	//生成
	if (deadParticle_ == nullptr) {
		//生成
		deadParticle_ = std::move(Elysia::Particle3D::Create(ParticleMoveType::Rise));
		//パーティクルの細かい設定
		deadParticle_->SetTranslate(GetWorldPosition());
		const float_t SCALE_SIZE = 0.8f;
		deadParticle_->SetScale({ .x = SCALE_SIZE,.y = SCALE_SIZE,.z = SCALE_SIZE });
		deadParticle_->SetCount(20u);
		deadParticle_->SetIsReleaseOnceMode(true);
		deadParticle_->SetIsToTransparent(true);
	}

	//全て消えたら、消えたかどうかのフラグがたつ
	if (deadParticle_->GetIsAllInvisible() == true) {
		isDeleted_ = true;
	}

}

void NormalEnemy::DisplayImGui() {
#ifdef _DEBUG

	ImGui::Begin("敵");
	ImGui::InputInt("体力", &hp_);
	ImGui::InputFloat3("方向", &direction_.x);
	ImGui::Checkbox("攻撃", &isAttack_);
	ImGui::Checkbox("生存", &isAlive_);

	if (ImGui::TreeNode("状態")==true) {
		int newCondition = static_cast<int>(condition_);
		int newPreCondition = static_cast<int>(preCondition_);
		ImGui::InputInt("現在", &newCondition);
		ImGui::InputInt("前", &newPreCondition);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("感電") == true) {
		ImGui::InputFloat("最初の感電のT", &firstElectricShockT_);
		ImGui::InputFloat3("振動の値", &electricDamageShakeValue_.x);
		ImGui::TreePop();
	}


	ImGui::InputFloat4("色", &material_.color.x);
	ImGui::InputFloat3("座標", &worldTransform_.translate.x);
	ImGui::InputFloat3("追跡前の座標", &preTrackingPosition_.x);
	ImGui::InputInt("AliveTive", &deleteTime_);
	ImGui::End();
#endif // _DEBUG


}


