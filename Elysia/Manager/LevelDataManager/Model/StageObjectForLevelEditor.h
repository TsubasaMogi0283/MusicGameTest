#pragma once

/**
 * @file StageObjectForLevelEditor.h
 * @brief レベルエディタ用のステージオブジェクトクラス
 * @author 茂木翼
 */

#include "BaseObjectForLevelEditor.h"
#include "StageObjectForLevelEditorCollider.h"

/// <summary>
/// レベルエディタ用のステージオブジェクト
/// </summary>
class StageObjectForLevelEditor :public BaseObjectForLevelEditor {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	StageObjectForLevelEditor() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelhandle"></param>
	/// <param name="transform"></param>
	void Initialize(const uint32_t& modelhandle, const Transform& transform) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

public:

	/// <summary>
	/// AABB用のサイズを設定。コライダーのサイズから取得してね。
	/// </summary>
	/// <param name="size"></param>
	inline void SetSize(const Vector3& size) {
		this->size_ = size;
	}

private:
	//AABB用のサイズ
	Vector3 size_ = {};


};

