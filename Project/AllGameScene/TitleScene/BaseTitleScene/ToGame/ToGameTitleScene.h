#pragma once

/**
* @file ToGameTitleScene.h
* @brief ゲームへ進むクラス
* @author 茂木翼
*/

#include "Sprite.h"
#include "TitleScene/BaseTitleScene/BaseTitleScene.h"

/// <summary>
/// ゲームへ
/// </summary>
class ToGameTitleScene : public BaseTitleScene {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ToGameTitleScene();

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
	~ToGameTitleScene() override =default;

private:
	/// <summary>
	/// ImGui用
	/// </summary>
	void DisplayImGui();


private:
	//透明度の変化
	const float_t INITIAL_TRANSPARENCY = 0.01f;

private:


	//黒フェード
	std::unique_ptr<Elysia::Sprite>blackFade_ = nullptr;
	//透明度
	float_t blackFadeTransparency_ = 0.0f;

};

