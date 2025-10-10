#pragma once
/**
 * @file ReturnTitleLoseScene.h
 * @brief タイトルに戻るクラス
 * @author 茂木翼
 */
#include "LoseScene/BaseLoseScene/BaseLoseScene.h"

/// <summary>
/// ゲームを続けるシーン
/// </summary>
class ReturnTitleLoseScene : public BaseLoseScene {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ReturnTitleLoseScene();

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
	~ReturnTitleLoseScene()override= default;

private:
	/// <summary>
	/// ImGui表示用
	/// </summary>
	void DisplayImGui();


private:
	//矢印の回転
	const float_t ROTATE_VALUE_ = 0.1f;
	//遷移する時間
	const float_t CHANGE_NEXTSCENE_TIME_ = 2.0f;
	//透明度の間隔
	const float_t TRANSPARENCY_INTERVAL_ = 0.01f;
	//終わる時間
	const float_t FINISH_WAIT_TIME_ = 1.0f;
	//ディゾルブの閾値が増える値
	const float_t THRESHOLD_INCREASE_AMOUNT_ = 0.01f;
	//音の高さの減る値
	const float_t DECREASE_FREAQUENCY_RATIO_VALUE_ = 0.003f;
	//最小の高さ
	const float_t MIN_FREAQUENCY_RATIO_VALUE_ = 0.2f;
	//音量が下がる値
	const float_t DECREASE_VOLUME_VALUE_ = 0.005f;

private:
	//矢印の初期座標を取得
	Vector3 arrowInitialPosition_ = {};
	//「タイトルへ」の座標を取得
	Vector3 toTitleInitialPosition_ = {};

	//決定時の動きに使うθ
	float_t decideArrowMoveTheta_ = 0.0f;
	//変化までの時間
	float_t changeWaitingTime_ = 0.0f;
	//透明度の設定
	float_t fadeTransparency_ = 0.0f;
	//ディゾルブの閾値用の線形補間の値
	float_t dissolveThresholdT_ = 0.0f;
	//暗転した後の待ち時間
	float_t waitTime_ = 0.0f;

	//BGMの音の高さ
	float_t bgmFreaquencyRatio_ = 1.0f;
	//音量
	float_t bgmVolume_ = MAX_VOLUME_;
};

