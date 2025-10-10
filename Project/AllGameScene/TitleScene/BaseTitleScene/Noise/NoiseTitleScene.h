#pragma once


/**
 * @file NoiseTitleScene.h
 * @brief ノイズのクラス
 * @author 茂木翼
 */

#include <array>

#include "Sprite.h"
#include "TitleScene/BaseTitleScene/BaseTitleScene.h"

/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {
	/// <summary>
	/// オーディオ
	/// </summary>
	class Audio;
}

/// <summary>
/// ノイズ
/// </summary>
class NoiseTitleScene : public BaseTitleScene {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	NoiseTitleScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="titleScene">タイトルシーン(メイン)</param>
	void Update(TitleScene* titleScene)override;

	/// <summary>
	/// スプライト
	/// </summary>
	void DrawSprite()override;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~NoiseTitleScene()override=default;


private:
	/// <summary>
	/// ImGui表示用
	/// </summary>
	void DisplayImGui()override;

private:
	//オーディオ
	Elysia::Audio* audio_ = nullptr;

private:	
	//ランダムエフェクトの表示時間の数
	static const uint32_t DISPLAY_LENGTH_QUANTITY_ = 2u;
	//開始時間
	const std::array<float_t, DISPLAY_LENGTH_QUANTITY_> RANDOM_EFFECT_DISPLAY_START_TIME = { 0.0f,2.5f };
	//表示の長さ
	const std::array<float_t, DISPLAY_LENGTH_QUANTITY_> RANDOM_EFFECT_DISPLAY_LENGTH = { 1.0f,2.5f };
	//1回目のエフェクト
	const uint32_t FIRST_EFFECT = 0u;
	//2回目のエフェクト
	const uint32_t SECOND_EFFECT = 1u;
	//ノイズ再生時のボリューム
	const float_t PLAY_NOISE_AUDIO_VALUME_ = 0.7f;

private:
	//ロゴ
	std::unique_ptr<Elysia::Sprite> logoSprite_ = nullptr;
	//ランダムエフェクトの時間
	float_t randomEffectTime_ = 0.0f;
	//表示終了
	bool isEndDisplay_ = false;

	//ノイズ音のハンドル
	uint32_t noiseHandle_ = 0u;
	float_t noiseVolume_ = 0.0f;
};