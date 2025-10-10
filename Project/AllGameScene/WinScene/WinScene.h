#pragma once
/**
 * @file WinScene.h
 * @brief 勝利シーン
 * @author 茂木翼
 */
#include <memory>

#include "IGameScene.h"
#include "Sprite.h"
#include "Model.h"
#include "Camera.h"
#include "Material.h"
#include "DirectionalLight.h"
#include "BackTexture.h"

#include "BaseWinScene/BaseWinScene.h"


/// <summary>
/// ElysiaEngine(前方宣言)
/// </summary>
namespace Elysia {

	/// <summary>
	/// 入力
	/// </summary>
	class Input;

	/// <summary>
	/// レベルデータ管理クラス
	/// </summary>
	class LevelDataManager;


	/// <summary>
	/// ゲーム管理クラス
	/// </summary>
	class GameManager;

	/// <summary>
	/// オーディオ
	/// </summary>
	class Audio;
}



/// <summary>
/// 勝利シーン
/// </summary>
class WinScene : public Elysia::IGameScene {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	WinScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="gameManager">ゲーム管理クラス</param>
	void Update(Elysia::GameManager* gameManager)override;

	/// <summary>
	/// ポストエフェクト描画処理前
	/// </summary>
	void PreDrawPostEffect()override;

	/// <summary>
	/// 描画(3D描画)
	/// </summary>
	void DrawObject3D()override;

	/// <summary>
	/// ポストエフェクト描画
	/// </summary>
	void DrawPostEffect()override;

	/// <summary>
	/// スプライトの描画
	/// </summary>
	void DrawSprite()override;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~WinScene()override=default;

public:
	/// <summary>
	/// 敗北の細かいシーンの遷移
	/// </summary>
	/// <param name="">細かいシーン</param>
	void ChangeDetailScene(std::unique_ptr<BaseWinScene> detailScene);

	/// <summary>
	/// カメラの速度を設定
	/// </summary>
	/// <param name="cameraVelocity">速度分</param>
	inline void SetCameraVelocity(const Vector3& cameraVelocity) {
		this->cameraVelocity_ = cameraVelocity;
	}

	/// <summary>
	/// 終わったことを設定する
	/// </summary>
	inline void SetIsEnd() {
		this->isEnd_ = true;
	}

	/// <summary>
	/// BGMの音量を下げる
	/// </summary>
	/// <param name="downValue"></param>
	inline void BgmVolumeDown(const float_t& downValue) {
		this->bgmVolume_ -= downValue;
		//0より小さくしないようにする
		if (this->bgmVolume_ <= 0.0f) {
			this->bgmVolume_ = 0.0f;
		}
	}

private:
	//レベルデータ管理クラス
	Elysia::LevelDataManager* levelDataManager_ = nullptr;
	//ハンドル
	uint32_t levelDataHandle_ = 0u;
	//オーディオ
	Elysia::Audio* audio_ = nullptr;

private:

	//テキスト
	const std::string ESCAPE_SUCCEEDED_TEXT = "EscapeSucceededObject";
	//浮遊用のθ
	const float_t FLOATING_THETA_ = 0.05f;
	//Y座標
	const float_t BASED_POSITION_Y_ = 6.0f;

private:
	//カメラ
	Camera camera_ = {};
	//速度
	Vector3 cameraVelocity_ = {};

	//平行光源
	DirectionalLight directionalLight_ = {};

	//ポストエフェクト
	std::unique_ptr<Elysia::BackTexture> backTexture_ = nullptr;

	//オブジェクトの浮遊感を出す
	float_t objectFloatingTheta_ = 0.0f;


	//細かいシーン
	std::unique_ptr<BaseWinScene> detailWinScene_ = nullptr;
	//処理が終わったかどうか
	bool isEnd_ = false;

	//勝利のBGMハンドル
	uint32_t bgmHandle_ = 0u;
	//音量
	float_t bgmVolume_ = 0.8f;
};

