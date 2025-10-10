#pragma once

/**
 * @file Player.h
 * @brief プレイヤーのクラス
 * @author 茂木翼
 */

#include "Camera.h"
#include "WorldTransform.h"

/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {
	/// <summary>
	/// 調整項目クラス
	/// </summary>
	class GlobalVariables;

}


/// <summary>
/// プレイヤーのカメラ(目の代わり)
/// </summary>
class PlayerEyeCamera{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerEyeCamera();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerEyeCamera() = default;


private:
	/// <summary>
	/// 振動
	/// </summary>
	void Shake();



public:

	/// <summary>
	/// 本体の座標を設定
	/// </summary>
	/// <param name="position">座標</param>
	inline void SetPlayerPosition(const Vector3& position) {
		this->playerPosition_ = position;
	}

	/// <summary>
	/// カメラ(目)を取得
	/// </summary>
	/// <returns>カメラ(目)</returns>
	inline Camera GetCamera()const {
		return camera_;
	}

	/// <summary>
	/// シータの設定
	/// </summary>
	/// <param name="theta_">シータ</param>
	inline void SetTheta(const float& theta) {
		this->theta_=theta;
	}

	/// <summary>
	/// ファイの設定
	/// </summary>
	/// <param name="phi">ファイ</param>
	inline void SetPhi(const float& phi) {
		this->phi_=phi;
	}

	/// <summary>
	/// シェイクの設定
	/// </summary>
	/// <param name="isShake">シェイク</param>
	inline void SetIsShake(const bool& isShake) {
		this->isShake_ = isShake;
	}




private:
	//グローバル変数クラス
	Elysia::GlobalVariables* globalVariables_ = nullptr;



private:
	//カメラ
	const std::string GAME_SCENE_CAMERA_NAME_ = "GameSceneCamera";
	//ディゾルブ
	const std::string HEIGHT_OFFSET_ = "HeightOffset";


private:
	//ワールドトランスフォーム
	WorldTransform worldTransform_ = {};
	//プレイヤーの座標
	Vector3 playerPosition_ = {};

	//プレイヤー用のカメラ
	Camera camera_ = {};
	//座標のオフセット
	Vector3 cameraPositionOffset_ = {};

	//角度
	float_t theta_ = 0.0f;
	float_t phi_ = 0.0f;


	//振動用
	bool isShake_ = false;
	//振動分
	Vector3 shakeValue_ = {};
	//割合
	float_t shakeRatio_ = 0.0f;
};

