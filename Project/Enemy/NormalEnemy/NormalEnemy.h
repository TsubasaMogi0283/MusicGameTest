#pragma once


/**
 * @file NormalEnemy.h
 * @brief 敵のクラス
 * @author 茂木翼
 */

#include <memory>

#include "WorldTransform.h"
#include "AnimationModel.h"
#include "Material.h"
#include "Particle3D.h"
#include "DirectionalLight.h"
#include "SkinCluster.h"

#include "AABB.h"
#include "Enemy/EnemyCondition.h"
#include "EnemyAttackCollision.h"
#include "EnemyFlashLightCollision.h"
#include "Enemy/BaseEnemy.h"
#include "State/BaseNormalEnemyState.h"

#pragma region 前方宣言

 /// <summary>
 /// カメラ
 /// </summary>
struct Camera;

/// <summary>
/// スポットライト
/// </summary>
struct SpotLight;

/// <summary>
/// プレイヤー
/// </summary>
class Player;

/// <summary>
/// EllysiaEngine(前方宣言)
/// </summary>
namespace Elysia {
	/// <summary>
	/// グローバル変数
	/// </summary>
	class GlobalVariables;
}


#pragma endregion

/// <summary>
/// 敵
/// </summary>
class NormalEnemy : public BaseEnemy {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	NormalEnemy();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelHandle">モデルハンドル</param>
	/// <param name="position">座標</param>
	/// <param name="speed">スピード</param>
	void Initialize(const uint32_t& modelHandle, const Vector3& position, const Vector3& speed)override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;


	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="spotLight">スポットライト</param>
	void Draw(const Camera& camera, const SpotLight& spotLight)override;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~NormalEnemy()override;

public:
	/// <summary>
	/// 状態遷移
	/// </summary>
	/// <param name="newState">新しい状態</param>
	void ChengeState(std::unique_ptr<BaseNormalEnemyState> newState);

	/// <summary>
	/// 現在の状態を取得
	/// </summary>
	/// <returns>状態名</returns>
	std::string GetCurrentStateName()const {
		return currentStateName_;
	}

	/// <summary>
	/// 前回の状態を取得
	/// </summary>
	/// <returns>状態名</returns>
	std::string GetPreStateName()const {
		return preStateName_;
	};

	/// <summary>
	/// 感電状態を取得
	/// </summary>
	/// <returns></returns>
	inline bool GetIsElectricShock()const {
		return isElectricShock_;
	}

	/// <summary>
	/// 攻撃しているかどうか
	/// </summary>
	/// <returns>フラグ</returns>
	inline bool GetIsAttack()const {
		return isAttack_;
	}

	/// <summary>
	/// 攻撃したかどうかのフラグを設定
	/// </summary>
	/// <param name="isAttack">攻撃しているかどうか</param>
	inline void SetIsAttack(const bool& isAttack) {
		this->isAttack_ = isAttack;
	}

	/// <summary>
	/// 攻撃アニメーションをしているかどうか
	/// </summary>
	/// <returns></returns>
	inline bool GetIsAttackAnimation()const {
		return isAttackAnimation_;
	}

	/// <summary>
	/// 攻撃アニメーションをしているかどうか
	/// </summary>
	/// <param name="isAttackAnimation"></param>
	inline void SetIsAttackAnimation(const bool& isAttackAnimation) {
		this->isAttackAnimation_ = isAttackAnimation;
	}

	/// <summary>
	/// ライトの強さを設定
	/// </summary>
	/// <param name="strength">強さ</param>
	inline void SetLightStrength(const uint32_t& strength) {
		this->damagedValue_ = strength;
	}

	/// <summary>
	/// ダメージの受け取り
	/// </summary>
	/// <param name="isAccept"></param>
	inline void SetIsAcceptDamaged(const bool& isAccept) {
		this->isAcceptDamage_ = isAccept;
	}

	/// <summary>
	/// アニメーションの時間を設定
	/// </summary>
	/// <param name="animationTime"></param>
	inline void SetAnimationTime(const float_t& animationTime) {
		this->animationTime_ = animationTime;
	}


	/// <summary>
	/// 現在の状態を取得
	/// </summary>
	/// <returns></returns>
	inline BaseNormalEnemyState* GetCurrentState() {
		return currentState_.get();
	}

	/// <summary>
	/// 攻撃用のコリジョンを取得
	/// </summary>
	/// <returns>コリジョン</returns>
	inline EnemyAttackCollision* GetEnemyAttackCollision() const {
		return attackCollision_.get();
	}

	/// <summary>
	/// 懐中電灯用のコリジョンを取得
	/// </summary>
	/// <returns>コリジョン</returns>
	inline EnemyFlashLightCollision* GetEnemyFlashLightCollision() const {
		return enemyFlashLightCollision_.get();
	}

private:
	/// <summary>
	/// 攻撃をライトで受ける
	/// </summary>
	void Damaged();

	/// <summary>
	/// 消える
	/// </summary>
	void Delete();

	/// <summary>
	/// ImGuiの表示
	/// </summary>
	void DisplayImGui();

private:
	//グローバル変数クラス
	Elysia::GlobalVariables* globalVariables_ = nullptr;
	//グループ名
	const std::string GROUNP_NAME_ = "NormalEnemy";

private:

	//幅
	const float_t RADIUS_ = 2.0f;
	//幅(Vector3)
	const Vector3 RADIUS_INTERVAL_ = { .x = RADIUS_,.y = RADIUS_,.z = RADIUS_ };
	//感電時間変化
	const float_t ELECTRIC_SHOCK_DELTA_TIME_ = 1.0f / 60.0f;

	//最初の感電時間変化
	const float_t FIRST_ELECTRIC_SHOCK_DELTA_TIME_ = 1.0f / 120.0f;
	//線形補間の最大値
	const float_t MAX_T_VALUE_ = 1.0f;

private:

	/// <summary>
	/// 体力状態
	/// </summary>
	enum HPCondition {
		Dead,
		Dangerous,
		Normal,
	};

private:
	//アニメーションモデル
	std::unique_ptr<Elysia::AnimationModel>animationmodel_ = nullptr;

	//スケルトン
	Skeleton skeleton_ = {};
	//アニメーション時間
	float animationTime_ = {};
	//クラスター
	SkinCluster skinCluster_ = {};
	//アニメーションハンドル
	uint32_t animationHandle_ = 0u;

	//パーティクル
	std::unique_ptr<Elysia::Particle3D> deadParticle_ = {};
	//感電
	std::unique_ptr<Elysia::Particle3D> electricShockParticle_ = {};
	uint32_t thunderTextureHandle_ = 0u;
	//マテリアル
	Material particleMaterial_ = {};
	//デバッグ用のモデルハンドル
	uint32_t debugModelHandle = 0;

	//感電の値
	Vector3 electricDamageShakeValue_ = {};

	//最初の感電
	bool IsFirstElectricShock_ = false;
	float_t firstElectricShockT_ = 0.0f;

	//2回目以降の感電
	bool isElectricShock_ = false;
	float_t electricShockT_ = 0.0f;

	//振動のオフセット
	float_t shakeOffset_ = 0.05f;
	bool isShake_ = false;

	//受けるダメージ
	uint32_t damagedValue_ = 0u;
	//攻撃を受けるかどうか
	bool isAcceptDamage_ = false;


	//攻撃状態
	bool isAttack_ = false;
	//攻撃アニメーションをしているかどうか
	bool isAttackAnimation_ = false;

	//状態の名前
	//前回
	std::string preStateName_ = "";
	//現在
	std::string currentStateName_ = "";
	
private:
	//攻撃用の当たり判定
	std::unique_ptr<EnemyAttackCollision> attackCollision_ = nullptr;
	//懐中電灯用の当たり判定
	std::unique_ptr<EnemyFlashLightCollision> enemyFlashLightCollision_ = nullptr;
	//状態
	std::unique_ptr<BaseNormalEnemyState> currentState_ = nullptr;
};