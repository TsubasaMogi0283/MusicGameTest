#pragma once

/**
 * @file PlayerCollisionToStrongEnemy.h
 * @brief 強敵に対してのコリジョンクラス
 * @author 茂木翼
 */

#include <memory>

#include "Model.h"
#include "WorldTransform.h"
#include "Material.h"
#include "BasePlayerCollision.h"

/// <summary>
/// カメラ
/// </summary>
struct Camera;

/// <summary>
/// マテリアル
/// </summary>
struct Material;

/// <summary>
/// スポットライト
/// </summary>
struct SpotLight;


/// <summary>
/// 強敵に対してのコライダー
/// </summary>
class PlayerCollisionToStrongEnemy :public BasePlayerCollision {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerCollisionToStrongEnemy() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelHandle"></param>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="material"></param>
	/// <param name="spotLight"></param>
	void Draw(const Camera& camera,const Material & material,const SpotLight& spotLight)override;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerCollisionToStrongEnemy() = default;


public:
	
	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns></returns>
	inline Vector3 GetWorldPosition()override {
		return worldTransform_.GetWorldPosition();
	};


private:
	//モデル
	std::unique_ptr<Elysia::Model> model_ = nullptr;
	//ワールドトランスフォーム
	WorldTransform worldTransform_ = {};
	

};

