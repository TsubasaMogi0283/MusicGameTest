#pragma once

/**
 * @file BaseObjectForLevelEditorCollider.h
 * @brief レベルエディタ用のオブジェクトの当たり判定クラス
 * @author 茂木翼
 */

#include "Collider.h"


/// <summary>
/// レベルエディタ用のオブジェクトの当たり判定
/// </summary>
class BaseObjectForLevelEditorCollider:public Collider {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize()=0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// オブジェクトの座標を取得
	/// </summary>
	/// <param name="position"></param>
	virtual void SetObjectPosition(const Vector3& position) {
		this->objectPosition_ = position;
	};

	/// <summary>
	/// 衝突したかどうかのフラグを取得
	/// </summary>
	/// <returns></returns>
	virtual bool GetIsTouch() const{
		return isTouch_;
	}

	/// <summary>
	/// 中心座標
	/// </summary>
	/// <param name="size"></param>
	virtual void SetCenterPosition(const Vector3& centerPosition) {
		this->centerPosition_ = centerPosition;
	}

	/// <summary>
	/// サイズの設定
	/// </summary>
	/// <param name="size"></param>
	virtual void SetSize(const Vector3& size) {
		this->size_ = size;
	}



	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseObjectForLevelEditorCollider() = default;


protected:
	//オブジェクトの座標
	Vector3 objectPosition_ = {};

	//中心座標
	Vector3 centerPosition_ = {};

	//サイズ
	Vector3 size_ = {};

	//衝突したかどうか
	bool isTouch_ = false;



};