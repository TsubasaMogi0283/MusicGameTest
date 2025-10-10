#pragma once

/**
 * @file PoltergeistBone.h
 * @brief 骨のポルターガイストクラス
 * @author 茂木翼
 */

#include <Particle3D.h>
#include <Material.h>

/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {
	/// <summary>
	/// オーディオ
	/// </summary>
	class Audio;

	/// <summary>
	/// レベルデータ管理クラス
	/// </summary>
	class LevelDataManager;

	/// <summary>
	/// モデル管理クラス
	/// </summary>
	class ModelManager;
}

/// <summary>
/// プレイヤー
/// </summary>
class Player;

/// <summary>
/// カメラ
/// </summary>
struct Camera;

/// <summary>
/// スポットライト
/// </summary>
struct SpotLight;

/// <summary>
/// 骨のポルターガイスト
/// </summary>
class PoltergeistBone{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PoltergeistBone();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="spotlight">スポットライト</param>
	void Draw(const Camera& camera, const SpotLight& spotlight);

	/// デストラクタ
	/// </summary>
	~PoltergeistBone() = default;

public:
	/// <summary>
	/// プレイヤー
	/// </summary>
	/// <param name="player"></param>
	inline void SetPlayer(Player* player) {
		this->player_ = player;
	}

	/// <summary>
	/// 動き始める
	/// </summary>
	inline void SetStart() {
		//this->isBoneRise_ = true;
	}

	/// <summary>
	/// レべルデータの設定
	/// </summary>
	/// <param name="levelDataHandle"></param>
	inline void SetLevelDataHandle(const uint32_t& levelDataHandle) {
		this->levelDataHandle_ = levelDataHandle;
	}


private:
	//レベルエディタ
	Elysia::LevelDataManager* levelDataManager_ = nullptr;
	//ハンドル
	uint32_t levelDataHandle_ = 0u;
	//モデル管理クラス
	Elysia::ModelManager* modelManager_ = nullptr;
	//オーディオ
	Elysia::Audio* audio_ = nullptr;
	//骨の名前
	std::string boneString_ = "Bone";
	//プレイヤー
	Player* player_ = nullptr;

private:

	//浮遊の高さ
	const float_t FLOATING_HEIGHT_ = 4.0f;
	//浮遊の増える値
	const float_t FLOATING_AMOUNT_ = 0.05f;
	//浮遊時間
	const float_t FLOATING_TIME_ = 3.0f;
	//落下準備時間
	const float_t READY_FOR_DROP_TIME_ = 2.5f;
	//落下時間
	const float_t DROP_TIME_ = 2.0f;
	//欠片の大きさ
	const float_t BONE_PIECE_SCALE_ = 0.5f;
	//Y方向のベロシティ
	const float_t THROW_UP_VELOCITY_Y_ = 0.3f;
	//時間変化
	const float_t DELTA_TIME_ = 1.0f / 60.0f;
	//回転の値
	const float_t ROTATE_THETA_VALUE_ = 0.1f;
	//高速回転
	const float_t RAPID_ROTATE_THETA_VALUE_ = 0.5f;
	//地面の座標
	const float_t GROUND_POSITION_Y = 0.0f;
	//回転する値の補正
	const float_t ROTATE_VALUE_OFFSET_ = 0.1f;
	//骨のスピード
	const float_t BONE_SPEED_ = 0.01f;
	//警告音のピッチ用
	const float_t DELTA_INCREASE_FREAQUENCY_RATION_VALUE_ = 0.005f;

private:
	/// <summary>
	/// 動きの状態
	/// </summary>
	enum PoltergeistBoneMovement {
		//準備
		BoneRise,
		//浮遊
		BoneFloat,
		//落下準備
		BoneReadyForDrop,
		//落下
		BoneDrop,
		//終了
		BoneProcessEnd,
	};

private:
	//動きの状態
	PoltergeistBoneMovement movementState_ = PoltergeistBoneMovement::BoneRise;

	//骨が上がり切ったかどうか
	bool isFinishRiseBone_ = false;
	//浮遊時間
	float_t floatingBoneTime_ = 0.0f;
	//浮遊時に使うθ
	float_t floatingTheta_ = 0.0f;
	//骨の座標
	Vector3 bonePosition_ = {};
	//落下準備委時間
	float_t readyForDropTime_ = 0.0f;
	//落下スピード
	float_t dropSpeed_ = 0.4f;
	//プレイヤーの座標ロックオン
	Vector3 loclOnPlayerPosition_ = {};
	//ロックオンしたかどうか
	bool isLockOn_ = false;
	//落下用の線形補間
	float_t dropT_ = 0.0f;
	//落下前の骨の座標
	Vector3 beforeBoneDropPosition_ = {};
	//骨とプレイヤーの方向
	Vector3 boneDirectionToPlayer_ = {};

	//骨の欠片のパーティクル
	std::unique_ptr<Elysia::Particle3D> bonePieceParticle_ = nullptr;
	//欠片のモデルハンドル
	uint32_t bonePieceParticleHandle_ = 0u;
	//骨の欠片パーティクル用のマテリアル
	Material bonePieceMaterial_ = {};
	//欠片の数
	uint32_t bonePieceCount_ = 30u;
	//一回だけ当たったかどうか
	bool isTouchOnce_ = false;

	//骨が壊れる音
	uint32_t boneBreakAudioHandle_ = 0u;

	//骨の警告音
	uint32_t warningBoneAudioHandle_ = 0u;
	float_t warningFrequencyRatio_ = 1.0f;
	bool isPlayWarningSE_ = false;

};