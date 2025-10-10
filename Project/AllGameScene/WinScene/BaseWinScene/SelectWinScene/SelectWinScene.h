#pragma once
/**
 * @file SelectWinScene.h
 * @brief 選択シーンのクラス
 * @author 茂木翼
 */


#include "Sprite.h"
#include "WinScene/BaseWinScene/BaseWinScene.h"

/// <summary>
/// 選択(と言ってもタイトルに戻るだけの)シーン
/// </summary>
class SelectWinScene : public BaseWinScene {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SelectWinScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="winScene">勝利シーン(メイン)</param>
	void Update(WinScene* winScene) override;

	/// <summary>
	/// スプライトの描画
	/// </summary>
	void DrawSprite()override;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SelectWinScene()override= default;

private:
	/// <summary>
	/// ImGui表示用
	/// </summary>
	void DisplayImGui()override;

private:
	
	//この時間になるまで点滅
	const float_t FLASH_TIME_LIMIT_ = 0.5f;
	//点滅が終わる時間
	const float_t FLASH_END_TIME_ = FLASH_TIME_LIMIT_*2.0f;
	//高速点滅の間隔
	const uint32_t FAST_FLASH_TIME_INTERVAL_ = 5u;

	//この時間になるまで高速点滅
	const uint32_t FAST_FLASH_TIME_LIMIT_ = 1u;
	//カウントが増える時間
	const uint32_t INCREASE_COUNT_TIME_ = 0u;
	//点滅の間隔
	const uint32_t FLASH_INTERVAL_ = 2u;
	//表示
	const uint32_t DISPLAY_ = 0u;
	//最大表示回数
	const uint32_t MAX_DISPLAY_ = 10u;
	//高速点滅の時間変化
	const uint32_t FAST_FLASH_DELTA_TIME_ = 1u;
private:

	//タイトルへのスプライト
	std::unique_ptr<Elysia::Sprite> textSprite_ = nullptr;
	//点滅時間
	float_t flashTime_ = 0.0f;

	//見せるカウント
	uint32_t textDisplayCount_ = 0u;
	//高速点滅時間
	uint32_t fastFlashTime_ = 0u;
	//決定したかどうか
	bool isDeside_ = false;

	//決定音のハンドル
	uint32_t desideSEHandle_ = 0u;
	//音量
	float_t desideSEVolume_ = 0.5f;

};

