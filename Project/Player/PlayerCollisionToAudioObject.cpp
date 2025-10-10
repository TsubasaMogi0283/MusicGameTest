#include "PlayerCollisionToAudioObject.h"
#include "CollisionConfig.h"
#include <imgui.h>

void PlayerCollisionToAudioObject::Initialize(){
	//名前の設定
	name_ = "ToAudioObject";
	
	//種類
	collisionType_ = ColliderType::PointType;
	//自分
	SetCollisionAttribute(COLLISION_ATTRIBUTE_PLAYER3);
	//相手
	SetCollisionMask(COLLISION_ATTRIBUTE_AUDIO_OBJECT);

}

void PlayerCollisionToAudioObject::Update(){

#ifdef _DEBUG
	ImGui::Begin("当たり判定(オーディオオブジェクト用)");
	ImGui::Checkbox("衝突",&isTouch_);
	ImGui::InputFloat3("座標", &playerPosition_.x);
	ImGui::End();
#endif // _DEBUG
}

void PlayerCollisionToAudioObject::Draw(const Camera& camera, const Material& material, const SpotLight& spotLight){
	camera; 
	material; 
	spotLight;
}
