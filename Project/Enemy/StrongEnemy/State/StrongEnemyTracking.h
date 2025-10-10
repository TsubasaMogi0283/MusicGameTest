#pragma once

/**
 * @file StrongEnemyNoneMove.h
 * @brief 強敵が追跡するクラス
 * @author 茂木翼
 */

#include "BaseStongEnemyState.h"


/// <summary>
/// 強敵が追跡する
/// </summary>
class StrongEnemyTracking : public BaseStongEnemyState {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	StrongEnemyTracking();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(StrongEnemy* strongEnemy)override;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~StrongEnemyTracking()=default;
};

