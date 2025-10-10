#include "Dissolve.h"

void Dissolve::Initialize(){
	//初期化
	//リソースの生成
	resource = Elysia::DirectXSetup::GetInstance()->CreateBufferResource(sizeof(DissolveData));
	//エッジを使うかどうか
	isUseEdge = true;
	//厚さ
	edgeThinkness = 0.04f;
	//エッジの色
	edgeColor = { .x = 1.0f,.y = 1.0f,.z = 1.0f };
	//閾値
	threshold = 0.5f;
}

void Dissolve::Update(){
	//書き込み
	resource->Map(0u, nullptr, reinterpret_cast<void**>(&dissolveData));
	//エッジを使うかどうか
	dissolveData->isUseEdge = isUseEdge;
	//エッジの厚さ
	dissolveData->edgeThinkness = edgeThinkness;
	//エッジの色
	dissolveData->edgeColor = edgeColor;
	//閾値
	dissolveData->threshold = threshold;
	//書き込み終了
	resource->Unmap(0u, nullptr);
}
