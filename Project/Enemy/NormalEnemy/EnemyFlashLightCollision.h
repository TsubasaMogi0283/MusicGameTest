#pragma once
/**
 * @file EnemyFlashLightCollision.h
 * @brief 扇との当たり判定クラス
 * @author 茂木翼
 */


#include "Collider.h"
#include "Vector3.h"

/// <summary>
/// 扇との当たり判定
/// </summary>
class EnemyFlashLightCollision : public Collider {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyFlashLightCollision() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyFlashLightCollision() = default;

public:


	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition()override;

	/// <summary>
	/// 接触
	/// </summary>
	void OnCollision()override;

	/// <summary>
	/// 非接触
	/// </summary>
	void OffCollision()override;


	/// <summary>
	/// 敵の位置を設定
	/// </summary>
	/// <param name="position"></param>
	inline void SetEnemyPosition(const Vector3& position) {
		this->enemyPosition_ = position;
	}

	/// <summary>
	/// 接触しているか
	/// </summary>
	/// <returns></returns>
	inline bool GetIsTouched()const {
		return isTouch_;
	}


private:
	//触れているかどうか
	bool isTouch_ = false;

	//位置
	Vector3 enemyPosition_ = {};


};

