#include "Key.h"

#include <imgui.h>

#include "ModelManager.h"
#include "VectorCalculation.h"
#include "TextureManager.h"
#include "Easing.h"
#include "SingleCalculation.h"

void Key::Initialize(const uint32_t& modelhandle,const Vector3& position){
	//モデルの生成
	model_.reset(Elysia::Model::Create(modelhandle));

	//スケールのサイズ
	const float_t SCALE = 0.4f;
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	//拡縮
	worldTransform_.scale = {.x = SCALE,.y = SCALE ,.z = SCALE };
	//回転
	worldTransform_.rotate.z = std::numbers::pi_v<float_t> / 2.0f;
	//座標
	worldTransform_.translate = position;
	originalPositionY_ = position.y;
	//半径
	//と言っても少しだけ大きくして取りやすくする
	radius_ = SCALE * 4.0f;
	//取得されたかどうかのフラグ
	isPickUp_ = false;

	//マテリアルの初期化
	material_.Initialize();
	//ライティングの種類
	material_.lightingKinds = SpotLighting;
	//色
	material_.color = {.x = 1.0f,.y = 1.0f,. z = 0.0f,. w = 1.0f };

	//鍵のスプライト
	uint32_t textureHandle = Elysia::TextureManager::GetInstance()->Load("Resources/Sprite/Item/Key.png");
	sprite_.reset(Elysia::Sprite::Create(textureHandle, {.x=0.0f,.y=0.0f}));
	//アンカーポイントを設定する
	const Vector2 ANCHOR_POINT = { .x = 0.5f,.y = 0.5f };
	sprite_->SetAnchorPoint(ANCHOR_POINT);
	//最初は非表示
	sprite_->SetInvisible(true);

}

void Key::Update(){
	//取得
	if (isPickUp_ == false) {
		//角度の計算
		heightTheta_ += ROTATE_AMOUNT_;
		//上下する
		worldTransform_.translate.y = std::sinf(heightTheta_) * MOVE_AMOUNT_ + originalPositionY_;
		//回転
		worldTransform_.rotate.x += ROTATE_AMOUNT_;

	}
	else {
		//上昇回転
		RiseAndRotate();
		//スプライトが動く
		SpriteMove();
	}

	//ワールドトランスフォームの更新
	worldTransform_.Update();
	//マテリアルの更新
	material_.Update();
	
	//ImGui表示用
	DisplayImGui();

}

void Key::DrawModel(const Camera& camera,const SpotLight& spotLight){
	//鍵(モデル)の描画
	model_->Draw(worldTransform_, camera, material_, spotLight);

}

void Key::DrawSprite(){
	//鍵(スプライト)の描画
	sprite_->Draw();
}

void Key::RiseAndRotate(){

	//上昇する前の座標を記録
	if (isWritePreUpPosition_==false) {
		preUpPosition_ = GetWorldPosition();
		isWritePreUpPosition_ = true;
	}

	//イージングを使い急上昇する感じを出す
	upT_ += 0.001f;
	float_t newUpT = Easing::EaseOutSine(upT_);
	
	//線形補間を使い
	float_t lerpedPositionY = SingleCalculation::Lerp(preUpPosition_.y, preUpPosition_.y+HEIGHT_, newUpT);
	worldTransform_.translate.y = lerpedPositionY;

	//高速回転
	const float_t RAPID_ROTATE_AMOUNT = 0.5f;
	worldTransform_.rotate.x += RAPID_ROTATE_AMOUNT;

	//4回転した後に拡縮
	if (worldTransform_.rotate.x > std::numbers::pi_v<float_t>*12.0f) {
		
		const Vector3 SCALE_DOWN = { .x = SCALE_DOWM_VALUE_,.y = SCALE_DOWM_VALUE_ ,.z = SCALE_DOWM_VALUE_ };
		worldTransform_.scale = VectorCalculation::Add(worldTransform_.scale, SCALE_DOWN);
	}

	//消える
	if (worldTransform_.scale.x < 0.0f&&
		worldTransform_.scale.y < 0.0f&&
		worldTransform_.scale.z < 0.0f) {
		worldTransform_.scale.x = 0.0f;
		worldTransform_.scale.y = 0.0f;
		worldTransform_.scale.z = 0.0f;
		//スプライトが動く
		isSpriteMove_ = true;
		
	}



}

void Key::SpriteMove(){
	if (isSpriteMove_ == true) {
		//表示
		sprite_->SetInvisible(false);

		scaleT_ += 0.01f;
		//サイズの設定
		float_t scaleSize = std::sinf(scaleT_ * std::numbers::pi_v<float_t>);
		sprite_->SetScale({ .x = scaleSize ,.y = scaleSize });

		//回転の設定
		spriteRotate_ -= 0.5f;
		sprite_->SetRotate(spriteRotate_);

		
		//座標の設定
		Vector2 position = VectorCalculation::Lerp(SPRITE_STRAT_POSITION_, spriteEndPosition_, scaleT_);
		sprite_->SetPosition(position);

		//消える
		if (scaleT_ >= 1.0f) {
			sprite_->SetInvisible(true);
			isDelete_ = true;
			isSpriteMove_ = false;
		}
	}
}

void Key::DisplayImGui(){
#ifdef _DEBUG
	ImGui::Begin("鍵のスプライト");
	ImGui::InputFloat("scaleT_", &scaleT_);
	ImGui::SliderFloat3("回転", &worldTransform_.rotate.x,0.0f,3.0f);
	ImGui::End();
#endif // _DEBUG

}


