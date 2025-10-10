#pragma once
#include <string>
#include "Vector3.h"
#include "Transform.h"
#include <Polygon/3D/Particle3DManager/Particle3DManager.h>

class ParticleEmitter{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ParticleEmitter() {};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	


	/// <summary>
	/// 更新
	/// </summary>
	void Update();


	/// <summary>
	/// 発生
	/// </summary>
	void Emit();



	/// <summary>
	/// デストラクタ
	/// </summary>
	~ParticleEmitter() {};


private:
	//エミッタの設定
	//Emitter newEmitter_ = {};
	const float DELTA_TIME = 1.0f / 60.0f;

	Vector3 transform_ = {};
	std::string name_ = {};
	uint32_t count_ = 10;

};

