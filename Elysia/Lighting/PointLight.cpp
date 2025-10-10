#include "PointLight.h"

void PointLight::Initialize(){
	//Resource作成
	resource = Elysia::DirectXSetup::GetInstance()->CreateBufferResource(sizeof(PointLightData)).Get();

	//初期値
	//色
	color= { .x = 1.0f,.y = 1.0f,.z = 1.0f,.w = 1.0f };
	//位置
	position= {.x = 0.0f,.y = 0.0f,.z = 0.0f };
	//輝度
	intensity = 4.0f;
	//ライトに届く最大距離
	radius = 5.0f;
	//減衰率
	decay = 5.0f;

}

void PointLight::Update(){
	//書き込み
	resource->Map(0u, nullptr, reinterpret_cast<void**>(&pointLightdata));
	//色
	pointLightdata->color = color;
	//座標
	pointLightdata->position= position;
	//輝度
	pointLightdata->intensity = intensity;
	//ライトに届く最大距離
	pointLightdata->radius = radius;
	//減衰率
	pointLightdata->decay = decay;
	//書き込み終了
	resource->Unmap(0u, nullptr);
}
