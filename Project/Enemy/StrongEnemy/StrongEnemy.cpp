#include "StrongEnemy.h"

#include <imgui.h>
#include <numbers>

#include "Camera.h"
#include "SpotLight.h"
#include "VectorCalculation.h"
#include "CollisionConfig.h"
#include "ModelManager.h"


#include "State/StrongEnemyNoneMove.h"
#include "State/StrongEnemyMove.h"
#include "State/StrongEnemyTracking.h"

StrongEnemy::StrongEnemy() {
	//強敵
	typeName_ = "Strong";
}

void StrongEnemy::Initialize(const uint32_t& modelHandle,const Vector3& position,const Vector3& speed){

	//モデル
	model_.reset(Elysia::Model::Create(modelHandle));

	//ワールドトランスフォーム
	worldTransform_.Initialize();
	const float SCALE_SIZE = 2.0f;
	worldTransform_.scale = { .x = SCALE_SIZE ,.y = SCALE_SIZE ,.z = SCALE_SIZE };
	worldTransform_.translate = position;


	//スピード
	speed_ = speed;

	//マテリアル
	material_.Initialize();
	//ライティングの種類
	material_.lightingKinds = SpotLighting;


	//デバッグ用モデル
	uint32_t collisionDebugModel = Elysia::ModelManager::GetInstance()->Load("Resources/Model/Sample/Cube","cube.obj");

	//プレイヤーに対してのコリジョン
	collisionToPlayer_ = std::make_unique<StrongEnemyCollisionToPlayer>();
	collisionToPlayer_->Initialize(collisionDebugModel);


	//行動パターン
	//最初は通常の動き
	currentState_ = std::make_unique<StrongEnemyMove>();
	currentState_->Initialize();
}


void StrongEnemy::Update(){
	//状態の更新
	currentState_->Update(this);

	//方向を取得
	Vector3 direction = currentState_->GetMoveDirection();
	direction_ = direction;

	//向きを計算しモデルを回転させる
	float directionToRotateY = std::atan2f(-direction.z, direction.x);
	//修正
	const float ROTATE_OFFSET = -std::numbers::pi_v<float> / 2.0f;
	//加算
	worldTransform_.rotate.y = directionToRotateY + ROTATE_OFFSET;

	//ワールドトランスフォームの更新
	worldTransform_.Update();

	//マテリアルの更新
	material_.Update();


	//更新
	collisionToPlayer_->Update();
	collisionToPlayer_->SetEnemyPosition(worldTransform_.GetWorldPosition());


#ifdef _DEBUG
	ImGui::Begin("強敵");
	ImGui::InputFloat3("座標", &worldTransform_.translate.x);
	ImGui::End();
#endif // _DEBUG


}

void StrongEnemy::Draw(const Camera& camera,const SpotLight& spotLight){
	//描画
	model_->Draw(worldTransform_, camera, material_, spotLight);

#ifdef _DEBUG
	collisionToPlayer_->Draw(camera, spotLight);
#endif // _DEBUG
}


void StrongEnemy::ChangeState(std::unique_ptr<BaseStongEnemyState>newState){
	//前回と違った場合だけ通す
	if (currentState_->GetStateName() != newState->GetStateName()) {
		currentState_ = std::move(newState);
		//引数が次に遷移するシーン
		currentState_->Initialize();

	}
	
}


