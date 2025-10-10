#include "FlashLightCollision.h"
#include <imgui.h>

#include "CollisionConfig.h"


void FlashLightCollision::Initialize(){

	//種類
	collisionType_ = ColliderType::FanType;

	//自分
	SetCollisionAttribute(COLLISION_ATTRIBUTE_FLASH_LIGHT);
	//相手
	SetCollisionMask(COLLISION_ATTRIBUTE_ENEMY2);


}

void FlashLightCollision::Update(){


}

Vector3 FlashLightCollision::GetWorldPosition(){
	//プレイヤーの座標を返す
	return playerWorldPosition_;
}

void FlashLightCollision::OnCollision(){
#ifdef _DEBUG
	ImGui::Begin("扇側衝突している");
	ImGui::End();

#endif // _DEBUG
}

void FlashLightCollision::OffCollision(){
#ifdef _DEBUG
	ImGui::Begin("扇側衝突していない");
	ImGui::End();
#endif // _DEBUG
}
