#pragma once


/**
 * @file ExplanationGameScene.h
 * @brief 説明のクラス
 * @author 茂木翼
 */

#include <array>

#include "GameScene/BaseGameScene/BaseGameScene.h"



/// <summary>
/// 説明
/// </summary>
class ExplanationGameScene : public BaseGameScene {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ExplanationGameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="gameScene">ゲームシーン(メイン)</param>
	void Update(GameScene* gameScene)override;

	/// <summary>
	/// 3Dモデルの描画
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="spotLight"></param>
	void DrawObject3D([[maybe_unused]] const Camera& camera, [[maybe_unused]] const SpotLight& spotLight)override {};

	/// <summary>
	/// スプライト
	/// </summary>
	void DrawSprite()override;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ExplanationGameScene()override=default;


private:
	/// <summary>
	/// ImGui表示用
	/// </summary>
	void DisplayImGui()override;

private:

	//場面
	//説明
	bool isExplain_ = false;
	//ゲーム
	bool isGamePlay_ = false;


	//説明の数
	static const uint32_t EXPLANATION_QUANTITY_ = 2u;
	//説明スプライト
	std::array<std::unique_ptr<Elysia::Sprite>, EXPLANATION_QUANTITY_> explanation_ = { nullptr };

	//Spaceで次に進むテキスト
	std::array<std::unique_ptr<Elysia::Sprite>, EXPLANATION_QUANTITY_> spaceToNext_ = { nullptr };

	//テクスチャの番号
	uint32_t howToPlayTextureNumber_ = 0u;

	//決定音のハンドル
	uint32_t desideSeHandle_ = 0u;
	//音量
	float_t desideSeVolume_ = 0.5f;
};