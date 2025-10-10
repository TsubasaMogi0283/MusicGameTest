#include "StrongEnemyCollisionToPlayer.h"
#include "CollisionConfig.h"
#include "VectorCalculation.h"

void StrongEnemyCollisionToPlayer::Initialize(const uint32_t& modelHandle){

	//生成
	model_.reset(Elysia::Model::Create(modelHandle));

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	//マテリアルの初期化
	material_.Initialize();
	material_.lightingKinds = SpotLighting;

	//名前
	name_ = "ToPlayer";
	//当たり判定で使う種類
	collisionType_ = ColliderType::SphereType;
	//半径
	radius_ = 3.0f;

	//判定
	//自分
	SetCollisionAttribute(COLLISION_ATTRIBUTE_STRONG_ENEMY);
	//相手
	SetCollisionMask(COLLISION_ATTRIBUTE_PLAYER2);

}

void StrongEnemyCollisionToPlayer::Update(){

	//AABB
	aabb_.min = VectorCalculation::Subtract(enemyPosition_, { radius_,radius_, radius_ });
	aabb_.max = VectorCalculation::Add(enemyPosition_, { radius_,radius_, radius_ });


	//ワールドトランスフォームの更新
	worldTransform_.translate = enemyPosition_;
	worldTransform_.Update();
	//マテリアルの更新
	material_.Update();
}

void StrongEnemyCollisionToPlayer::Draw(const Camera& camera, const SpotLight& spotLight){
	//モデル
	model_->Draw(worldTransform_, camera, material_, spotLight);
	
}

void StrongEnemyCollisionToPlayer::OnCollision(){
	isTouchPlayer_ = true;
}

void StrongEnemyCollisionToPlayer::OffCollision(){
	isTouchPlayer_ = false;
}
