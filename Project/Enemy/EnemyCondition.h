#pragma once

/**
 * @file EnemyCondition.h
 * @brief 敵の状態の列挙型
 * @author 茂木翼
 */

/// <summary>
/// 敵の状態
/// </summary>
enum EnemyCondition {
	//何もしない
	NoneMove,
	//通常
	Move,
	//追いかけ始めるときの座標
	PreTracking,
	//追いかける
	Tracking,
	//攻撃
	Attack,

	//速度反転
	InverseSpeed,


};