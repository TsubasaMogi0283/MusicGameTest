#pragma once

/**
 * @file ParticleMoveType.h
 * @brief  動き方の設定の構造体
 * @author 茂木翼
 */

/// <summary>
/// 動き方の設定
/// </summary>
enum ParticleMoveType {

	//通常の放出
	NormalRelease,
	//鉛直投げ上げ
	ThrowUp,
	//自由落下
	FreeFall,
	//上昇
	Rise,
	//吸収
	Absorb,
	//留まる
	Stay

};