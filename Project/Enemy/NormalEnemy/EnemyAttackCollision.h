#pragma once
/**
 * @file EnemyAttackCollision.h
 * @brief 敵の攻撃用のコライダークラス
 * @author 茂木翼
 */

#include <memory>

#include "Model.h"
#include "WorldTransform.h"
#include "Material.h"
#include "Collider.h"

#pragma region 前方宣言

/// <summary>
/// カメラ
/// </summary>
struct Camera;

/// <summary>
/// スポットライト
/// </summary>
struct SpotLight;

#pragma endregion


/// <summary>
/// 敵の攻撃用の当たり判定
/// </summary>
class EnemyAttackCollision :public Collider {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyAttackCollision() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelHandle">モデルハンドル</param>
	void Initialize(const uint32_t& modelHandle);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="spotLight"></param>
	void Draw(const Camera& camera,const SpotLight& spotLight);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyAttackCollision() = default;


public:
	/// <summary>
	/// 敵のワールド座標の設定
	/// </summary>
	/// <param name="position"></param>
	inline void SetEnemyPosition(const Vector3& position) {
		this->enemyWorldPosition_ = position;
	}

	/// <summary>
	/// 敵の方向を設定
	/// </summary>
	/// <param name="direction"></param>
	inline void SetEnemyDirection(const Vector3& direction) {
		this->enemyDirection_ = direction;
	}

	/// <summary>
	/// 衝突の接触
	/// </summary>
	/// <returns></returns>
	inline bool GetIsTouch()const {
		return isTouch_;
	}

	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition()override;

	/// <summary>
	/// 衝突
	/// </summary>
	void OnCollision()override;

	/// <summary>
	/// 非接触
	/// </summary>
	void OffCollision()override;


	inline void SetColor(const Vector4& color) {
		material_.color = color;
	}


private:
	//モデル
	std::unique_ptr<Elysia::Model> model_ = nullptr;
	//ワールドトランスフォーム
	WorldTransform worldTransform_ = {};
	//マテリアル
	Material material_ = {};

	//敵の座標と向き
	Vector3 enemyWorldPosition_ = {};
	Vector3 enemyDirection_ = {};

	//衝突しているかどうか
	bool isTouch_ = false;

	//間隔
	const float INTERVAL = 4.0f;
};

