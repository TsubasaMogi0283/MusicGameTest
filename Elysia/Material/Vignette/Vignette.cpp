#include "Vignette.h"

void Vignette::Initialize(){
	//リソースの生成
	resource = Elysia::DirectXSetup::GetInstance()->CreateBufferResource(sizeof(VignetteData));

	//初期値
	scale = 16.0f;
	pow = 0.8f;
	color = { .x = 0.0f,.y = 0.0f,.z = 1.0f };
}

void Vignette::Update(){
	//書き込み
	resource->Map(0u, nullptr, reinterpret_cast<void**>(&vignetteData));
	//倍
	vignetteData->scale = scale;
	//乗
	vignetteData->pow = pow;
	//色
	vignetteData->color = color;
	//書き込み終了
	resource->Unmap(0u, nullptr);
}
