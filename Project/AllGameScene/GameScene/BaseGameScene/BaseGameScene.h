#pragma once
/**
 * @file BaseGameScene.h
 * @brief 細かいゲームシーンの基底クラス
 * @author 茂木翼
 */

#include <cstdint>
#include <memory>

#include "Sprite.h"
#include "Vignette.h"

/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {
	/// <summary>
	/// 入力
	/// </summary>
	class Input;

	/// <summary>
	/// レベル管理クラス
	/// </summary>
	class LevelDataManager;

	/// <summary>
	/// ゲーム管理クラス
	/// </summary>
	class GameManager;

	/// <summary>
	/// テクスチャ管理クラス
	/// </summary>
	class TextureManager;

	/// <summary>
	/// オーディオ
	/// </summary>
	class Audio;
}

/// <summary>
/// ゲームシーン(メイン)
/// </summary>
class GameScene;

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
/// 敵管理クラス
/// </summary>
class EnemyManager;

/// <summary>
/// 鍵管理クラス
/// </summary>
class KeyManager;

/// <summary>
/// 細かいゲームシーンの基底クラス
/// </summary>
class BaseGameScene {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="gameScene">ゲームシーン(メイン)</param>
	virtual void Update(GameScene* gameScene) = 0;

	/// <summary>
	/// 3Dモデルの描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="spotLight">スポットライト</param>
	virtual void DrawObject3D(const Camera& camera, const SpotLight& spotLight) = 0;

	/// <summary>
	/// スプライト
	/// </summary>
	virtual void DrawSprite() = 0;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseGameScene() = default;


protected:
	/// <summary>
	/// ImGui表示用
	/// </summary>
	virtual void DisplayImGui() = 0;


public:
	/// <summary>
	/// レベルデータハンドルの設定
	/// </summary>
	/// <param name="levelDataHandle">レベルデータハンドル</param>
	inline void SetLevelDataHandle(const uint32_t& levelDataHandle) {
		this->levelDataHandle_ = levelDataHandle;
	}
	
	/// <summary>
	/// プレイヤーの設定
	/// </summary>
	/// <param name="player">プレイヤー</param>
	inline void SetPlayer(Player* player) {
		this->player_ = player;
	}


	/// <summary>
	/// 敵管理クラスの設定
	/// </summary>
	/// <param name="enemyManager">敵管理クラス</param>
	inline void SetEnemyManager(EnemyManager* enemyManager) {
		this->enemyManager_ = enemyManager;
	}

	/// <summary>
	/// 鍵管理クラス
	/// </summary>
	/// <param name="keyManager">鍵管理クラス</param>
	inline void SetKeyManager(KeyManager* keyManager) {
		this->keyManager_ = keyManager;
	}


	/// <summary>
	/// 強い敵と接触したかどうか
	/// </summary>
	/// <returns>接触たかどうか</returns>
	inline bool GetIsTouch()const {
		return isTouchStrongEnemy_;
	}

	/// <summary>
	/// ビネットの値を取得
	/// </summary>
	/// <returns>値</returns>
	inline Vignette GetVignette() {
		return vignette_;
	}

protected:
	//テクスチャ管理クラス
	Elysia::TextureManager* textureManager_ = nullptr;
	//入力クラス
	Elysia::Input* input_ = nullptr;
	//レベルエディタ
	Elysia::LevelDataManager* levelDataManager_ = nullptr;
	//ハンドル
	uint32_t levelDataHandle_ = 0u;
	//オーディオ
	Elysia::Audio* audio_ = nullptr;

protected:
	//GameScene本体から持ってくる
	//プレイヤー
	Player* player_ = nullptr;
	//敵管理
	EnemyManager* enemyManager_ = nullptr;
	//鍵管理クラス
	KeyManager* keyManager_ = nullptr;

protected:
	//完全に透明になる値
	const float_t PERFECT_TRANSPARENT_ = 0.0f;
	//完全に不透明になる値
	const float_t PERFECT_NONE_TRANSPARENT_ = 1.0f;
	//フェードの初期座標
	const Vector2 INITIAL_SPRITE_POSITION_ = { .x = 0.0f,.y = 0.0f };
	//時間変化
	const float_t DELTA_TIME_ = 1.0f / 60.0f;

	//線形補間の最小値
	const float_t MIN_T_VALUE_ = 0.0f;
	//線形補間の最大値
	const float_t MAX_T_VALUE_ = 1.0f;

protected:
	//強敵と接触したかどうか
	bool isTouchStrongEnemy_ = false;

	//ビネット
	Vignette vignette_ = {};


};
