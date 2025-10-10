#include "PlayerCollisionToNormalEnemyAttack.h"
#include <imgui.h>

#include "CollisionConfig.h"
#include "ModelManager.h"

void PlayerCollisionToNormalEnemyAttack::Initialize(){

	//モデルの生成
	uint32_t modelHandle = Elysia::ModelManager::GetInstance()->Load("Resources/Model/Sample/Sphere","Sphere.obj");
	model_.reset(Elysia::Model::Create(modelHandle));


	//初期化
	worldTransform_.Initialize();
	worldTransform_.scale = {.x= 1.0f,.y= 1.0f,.z= 1.0f };


	//名前の設定
	name_ = "ToNormalEnemyAttack";
	//種類
	collisionType_ = ColliderType::SphereType;
	//半径
	radius_ = 1.0f;
	//自分
	SetCollisionAttribute(COLLISION_ATTRIBUTE_PLAYER);
	//相手
	SetCollisionMask(COLLISION_ATTRIBUTE_ENEMY_ATTACK);

}

void PlayerCollisionToNormalEnemyAttack::Update(){

	//プレイヤーの座標を持ってくる
	worldTransform_.translate = playerPosition_;
	//ワールドトランスフォームの更新
	worldTransform_.Update();


#ifdef _DEBUG
	ImGui::Begin("敵からの攻撃"); 
	ImGui::Checkbox("接触", &isTouch_);
	ImGui::InputFloat3("座標", &playerPosition_.x);
	ImGui::End();
#endif // _DEBUG



}

void PlayerCollisionToNormalEnemyAttack::Draw(const Camera& camera,const Material& material,const SpotLight& spotLight){
#ifdef _DEBUG
	model_->Draw(worldTransform_, camera, material, spotLight);
#endif // _DEBUG

}


