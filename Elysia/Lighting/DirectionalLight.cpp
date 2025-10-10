#include "DirectionalLight.h"

void DirectionalLight::Initialize(){
	//リソースの生成
	resource = Elysia::DirectXSetup::GetInstance()->CreateBufferResource(sizeof(DirectionalLightData)).Get();

	//初期値
	//ライトの色
	color = {.x = 1.0f,.y = 1.0f,.z = 1.0f,.w = 1.0f};
	//ライトの向き
	direction= { 0.0f,-1.0f,0.0f };
	//輝度
	intensity= 1.0f;

}

void DirectionalLight::Update(){
	//書き込み
	resource->Map(0u, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	//色
	directionalLightData_->color = color;
	//方向
	directionalLightData_->direction = direction;
	//輝度
	directionalLightData_->intensity = intensity;
	//書き込み終了
	resource->Unmap(0u, nullptr);
}

