#pragma once

/**
 * @file Emitter.h
 * @brief エミッターの構造体
 * @author 茂木翼
 */

#include "Transform.h"

/// <summary>
/// エミッター
/// </summary>
struct Emitter {
public:
	/// <summary>
	/// 更新
	/// </summary>
	void Update();


private:
	//時間変化
	const float_t DELTA_TIME = 1.0f / 60.0f;

public:
	//エミッタのトランスフォーム;
	Transform transform = {};
	//発生数
	uint32_t count = 20u;
	//発生頻度
	float_t frequency = 0.5f;
	//頻度用時刻
	float_t frequencyTime = 0.5f;


	//一度だけ放出するかどうか
	bool isReleaseOnceMode_ = true;
	//生成を止めるかどうか
	bool isStopGenerate_ = false;


	//最初に放出する
	bool isFirstRelease_ = false;
	
};