#pragma once

/**
 * @file StrongEnemyNoneMove.h
 * @brief 強敵が動かない状態のクラス
 * @author 茂木翼
 */

#include "BaseStongEnemyState.h"


/// <summary>
/// 強敵が動かない状態
/// </summary>
class StrongEnemyNoneMove : public BaseStongEnemyState {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	StrongEnemyNoneMove();

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
	~StrongEnemyNoneMove()=default;
};

