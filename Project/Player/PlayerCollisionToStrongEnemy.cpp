#include "PlayerCollisionToStrongEnemy.h"
#include "CollisionConfig.h"
#include <ModelManager.h>

void PlayerCollisionToStrongEnemy::Initialize(){

	//モデルの生成
	uint32_t modelHandle= Elysia::ModelManager::GetInstance()->Load("Resources/Model/Sample/Sphere", "Sphere.obj");
	model_.reset(Elysia::Model::Create(modelHandle));

	//初期化
	worldTransform_.Initialize();

	//名前の設定
	name_ = "ToStrongEnemy";

	//種類
	collisionType_ = ColliderType::SphereType;
	//半径
	radius_ = 1.0f;
	//自分
	SetCollisionAttribute(COLLISION_ATTRIBUTE_PLAYER2);
	//相手
	SetCollisionMask(COLLISION_ATTRIBUTE_STRONG_ENEMY);

}

void PlayerCollisionToStrongEnemy::Update(){
	//プレイヤーの座標を持ってくる
	worldTransform_.translate = playerPosition_;
	//更新
	worldTransform_.Update();
}

void PlayerCollisionToStrongEnemy::Draw(const Camera& camera,const Material& material,const SpotLight& spotLight){
#ifdef _DEBUG
	model_->Draw(worldTransform_, camera, material, spotLight);
#endif // _DEBUG

}
