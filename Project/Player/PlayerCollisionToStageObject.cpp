#include "PlayerCollisionToStageObject.h"
#include "CollisionConfig.h"
#include <imgui.h>

void PlayerCollisionToStageObject::Initialize(){
	
	//名前の設定
	name_ = "ToStageObject";

	//種類
	collisionType_ = ColliderType::AABBType;

	//自分
	SetCollisionAttribute(COLLISION_ATTRIBUTE_PLAYER4);
	//相手
	SetCollisionMask(COLLISION_ATTRIBUTE_STAGE_OBJECT);

}

void PlayerCollisionToStageObject::Update(){

	const float SIZE = 0.5f;

	//AABBに値を入れていく
	aabb_.max = {
		.x = playerPosition_.x + SIZE,
		.y = playerPosition_.y + SIZE,
		.z = playerPosition_.z + SIZE,
	};

	aabb_.min = {
		.x = playerPosition_.x - SIZE,
		.y = playerPosition_.y - SIZE,
		.z = playerPosition_.z - SIZE,
	};

	


#ifdef _DEBUG
	ImGui::Begin("プレイヤーのコリジョン(ステージオブジェクト用)");
	ImGui::Checkbox("接触", &isTouch_);
	ImGui::InputFloat3("位置", &playerPosition_.x);
	ImGui::InputFloat3("MAX", &aabb_.max.x);
	ImGui::InputFloat3("MIN", &aabb_.min.x);
	ImGui::End();
#endif // _DEBUG


}

void PlayerCollisionToStageObject::Draw(const Camera& camera, const Material& material, const SpotLight& spotLight){
	camera;
	material;
	spotLight;
}
