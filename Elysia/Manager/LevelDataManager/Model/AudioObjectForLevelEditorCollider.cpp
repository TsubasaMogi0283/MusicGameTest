#include "AudioObjectForLevelEditorCollider.h"
#include "CollisionConfig.h"
#include <imgui.h>

void AudioObjectForLevelEditorCollider::Initialize(){
	//必要になったら他の形も追加できるようにする
	//今の所これだけで充分
	collisionType_ = ColliderType::PlaneType;

	//自分
	SetCollisionAttribute(COLLISION_ATTRIBUTE_AUDIO_OBJECT);
	//相手
	SetCollisionMask(COLLISION_ATTRIBUTE_PLAYER3);

}


void AudioObjectForLevelEditorCollider::Plane(){
	plane_ = {
		.position = objectPosition_,
		.length = size_.z,
		.width = size_.x,
	};


#ifdef _DEBUG
	ImGui::Begin("オーディオオブジェクトのコライダー");
	ImGui::InputFloat3("位置", &plane_.position.x);
	ImGui::InputFloat("縦", &plane_.length);
	ImGui::InputFloat("横", &plane_.width);
	ImGui::Checkbox("isTouch", &isTouch_);
	ImGui::End();
#endif // _DEBUG


}

void AudioObjectForLevelEditorCollider::Update(){
	
	//平面の場合
	Plane();

}

void AudioObjectForLevelEditorCollider::OnCollision(){
	isTouch_ = true;
}

void AudioObjectForLevelEditorCollider::OffCollision(){
	isTouch_ = false;;
}

Vector3 AudioObjectForLevelEditorCollider::GetWorldPosition(){
	
	return objectPosition_;
}


