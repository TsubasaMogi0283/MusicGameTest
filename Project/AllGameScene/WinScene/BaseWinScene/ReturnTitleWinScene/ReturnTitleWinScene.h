#pragma once
/**
 * @file ReturnTitleWinScene.h
 * @brief タイトルに戻るクラス
 * @author 茂木翼
 */

#include "Sprite.h"

#include "WinScene/BaseWinScene/BaseWinScene.h"


/// <summary>
/// タイトルに戻る
/// </summary>
class ReturnTitleWinScene : public BaseWinScene {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ReturnTitleWinScene();

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
	~ReturnTitleWinScene()override= default;

private:
	/// <summary>
	/// ImGui表示用
	/// </summary>
	void DisplayImGui()override;

private:
	//動くのが終わる時間
	const float_t MOVE_END_TIME_ = 4.0f;
	//透明度の増える値
	const float_t INCREASE_TRANSPARENCY_VALUE_ = 0.01f;

private:
	//背景のスプライト
	std::unique_ptr<Elysia::Sprite> whiteFadeSprite_ = nullptr;
	//透明度
	float_t transparency_ = 0.0f;

	//速度
	Vector3 cameraVelocity_ = {};
	//加速
	Vector3 cameraAcceleration_ = {};
	//動く時間
	float_t moveTime_ = 0.0f;

	//スウィープアップ音(「シュー」っと上がっていく音)のハンドル
	uint32_t sweepUpSEHandle_ = 0u;
	//音量
	float_t sweepUpSEVolume_ = 0.5f;


};

