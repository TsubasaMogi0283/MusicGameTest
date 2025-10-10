#pragma once

/**
 * @file StartGameScene.h
 * @brief ゲームの始めのクラス
 * @author 茂木翼
 */

#include "GameScene/BaseGameScene/BaseGameScene.h"

/// <summary>
/// ゲームの始め
/// </summary>
class StartGameScene : public BaseGameScene {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	StartGameScene();

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
	~StartGameScene()override=default;


private:
	/// <summary>
	/// ImGui表示用
	/// </summary>
	void DisplayImGui()override;

private:
	//フェードインの増える値
	const float_t FADE_VALUE_ = 0.01f;
	//最大音量
	const float_t MAX_VOLUME_ = 0.4f;
	//最小音量
	const float_t MIN_VOLUME_ = 0.0f;
	//音量の増える値
	const float_t VOLUME_INCREASE_VALUE_ = 0.01f;

private:
	//白フェードのスプライト
	std::unique_ptr<Elysia::Sprite> blackFadeSprite_ = nullptr;
	//透明度
	float_t fadeTransparency_ = 1.0f;

	//環境音の音量
	float_t enviromentAudioVolume_ = 0.0f;
};