#pragma once
/**
 * @file KeyFrame.h
 * @brief キーフレームの構造体
 * @author 茂木翼
 */

#include <Vector3.h>
#include <Quaternion.h>

template <typename tValue>

/// <summary>
/// キーフレーム
/// </summary>
struct KeyFrame {
	//キーフレームの値
	tValue value;
	//キーフレームの時刻(秒)
	float time;
};



//テンプレートを作っておくと拡張が楽になるよ！
//Vector3版
using KeyFrameVector3 = KeyFrame<Vector3>;
//Quaternion
using KeyFrameQuaternion = KeyFrame<Quaternion>;

