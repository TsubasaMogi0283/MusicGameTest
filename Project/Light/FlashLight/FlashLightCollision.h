#pragma once
/**
 * @file FlashLightCollision.h
 * @brief 懐中電灯の当たり判定クラス
 * @author 茂木翼
 */

#include "Collider.h"


/// <summary>
/// 懐中電灯の当たり判定
/// </summary>
class FlashLightCollision : public Collider{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	FlashLightCollision() = default;

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
	~FlashLightCollision() = default;

public:
	/// <summary>
	/// 3D扇の設定
	/// </summary>
	/// <param name="fan3D"></param>
	inline void SetFan3D(Fan3D& fan3D) {
		this->fan3D_ = fan3D;
	}

	/// <summary>
	/// ワールド座標を設定
	/// </summary>
	/// <param name="position"></param>
	void SetWorldPosition(const Vector3& position) {
		this->playerWorldPosition_ = position;
	}


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


private:
	Vector3 playerWorldPosition_ = {};

};