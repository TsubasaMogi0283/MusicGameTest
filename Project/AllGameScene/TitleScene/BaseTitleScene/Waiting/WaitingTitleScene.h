#pragma once

/**
 * @file WaitingTitleScene.h
 * @brief スタート待ちのタイトルシーンクラス
 * @author 茂木翼
 */

#include "Sprite.h"

#include "TitleScene/BaseTitleScene/BaseTitleScene.h"


/// <summary>
/// スタート待ちのタイトルシーン
/// </summary>
class WaitingTitleScene: public BaseTitleScene {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	WaitingTitleScene();

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
	~WaitingTitleScene()override=default;


private:
	/// <summary>
	/// ImGui表示用
	/// </summary>
	void DisplayImGui()override;

private:
	//点滅の制限
	const uint32_t FLASH_TIME_LIMIT_ = 30u;
	//高速点滅の制限
	const uint32_t FAST_FLASH_TIME_LIMIT_ = 60u;
	//高速点滅の間隔
	const uint32_t FAST_FLASH_TIME_INTERVAL_ = 3u;
	//カウントが増える時間
	const uint32_t INCREASE_COUNT_TIME = 0u;
	//表示
	const uint32_t DISPLAY_TIME_ = 0u;
	//点滅の間隔
	const uint32_t FLASH_INTERVAL_ = 2u;
private:

	//テキスト
	std::unique_ptr<Elysia::Sprite> textSprite_ = nullptr;
	//ロゴ
	std::unique_ptr<Elysia::Sprite> logoSprite_ = nullptr;

	//点滅
	bool isFlash_ = true;
	//時間
	uint32_t flashTime_ = 0u;
	
	//高速点滅
	bool isFastFlash_ = false;
	//時間
	uint32_t fastFlashTime_ = 0u;


	//見せる番号
	uint32_t textDisplayCount_ = 0;

	

};