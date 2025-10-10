#pragma once
/**
 * @file EscapeAssistArrow.h
 * @brief 脱出アシストの矢印のクラス
 * @author 茂木翼
 */

#include <memory>
#include <Model.h>
#include <WorldTransform.h>
#include <Material.h>


/// <summary>
/// カメラ
/// </summary>
struct Camera;

/// <summary>
/// スポットライト
/// </summary>
struct SpotLight;

/// <summary>
/// プレイヤー
/// </summary>
class Player;

/// <summary>
/// 脱出アシストの矢印
/// </summary>
class EscapeAssistArrow{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EscapeAssistArrow() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelHandle"></param>
	/// <param name="gateCenterPosition"></param>
	void Initialize(const uint32_t& modelHandle, const Vector3& gateCenterPosition);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="spotLight">スポットライト</param>
	void Draw(const Camera& camera, const SpotLight& spotLight);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~EscapeAssistArrow() = default;

public:
	/// <summary>
	/// プレイヤーの設定
	/// </summary>
	/// <param name="player"></param>
	inline void SetPlayer(Player* player) {
		this->player_ = player;
	}

	/// <summary>
	/// 不透明にしていくかどうか
	/// </summary>
	/// <param name="isOpaque">不透明にするかのフラグ</param>
	inline void SetIsOpaque(const bool& isOpaque) {
		this->isOpaque_ = isOpaque;
	}


private:
	//プレイヤーと矢印の距離
	const float_t PLAYER_TO_ARROW_DISTANCE_ = 3.0f;
	//大きさ
	const float_t SCALE_ = 0.3f;
	//線形補間の増える量
	const float_t INCREASE_T_VALUE_ = 0.01f;
	//高さ
	const float_t HEIGHT_ = 0.75f;
	//透明度の増える値
	const float_t INCREASE_TRANSPARENCY_VALUE_ = 0.01f;

private:
	//プレイヤー
	Player* player_ = nullptr;

	//脱出アシスト用の矢印
	std::unique_ptr<Elysia::Model>model_ = nullptr;
	WorldTransform worldTransform_ = {};
	Material material_ = {};
	//指し示す動きの量
	float_t indicateValueZ_ = {};
	//線形補間
	float_t indicateT_ = 0.0f;
	
	//不透明にしていくかどうか
	bool isOpaque_ = false;

	//透明度
	float_t transparency_ = 0.0f;

	//門の中心座標
	Vector3 gateCenterPosition_ = {};
	//差分
	Vector3 difference_ = {};
	//角度
	float_t arrowTheta_ = 0.0f;

};

