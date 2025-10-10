#pragma once

#include "Model.h"
#include "Material.h"
#include "WorldTransform.h"
#include "Collider.h"


/// <summary>
/// プレイヤーに対してのコリジョン
/// </summary>
class StrongEnemyCollisionToPlayer : public Collider{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	StrongEnemyCollisionToPlayer() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelHandle"></param>
	void Initialize(const uint32_t & modelHandle);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const Camera& camera, const SpotLight& spotLight);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~StrongEnemyCollisionToPlayer() = default;

public:

	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns></returns>
	inline Vector3 GetWorldPosition()override {
		return worldTransform_.GetWorldPosition();
	};

	/// <summary>
	/// 衝突
	/// </summary>
	void OnCollision()override;

	/// <summary>
	/// 非接触
	/// </summary>
	void OffCollision()override;



public:
	/// <summary>
	/// 敵の座標を取得
	/// </summary>
	/// <param name="position"></param>
	inline void SetEnemyPosition(const Vector3& position) {
		this->enemyPosition_ = position;
	}

	/// <summary>
	/// プレイヤーと当たったかどうか
	/// </summary>
	/// <returns></returns>
	inline bool GetIsTouchPlayer()const {
		return isTouchPlayer_;
	}

private:
	//モデル
	std::unique_ptr<Elysia::Model>model_ = nullptr;
	//ワールドトランスフォーム
	WorldTransform worldTransform_ = {};
	//マテリアル
	Material material_ = {};
	
	//座標
	Vector3 enemyPosition_ = {};

	//当たったかどうか
	bool isTouchPlayer_ = false;


};

