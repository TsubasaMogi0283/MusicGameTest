#include "EnemyAttackCollision.h"

#include "CollisionConfig.h"
#include "VectorCalculation.h"


void EnemyAttackCollision::Initialize(const uint32_t& modelHandle){
	//モデルの生成
	model_.reset(Elysia::Model::Create(modelHandle));

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//マテリアルの初期化
	material_.Initialize();
	//ライティング
	material_.lightingKinds = SpotLighting;
	
	//計算の量が減るからね
	//種類
	collisionType_ = ColliderType::SphereType;
	//半径
	radius_ = 1.0f;

	//自分
	SetCollisionAttribute(COLLISION_ATTRIBUTE_ENEMY_ATTACK);
	//相手
	SetCollisionMask(COLLISION_ATTRIBUTE_PLAYER);


}

void EnemyAttackCollision::Update(){

	
	//新しく方向を計算する
	Vector3 newDirection = VectorCalculation::Multiply(enemyDirection_, INTERVAL);
	
	//座標の計算
	worldTransform_.translate = VectorCalculation::Add(enemyWorldPosition_, newDirection);
	
	//ワールドトランスフォームの更新
	worldTransform_.Update();
	//マテリアルの更新
	material_.Update();
}

void EnemyAttackCollision::Draw(const Camera& camera,const SpotLight& spotLight){

#ifdef _DEBUG
	//描画
	model_->Draw(worldTransform_, camera, material_, spotLight);
	
#endif // _DEBUG

}

Vector3 EnemyAttackCollision::GetWorldPosition(){
	return worldTransform_.GetWorldPosition();
}

void EnemyAttackCollision::OnCollision(){
	isTouch_ = true;
}

void EnemyAttackCollision::OffCollision(){
	isTouch_ = false;
}

