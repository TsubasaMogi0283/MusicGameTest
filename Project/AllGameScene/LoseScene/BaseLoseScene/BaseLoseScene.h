#pragma once
/**
 * @file BaseLoseScene.h
 * @brief 細かい敗北シーンの基底クラス
 * @author 茂木翼
 */

#include <cstdint>

#include "Model.h"
#include "Sprite.h"
#include "Camera.h"
#include "BackTexture.h"
#include "PointLight.h"
#include "Dissolve.h"

/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {
	/// <summary>
	/// 入力
	/// </summary>
	class Input;

	/// <summary>
	/// レベル管理クラス
	/// </summary>
	class LevelDataManager;

	/// <summary>
	/// ゲーム管理クラス
	/// </summary>
	class GameManager;

	/// <summary>
	/// テクスチャ管理クラス
	/// </summary>
	class TextureManager;

	/// <summary>
	/// グローバル変数
	/// </summary>
	class GlobalVariables;

	/// <summary>
	/// オーディオ
	/// </summary>
	class Audio;
}


/// <summary>
/// 敗北シーン(メイン)
/// </summary>
class LoseScene;

/// <summary>
/// 細かい敗北シーンの基底クラス
/// </summary>
class BaseLoseScene {
public:

	/// <summary>
	/// 個別の初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="loseScene">失敗シーン(メイン)</param>
	virtual void Update(LoseScene* loseScene) = 0;


	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseLoseScene() = default;


public:


	/// <summary>
	/// 点光源を取得
	/// </summary>
	/// <returns>点光源</returns>
	inline PointLight GetPointLight() const{
		return pointLight_;
	}

	/// <summary>
	/// ディゾルブの取得
	/// </summary>
	/// <returns>ディゾルブ</returns>
	inline Dissolve GetDissolve()const {
		return dissolve_;
	}
		
	/// <summary>
	/// カメラの加速分の値を取得
	/// </summary>
	/// <returns>加速分</returns>
	inline Vector3 GetCameraVelocity()const {
		return cameraVelocity_;
	}


	/// <summary>
	/// レベルデータハンドルの設定
	/// </summary>
	/// <param name="levelDataHandle">レベルデータのハンドル</param>
	inline void SetLevelDataHandle(const uint32_t& levelDataHandle) {
		this->levelDataHandle_ = levelDataHandle;
	}

protected:
	//入力クラス
	Elysia::Input* input_ = nullptr;
	//テクスチャ管理クラス
	Elysia::TextureManager* textureManager_ = nullptr;
	//レベルデータ管理クラス
	Elysia::LevelDataManager* levelDataManager_ = nullptr;
	//ハンドル
	uint32_t levelDataHandle_ = 0u;

	//モデル管理クラス
	Elysia::ModelManager* modelManager_ = nullptr;
	//グローバル変数クラス
	Elysia::GlobalVariables* globalVariables_ = nullptr;
	//オーディオ
	Elysia::Audio* audio_ = nullptr;

protected:
	//線形補間で使う変数の最大値
	const float_t MAX_T_VALUE_ = 1.0f;
	//増える時間の値
	const uint32_t INCREASE_VALUE = 1u;
	//完全透明値
	const float_t PERFECT_TRANSPARENT_ = 0.0f;
	//完全不透明値
	const float_t PERFECT_NO_TRANSPARENT_ = 1.0f;

	//時間変化
	const float_t DELTA_TIME_ = 1.0f / 60.0f;
	//点光源の最大の半径
	const float MAX_LIGHT_RADIUS_ = 11.8f;
	//最大値
	const float_t MAX_VOLUME_ = 0.9f;
	//最小値
	const float_t MIN_VOLUME_ = 0.0f;


	//テキストの名前
	//矢印
	const std::string SELECT_ARROW = "SelectArrow";
	//ゲーム
	const std::string TO_GAME = "ToGame";
	//タイトル
	const std::string TO_TITLE = "ToTitle";

	
protected:
	
	//点光源
	PointLight pointLight_ = {};
	//ディゾルブ
	Dissolve dissolve_ = {};
	//速度
	Vector3 cameraVelocity_ = {};
	

};
