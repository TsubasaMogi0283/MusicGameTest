#pragma once
/**
 * @file FlashLight.h
 * @brief 懐中電灯クラス
 * @author 茂木翼
 */

#include <memory>
#include <list>

#include "SpotLight.h"
#include "Sprite.h"
#include "Particle3D.h"
#include "Fan.h"
#include "WorldTransform.h"
#include "Model.h"
#include "Material.h"
#include "FlashLightCollision.h"


 /// <summary>
 /// ElysiaEngine(前方宣言)
 /// </summary>
namespace Elysia {
	/// <summary>
	/// グローバル変数
	/// </summary>
	class GlobalVariables;

	/// <summary>
	/// テクスチャ管理クラス
	/// </summary>
	class TextureManager;
}

/// <summary>
/// カメラ
/// </summary>
struct Camera;

/// <summary>
/// チャージ段階
/// </summary>
enum ChargeCondition {
	//不十分
	NoEnoughAttack,
	//通常
	NormalChargeAttack,
	//強い
	StrongChargeAttack,
};

/// <summary>
/// 懐中電灯
/// </summary>
class FlashLight {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	FlashLight();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画(デバッグ用)
	/// </summary>
	/// <param name="camera">カメラ</param>
	void DrawObject3D(const Camera& camera);

	/// <summary>
	/// スプライトの描画
	/// </summary>
	void DrawSprite();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~FlashLight() = default;

private:
	/// <summary>
	/// パーティクル生成
	/// </summary>
	void GenerateParticle();

	/// <summary>
	/// チャージ
	/// </summary>
	void Charge();

	/// <summary>
	/// ImGuiの表示
	/// </summary>
	void ImGuiDisplay();

	/// <summary>
	/// 調整
	/// </summary>
	void Adjustment();

	/// <summary>
	/// デバッグ用処理
	/// </summary>
	void DebugProcess();

public:
	/// <summary>
	/// スポットライトの取得
	/// </summary>
	/// <returns>スポットライト</returns>
	inline SpotLight GetSpotLight() const {
		return spotLight_;
	}


	/// <summary>
	/// プレイヤーの座標を取得
	/// </summary>
	/// <param name="position">座標</param>
	inline void SetPlayerPosition(const Vector3& position) {
		this->playerPosition_ = position;
	}

	/// <summary>
	/// θの設定
	/// </summary>
	/// <param name="theta_">θ</param>
	inline void SetTheta(const float_t& theta) {
		this->theta_ = theta;
	}

	/// <summary>
	/// φの設定
	/// </summary>
	/// <param name="phi">φ</param>
	inline void SetPhi(const float_t& phi) {
		this->phi_ = phi;
	}

	/// <summary>
	/// チャージをしているかどうかの設定
	/// </summary>
	/// <param name="isCharge">チャージ中か</param>
	inline void SetIsCharge(const bool& isCharge) {
		this->isCharge_ = isCharge;
	}

	/// <summary>
	/// クールタイムの設定
	/// </summary>
	/// <param name="isCoolTime"></param>
	inline void SetIsCoolTime(const bool& isCoolTime) {
		this->isCoolTime_= isCoolTime;
	}

	/// <summary>
	/// 攻撃可能かどうかを取得
	/// </summary>
	/// <returns>攻撃可能フラグ</returns>
	inline bool GetIsAbleToAttack() const {
		return isAbleToAttack_;
	}

	/// <summary>
	/// チャージ状態
	/// </summary>
	/// <returns></returns>
	inline uint32_t GetChargeCondition()const {
		return chargeConditionValue_;
	}

	/// <summary>
	/// 扇の取得(3次元)
	/// </summary>
	/// <returns>扇</returns>
	inline Fan3D GetFan3D() {
		return fan3D_;
	}

	/// <summary>
	/// コリジョンの取得
	/// </summary>
	/// <returns>コリジョン</returns>
	inline FlashLightCollision* GetFanCollision()const {
		return flashLightCollision_.get();
	}

private:
	//グローバル変数クラス
	Elysia::GlobalVariables* globalVariables_ = nullptr;
	//テクスチャ管理クラス
	Elysia::TextureManager* textureManager_ = nullptr;

private:
	//調整項目
	//cosFallowoffStart
	const std::string FLASH_LIGHT_COS_FALLOWOFF_START_STRING_ = "FlashLightCosFallowOffStart";
	//最大
	const std::string MAX_STRING_ = "Max";
	float_t maxStart_ = 2.4f;
	//最小
	const std::string MIN_STRING_ = "Min";
	float_t minStart_ = 1.5f;

	//チャージ
	std::string FLASH_LIGHT_CHARGE_VALUE_ = "FlashLightCharge";
	//増える値
	const std::string CHARGE_INCREASE_STRING_ = "IncreaseChargeValue";
	float_t chargeIncreaseValue_ = 0.1f;
	//減る値
	const std::string CHARGE_DECREASE_STRING_ = "DecreaseChargeValue";
	float_t chargeDecreaseValue_ = 0.2f;

	//攻撃可能になる値
	const std::string IS_ABLE_TO_CHARGE_VALUE_STRING = "IsAbleToChargeValue";
	float_t isAbleToAttackValue_ = 5.0f;
	//ゲージスプライトの座標
	const std::string CHARGE_GAUGE_SPRITE_POSITION_STRING_ = "GaugePosition";
	Vector2 chargeGaugeSpritePosition_ = {};

	//パーティクルの放出時間
	const std::string CHARGE_PARTICLE_RELEASE_TIME_ = "PartcleReleaseTime";
	float_t releaseTime_ = 0.0f;

private:
	//チャージスピード
	const float_t CHARGE_INTERVAL_VALUE_ = 0.1f;
	//光の届く距離
	const float_t LIGHT_DISTANCE = 22.0f;
	//最大チャージの値
	const float_t MAX_CHARGE_VALUE_ = 10.0f;
	//最小チャージの値
	const float_t MIN_CHARGE_VALUE_ = 0.0f;
	//時間増加
	const float_t DELTA_ANIMATION_TIME_ = 1.0f / 60.0f;
	//初期の強さ
	const float_t INITIAL_INTENCITY_ = 100.0f;
	//まぶしすぎたので弱める
	const float_t ATTACL_LIGHT_OFFSET_ = 0.5f;
	//攻撃時の強さ
	const float_t ATTACK_LIGHT_INTENCITY_ = 800.0f;
private:
	//スポットライト
	SpotLight spotLight_ = {};
	//プレイヤーの座標
	Vector3 playerPosition_ = {};
	//座標
	Vector3 position_ = {};
	//方向
	Vector3 direction_ = {};
	//幅
	float_t lightSideTheta_ = 0.0f;

	//ライトの幅
	float_t ratio_ = 0.0f;
	//角度
	float_t theta_ = 0.0f;
	float_t phi_ = 0.0f;
	//扇
	Fan3D fan3D_ = {};

	//チャージ時のパーティクル
	std::list<std::unique_ptr<Elysia::Particle3D>>chargeParticle_ = {};
	//パーティクルマテリアル
	Material particleMaterial = {};
	//生成時間
	bool isGenerate_ = false;
	float_t readyForGenerateParticleTime_ = 0.0f;
	
	//チャージのゲージ
	std::unique_ptr<Elysia::Sprite>chargeGaugeSprite_ = nullptr;
	//通常のチャージの色
	Vector4 chargeColor_ = {};

	//フレーム
	std::unique_ptr<Elysia::Sprite>frameSprite_ = nullptr;
	//チャージしているかどうか
	bool isCharge_ = false;
	//チャージの値
	float_t chargeValue_ = 0.0f;
	//チャージ段階
	uint32_t chargeConditionValue_ = 0u;

	//攻撃できるかどうか
	bool isAbleToAttack_ = false;
	//クールタイム
	bool isCoolTime_ = false;

	//攻撃のホワイトフェード
	std::unique_ptr<Elysia::Sprite>attackWhiteFadeSprite_ = nullptr;

private:
	//当たり判定
	std::unique_ptr<FlashLightCollision> flashLightCollision_ = nullptr;

private:

#ifdef _DEBUG

	//デバッグ用
	//左右
	const uint32_t RIGHT_ = 0u;
	const uint32_t LEFT_ = 1u;

	static const uint32_t SIDE_QUANTITY_ = 2u;
	std::unique_ptr<Elysia::Model>sideModel_[SIDE_QUANTITY_] = { nullptr };
	WorldTransform worldTransform_[SIDE_QUANTITY_] = {};
	Material material_ = {};

	//中心
	std::unique_ptr<Elysia::Model>lightCenterModel_ = nullptr;
	WorldTransform lightCenterWorldTransform_ = {};
	Material lightCenterMaterial_ = {};

#endif // _DEBUG


};

