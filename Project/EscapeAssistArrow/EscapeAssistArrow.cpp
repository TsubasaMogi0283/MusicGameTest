#include "EscapeAssistArrow.h"

#include <ModelManager.h>
#include <VectorCalculation.h>
#include <Easing.h>
#include <Player/Player.h>

void EscapeAssistArrow::Initialize(const uint32_t& modelHandle, const Vector3& gateCenterPosition){
	//プレイヤーが空だった場合停止
	assert(player_);

	//生成
	model_.reset(Elysia::Model::Create(modelHandle));
	worldTransform_.Initialize();
	worldTransform_.scale = { .x = SCALE_ ,.y = SCALE_ ,.z = SCALE_ };
	worldTransform_.translate.y = HEIGHT_;
	material_.Initialize();
	material_.lightingKinds = LightingType::SpotLighting;

	//門の中心座標を入れる
	this->gateCenterPosition_ = gateCenterPosition;

}

void EscapeAssistArrow::Update() {
	//不透明にしていくとき
	if (isOpaque_ == true) {
		//透明度
		transparency_ += INCREASE_TRANSPARENCY_VALUE_;
		//1.0以上にはしない
		if (transparency_ > 1.0f) {
			transparency_ = 1.0f;
		}
	}
	
	material_.color.w = transparency_;

	//表示
	//線形補間
	indicateT_ += INCREASE_T_VALUE_;
	if (indicateT_ > 1.0f) {
		indicateT_ = 0.0f;
	}
	//差分
	difference_ = VectorCalculation::Subtract(gateCenterPosition_, worldTransform_.GetWorldPosition());
	//角度を求める
	arrowTheta_ = std::atan2f(difference_.x, difference_.z);
	//動く量
	indicateValueZ_ = Easing::EaseInBack(indicateT_);
	//矢印の向きを変える
	worldTransform_.rotate.y = arrowTheta_;
	//指し示す方向。XとZに注意
	Vector3 indicatedirection = {
		.x = std::sinf(arrowTheta_),
		.y = 0.0f,
		.z = std::cosf(arrowTheta_),
	};

	//大元の座標
	Vector3 basePosition = VectorCalculation::Add(player_->GetWorldPosition(), VectorCalculation::Multiply(player_->GetLightDirection(), PLAYER_TO_ARROW_DISTANCE_));
	//指し示した後の座標
	Vector3 indicatedPosition = VectorCalculation::Add(basePosition, indicatedirection);

	//矢印の座標を計算
	worldTransform_.translate = VectorCalculation::Lerp(basePosition, indicatedPosition, indicateValueZ_);
	//Yは固定
	worldTransform_.translate.y = HEIGHT_;


	//更新
	worldTransform_.Update();
	material_.Update();
}


void EscapeAssistArrow::Draw(const Camera& camera, const SpotLight& spotLight) {
	//描画
	model_->Draw(worldTransform_, camera, material_, spotLight);
}