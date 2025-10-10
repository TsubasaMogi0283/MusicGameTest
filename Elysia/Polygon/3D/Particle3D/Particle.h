#pragma once

/**
 * @file ParticleInformation.h
 * @brief パーティクル
 * @author 茂木翼
 */


#include "Transform.h"
#include "Vector4.h"
#include "Matrix4x4.h"

/// <summary>
/// パーティクルの情報
/// </summary>
struct ParticleInformation {
	//トランスフォーム
	Transform transform;
	//初期トランスフォーム
	Transform initialTransform;
	//速度
	Vector3  velocity;
	//色
	Vector4 color;
	//生存時間
	float_t lifeTime;
	//現在の時間
	float_t currentTime;
	//消えているかどうか
	bool isInvisible;

	//吸収用
	float absorbT;

};

/// <summary>
/// GPUに送る方のパーティクル
/// </summary>
struct ParticleForGPU {
	//ワールド座標
	Matrix4x4  world;
	//色
	Vector4 color;
};
