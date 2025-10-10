#pragma once
/**
 * @file BaseTitleScene.h
 * @brief 細かいタイトルシーンの基底クラス
 * @author 茂木翼
 */

#include <cstdint>

#include "DirectionalLight.h" 
#include "TitleScene/BaseBackTexture/BaseTitleBackTexture.h"

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
/// タイトルシーン(メイン)
/// </summary>
class TitleScene;

/// <summary>
/// 細かいタイトルシーンの基底クラス
/// </summary>
class BaseTitleScene {
public:

	/// <summary>
	/// 個別の初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="titleScene">タイトルシーン(メイン)</param>
	virtual void Update(TitleScene* titleScene) = 0;

	

	/// <summary>
	/// スプライト
	/// </summary>
	virtual void DrawSprite() = 0;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseTitleScene() = default;


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
	/// 平行光源の取得
	/// </summary>
	/// <returns>平行光源</returns>
	inline DirectionalLight GetDirectionalLight()const {
		return directionalLight_;
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
	//再スタート時間
	const uint32_t RESTART_TIME = 0u;
	//増える時間
	const uint32_t INCREASE_VALUE_ = 1u;
	//通常の時間変化
	const float_t DELTA_TIME = 1.0f / 60.0f;

	//スプライトの初期座標
	const Vector2 SPRITE_INITIAL_POSITION_ = { .x = 0.0f,.y = 0.0f };
	//完全不透明値
	const float_t PERFECT_NO_TRANSPARENT_ = 1.0f;


protected:
	//平行光源
	DirectionalLight directionalLight_ = {};
	//決定音のハンドル
	uint32_t desideSEHandle_ = 0u;
	//音量
	float_t desideSEVolume_ = 0.5f;
};
