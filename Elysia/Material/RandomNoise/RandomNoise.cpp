#include "RandomNoise.h"

#include "DirectXSetup.h"

void RandomNoise::Initialize(){
	//リソースの生成
	resource = Elysia::DirectXSetup::GetInstance()->CreateBufferResource(sizeof(RandomNoiseData));

	//乱数生成の初期化
	std::random_device rand;
	std::mt19937 newRandomEngine(rand());
	randomEngine = newRandomEngine;
	std::uniform_real_distribution<float_t> distribute(0.0f, 1.0f);
	//値の生成
	value = distribute(randomEngine);
	//テクスチャを使うかどうか
	isUseTexture = false;
}

void RandomNoise::Update(){
	//書き込み
	resource->Map(0u, nullptr, reinterpret_cast<void**>(&randomNoiseData));
	std::uniform_real_distribution<float_t> distribute(0.0f, 1.0f);
	value = distribute(randomEngine);
	randomNoiseData->value = value;
	randomNoiseData->strength = strength;
	randomNoiseData->isUseTexture = isUseTexture;
	resource->Unmap(0u, nullptr);
}
