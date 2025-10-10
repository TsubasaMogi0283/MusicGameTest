#include "SpotLight.h"
#include <numbers>

void SpotLight::Initialize(){
	//Resource作成
	resource = Elysia::DirectXSetup::GetInstance()->CreateBufferResource(sizeof(SpotLightData)).Get();

	//初期値
	//色
	color = { .x = 1.0f,.y = 1.0f,.z = 1.0f,.w = 1.0f };
	//座標
	position = {.x= 2.0f,.y= 1.25f,.z= 0.0f };
	//輝度
	intensity = 4.0f;
	//方向
	direction={.x= -1.0f,.y= 1.0f,.z= 0.0f };
	//届く最大距離
	distance = 7.0f;
	//減衰率
	decay = 2.0f;
	//Fallowoffを制御する
	cosFallowoffStart = 0.3f;
	//余弦
	cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	//ライトに当たっていないところの明るさ
	aroundOffset = 0.0f;

}

void SpotLight::Update(){
	//書き込み
	resource->Map(0u, nullptr, reinterpret_cast<void**>(&data_));
	//色
	data_->color = color;
	//座標
	data_->position = position;
	//輝度
	data_->intensity = intensity;
	//方向
	data_->direction = direction;
	//届く最大距離
	data_->distance = distance;
	//減衰率
	data_->decay = decay;
	//Fallowoffを制御する
	data_->cosFallowoffStart = cosFallowoffStart;
	//余弦
	data_->cosAngle = cosAngle;
	//ライトに当たっていないところの明るさ
	data_->aroundOffset = aroundOffset;
	//書き込み終了
	resource->Unmap(0u, nullptr);

}
