#include "StageObjectForLevelEditorCollider.h"
#include "CollisionConfig.h"
#include <imgui.h>

void StageObjectForLevelEditorCollider::Initialize() {
	//基本AABB
	//必要があれば追加していく
	collisionType_ = ColliderType::AABBType;

	
	//自分
	SetCollisionAttribute(COLLISION_ATTRIBUTE_STAGE_OBJECT);
	//相手
	SetCollisionMask(COLLISION_ATTRIBUTE_PLAYER4);

}

void StageObjectForLevelEditorCollider::Update(){
	


	aabb_.max = {
		.x = objectPosition_.x + size_.x/2.0f,
		.y = objectPosition_.y + size_.y/2.0f,
		.z = objectPosition_.z + size_.z/2.0f,
	};
	aabb_.min = {
		.x = objectPosition_.x - size_.x/2.0f,
		.y = objectPosition_.y - size_.y/2.0f,
		.z = objectPosition_.z - size_.z/2.0f,
	};

#ifdef _DEBUG
	ImGui::Begin("ステージオブジェクト(コライダー)");
	ImGui::InputFloat3("MAX", &aabb_.max.x);
	ImGui::InputFloat3("MIN", &aabb_.min.x);
	ImGui::InputFloat3("位置", &objectPosition_.x);
	ImGui::Checkbox("衝突", &isTouch_);
	ImGui::End();
#endif // _DEBUG

}

void StageObjectForLevelEditorCollider::OnCollision(){
	isTouch_ = true;
}

void StageObjectForLevelEditorCollider::OffCollision(){
	isTouch_ = false;
}

Vector3 StageObjectForLevelEditorCollider::GetWorldPosition() {
	return objectPosition_;
}
