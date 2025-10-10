#pragma once

/**
 * @file Fan.h
 * @brief 扇の構造体
 * @author 茂木翼
 */



#include "Vector2.h"
#include "Vector3.h"



/// <summary>
/// 扇型(2D)
/// </summary>
struct Fan2D {
	//中心座標
	Vector2 position;
	//長さ
	float_t length;

	//片側の角度
	float_t sideAngle;

	//2等分するベクトル
	Vector2 direction;
	Vector2 rightVector;
	Vector2 leftVector;


	//左側の角度
	float_t leftSideRadian;
	//中心の角度
	float_t centerRadian;
	//右側の角度
	float_t rightSideRadian;


};

/// <summary>
/// 扇形(3D)
/// </summary>
struct Fan3D {
	//中心座標
	Vector3 position;
	//長さ
	float_t length;

	//片側幅の角度
	float_t sideThetaAngle;
	float_t sidePhiAngleSize;

	//2等分するベクトル
	Vector3 direction;
	Vector3 rightVector;
	Vector3 leftVector;


	//中心の角度
	float_t centerRadian;
	float_t centerPhi;


};