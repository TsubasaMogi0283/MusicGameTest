#include "ParticleEmitter.h"


void ParticleEmitter::Initialize(){
	transform_ = { 0.0f,0.0f,0.0f };
	name_ = {};
	count_ = 10;


	//newEmitter_.count = 100;
	////0.5秒ごとに発生
	//newEmitter_.frequency = 0.5f;
	////発生頻度用の時刻。0.0で初期化
	//newEmitter_.frequencyTime = 0.0f;
	////SRT
	//newEmitter_.transform.scale = { 1.0f,1.0f,1.0f };
	//newEmitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	//newEmitter_.transform.translate = { 0.0f,0.0f,0.0f };
}

void ParticleEmitter::Update(){
	///時間経過
	//newEmitter_.frequencyTime += DELTA_TIME;
	//頻度より大きいなら
	//if (newEmitter_.frequency <= newEmitter_.frequencyTime) {
		
		//ManagerのEmitを呼び出すだけ
		//Particle3DManager::GetInstance()->Emit(name_, transform_, count_);

		//余計に過ぎた時間も加味して頻度計算する
		//newEmitter_.frequencyTime -= newEmitter_.frequency;

	//}
}

void ParticleEmitter::Emit(){
	
}
