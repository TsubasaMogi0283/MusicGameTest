#pragma once
#pragma once
/**
 * @file NodeAnimation.h
 * @brief ノードアニメーションの構造体
 * @author 茂木翼
 */

#include <vector>

#include "KeyFrame.h"
#include "Vector3.h"

template <typename tValue>
/// <summary>
/// アニメーションカーブ
/// </summary>
struct AnimationCurve {
	std::vector<KeyFrame<tValue>> keyFrames;
};

/// <summary>
/// ノードアニメーション
/// </summary>
struct NodeAnimation {
	//スケール
	AnimationCurve<Vector3> scale;
	//回転
	AnimationCurve<Quaternion> rotate;
	//座標
	AnimationCurve<Vector3> translate;
	
	
};