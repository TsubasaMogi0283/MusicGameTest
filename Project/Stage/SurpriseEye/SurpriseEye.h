#pragma once

/**
 * @file SurpriseEye.h
 * @brief びっくり要素のクラス
 * @author 茂木翼
 */


#include <Sprite.h>
#include <memory>

/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {
	/// <summary>
	/// テクスチャ管理クラス
	/// </summary>
	class TextureManager;

	/// <summary>
	/// オーディオ
	/// </summary>
	class Audio;

	/// <summary>
	/// レベルデータ管理クラス
	/// </summary>
	class LevelDataManager;

}

/// <summary>
/// プレイヤー
/// </summary>
class Player;

/// <summary>
/// びっくり要素
/// </summary>
class SurpriseEye{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SurpriseEye();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void DrawSprite();
	
	/// <summary>
	/// デストラクタ
	/// </summary>
	~SurpriseEye()=default;

private:
	/// <summary>
	/// 衝突処理
	/// </summary>
	void CollisionProcess();


public:
	/// <summary>
	/// 非表示の設定
	/// </summary>
	inline void SetIsInvisible() {
		this->isInvisible_=false;
	}

	/// <summary>
	/// レベルデータのハンドルを設定
	/// </summary>
	/// <param name="levelHandle"></param>
	inline void SetLevelDataHandle(const uint32_t& levelHandle) {
		this->levelHandle_ = levelHandle;
	}

	/// <summary>
	/// プレイヤーの設定
	/// </summary>
	/// <param name="player"></param>
	inline void  SetPlayer(Player* player) {
		this->player_ = player;
	}

private:
	//テクスチャ管理クラス
	Elysia::TextureManager* textureManager_ = nullptr;
	//オーディオ
	Elysia::Audio* audio_ = nullptr;
	//レベルエディタ
	Elysia::LevelDataManager* levelDataManager_ = nullptr;
	//ハンドル
	uint32_t levelHandle_ = 0u;
	//プレイヤー
	Player* player_ = nullptr;

private:
	//時間変化
	const float_t DELTA_TIME_ = 1.0f / 60.0f;
	//最大表示時間
	const float_t MAX_DISPLAY_TIME_ = 1.0f;
	//最大距離
	const float_t DISTANCE_ = 5.0f;
private:
	//目のスプライト
	std::unique_ptr<Elysia::Sprite>eye_ = nullptr;
	//びっくりSE
	uint32_t surpriseSeHandle_ = 0u;
	bool isPlaySE_ = false;

	//非表示
	bool isInvisible_ = true;
	//表示の時間
	float_t displayTime_ = 0.0f;

	//コリジョンの座標
	Vector3 eventCollisionPosition_ = {};

	//処理が終わったかどうか
	bool isEndProcess_ = false;
};

