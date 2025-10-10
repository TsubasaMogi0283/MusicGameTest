#pragma once

/**
 * @file AudioObjectForLevelEditorCollider.h
 * @brief オーディオオブジェクト用の当たり判定クラス
 * @author 茂木翼
 */


#include "IObjectForLevelEditorCollider.h"

/// <summary>
/// オーディオオブジェクト用の当たり判定
/// </summary>
class AudioObjectForLevelEditorCollider :public BaseObjectForLevelEditorCollider {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	AudioObjectForLevelEditorCollider() = default;

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
	/// 非衝突
	/// </summary>
	void OffCollision()override;


	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition()override;

private:

	/// <summary>
	/// 平面
	/// </summary>
	void Plane();


};

