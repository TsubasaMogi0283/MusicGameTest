#pragma once
/**
 * @file BaseWinScene.h
 * @brief 細かい勝利シーンの基底クラス
 * @author 茂木翼
 */

#include <cmath>


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
	/// グローバル変数
	/// </summary>
	class GlobalVariables;

	/// <summary>
	/// オーディオ
	/// </summary>
	class Audio;
}


/// <summary>
/// 勝利シーン(メイン)
/// </summary>
class WinScene;

/// <summary>
/// 細かい勝利シーンの基底クラス
/// </summary>
class BaseWinScene {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="winScene">勝利シーン(メイン)</param>
	virtual void Update(WinScene* winScene) = 0;

	/// <summary>
	/// スプライトの描画
	/// </summary>
	virtual void DrawSprite() = 0;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseWinScene() = default;

protected:
	/// <summary>
	/// ImGui表示用
	/// </summary>
	virtual void DisplayImGui()=0;

public:
	/// <summary>
	/// レベルデータハンドルの設定
	/// </summary>
	/// <param name="levelDataHandle">レベルデータのハンドル</param>
	inline void SetLevelDataHandle(const uint32_t& levelDataHandle) {
		this->levelDataHandle_ = levelDataHandle;
	}

protected:
	//入力クラス
	Elysia::Input* input_ = nullptr;
	//レベルデータ管理クラス
	Elysia::LevelDataManager* levelDataManager_ = nullptr;
	//ハンドル
	uint32_t levelDataHandle_ = 0u;
	//テクスチャ管理クラス
	Elysia::TextureManager* textureManager_ = nullptr;
	//グローバル変数クラス
	Elysia::GlobalVariables* globalVariables_ = nullptr;
	//オーディオ
	Elysia::Audio* audio_ = nullptr;

protected:
	//線形補間で使う変数の最大値
	const float_t MAX_T_VALUE_ = 1.0f;
	//増える時間の値
	const uint32_t INCREASE_VALUE_ = 1u;
	//再スタート時間
	const float_t RESTART_TIME_ = 0.0f;
	//完全透明値
	const float_t PERFECT_TRANSPARENT_ = 0.0f;
	//完全不透明値
	const float_t PERFECT_NO_TRANSPARENT_ = 1.0f;
	//時間変化
	const float_t DELTA_TIME_ = 1.0f / 60.0f;
	//BGMの音量が減る値
	const float_t BGM_VOLUME_DECREASE_VALUE_ = 0.01f;

	//スプライトの初期位置
	const Vector2 INITIAL_SPRITE_POSITION_ = { .x = 0.0f,.y = 0.0f };
};
