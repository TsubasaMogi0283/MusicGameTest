#pragma once
/**
 * @file StageObjectForLevelEditorCollider.h
 * @brief ステージオブジェクト用のクラス
 * @author 茂木翼
 */

#include "IObjectForLevelEditorCollider.h"

/// <summary>
/// ステージオブジェクト用のクラス
/// </summary>
class StageObjectForLevelEditorCollider :public BaseObjectForLevelEditorCollider {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 衝突
	/// </summary>
	void OnCollision()override;


	/// <summary>
	/// 比衝突
	/// </summary>
	void OffCollision()override;


	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition()override;

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <param name="position"></param>
	inline void SetWorldPosition(const Vector3& position) {
		this->objectPosition_ = position;
	}


};

