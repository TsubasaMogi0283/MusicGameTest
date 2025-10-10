#pragma once

/**
* @file LightUpLoseScene.h
* @brief ライトアップシーンのクラス
* @author 茂木翼
*/

#include "LoseScene/BaseLoseScene/BaseLoseScene.h"

/// <summary>
/// ライトアップ
/// </summary>
class LightUpLoseScene : public BaseLoseScene{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	LightUpLoseScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="loseScene">失敗シーン(メイン)</param>
	void Update(LoseScene* loseScene) override;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LightUpLoseScene() override =default;

private:
	/// <summary>
	/// ImGui用
	/// </summary>
	void DisplayImGui();


private:
	//増える間隔
	const float_t INTERVAL_ = 0.025f;
	//遷移する時間
	const float_t CHANGE_NEXTSCENE_TIME_ = 2.0f;



private:
	//半径の線形補間
	float startLightUpT_ = 0.0f;

	//テキストの透明度
	float textTransparency_ = 0.0f;
	//透明度を設定するときの値
	float transparencyT_ = 0.0f;


	//遷移まで待つ時間
	float_t waitForNextSceneTime_ = 0.0f;
};

