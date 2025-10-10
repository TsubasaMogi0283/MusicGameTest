#pragma once
/**
 * @file KeyManager.h
 * @brief 鍵のクラス
 * @author 茂木翼
 */

#include <memory>
#include <list>
#include <sstream>

#include "Audio.h"
#include "Key.h"
#include "Sprite.h"


#pragma region 前方宣言

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
/// EllysiaEngine(前方宣言)
/// </summary>
namespace Elysia {
	/// <summary>
	/// オーディオ
	/// </summary>
	class Audio;

	/// <summary>
	/// テクスチャ管理クラス
	/// </summary>
	class TextureManager;

	/// <summary>
	/// 入力
	/// </summary>
	class Input;

	/// <summary>
	/// レベルデータ管理クラス
	/// </summary>
	class LevelDataManager;
}


#pragma endregion


/// <summary>
/// 鍵管理クラス
/// </summary>
class KeyManager {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	KeyManager();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelHandle"></param>
	/// <param name="positions"></param>
	void Initialize(const uint32_t& modelHandle, const std::vector<Vector3>& positions);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// オブジェクトの描画
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="spotLight"></param>
	void DrawObject3D(const Camera& camera, const SpotLight& spotLight);

	/// <summary>
	/// スプライトの描画
	/// </summary>
	void DrawSprite();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~KeyManager() = default;


private:

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="position">座標</param>
	/// <param name="isAbleToPickUp">取得可能か</param>
	void Genarate(const Vector3& position,const bool& isAbleToPickUp);

	/// <summary>
	/// 消去
	/// </summary>
	void Delete();

	/// <summary>
	/// 取得
	/// </summary>
	void PickUp();

	/// <summary>
	/// ImGui表示用
	/// </summary>
	void DisplayImGui();


public:
	/// <summary>
	/// レベルデータハンドルの設定
	/// </summary>
	/// <param name="handle"></param>
	inline void SetLevelDataHandle(const uint32_t& handle) {
		this->levelDataHandle_ = handle;
	}

	/// <summary>
	/// 宝箱を開けたかどうか
	/// </summary>
	/// <param name="isOpen">開けたかどうかのフラグ</param>
	inline void SetIsOpenTreasureBox(const bool& isOpen) {
		this->isOpenTreasureBox_ = isOpen;
	}

	/// <summary>
	/// 鍵のリストを取得
	/// </summary>
	/// <returns>リスト</returns>
	inline std::list<Key*> GetKeyes() const {
		std::list <Key*> keys = {};
		for (const std::unique_ptr<Key>& key : keies_) {
			keys.push_back(key.get());
		}

		return keys;
	}

	/// <summary>
	/// 今ステージ上にある鍵の数を取得
	/// </summary>
	/// <returns>鍵の数</returns>
	inline uint32_t GetKeyQuantity() const {
		return static_cast<uint32_t>(keies_.size());
	}

	/// <summary>
	/// 最大で取得できる数
	/// </summary>
	/// <returns>最大数</returns>
	inline uint32_t GetMaxKeyQuantity()const {
		return MAX_KEY_QUANTITY_;
	}

	/// <summary>
	/// 墓場の鍵を取得したかどうか
	/// </summary>
	/// <returns>取得のフラグ</returns>
	inline bool GetIsPickUpKeyInCemetery()const {
		return isPickUpKeyInCemetery_;
	}


public:
	/// <summary>
	/// プレイヤー
	/// </summary>
	/// <param name="player">プレイヤー</param>
	inline void SetPlayer(Player* player) {
		this->player_ = player;
	};

	/// <summary>
	/// 音が止まる
	/// </summary>
	void StopAudio();

private:
	//オーディオ
	Elysia::Audio* audio_ = nullptr;
	//テクスチャ管理クラス
	Elysia::TextureManager* textureManager_ = nullptr;
	//入力
	Elysia::Input* input_ = nullptr;
	//レベルデータ管理クラス
	Elysia::LevelDataManager* levelDataManager_ = nullptr;
	//ハンドル
	uint32_t levelDataHandle_ = 0u;
	//プレイヤー
	Player* player_ = nullptr;


private:
	//最大数
	static const uint32_t MAX_KEY_QUANTITY_ = 3u;
	//数
	static const uint32_t NUMBER_QUANTITY_ = 10u;
	//鍵の音が聞こえる最大距離
	const float_t MAX_DISTANCE_ = 30.0f;


private:
	//鍵
	std::list<std::unique_ptr<Key>>keies_ = {};
	//モデルハンドル
	uint32_t modelHandle_ = 0u;

	//UI
	std::unique_ptr<Elysia::Sprite> keyListSprite_ = nullptr;
	//鍵
	std::array<std::unique_ptr<Elysia::Sprite>, MAX_KEY_QUANTITY_> keySprites_ = { nullptr };
	//サイズ
	uint64_t keySpriteWidth_ = 0u;
	uint64_t keySpriteHeight_ = 0u;

	//スケール
	std::array<float_t, MAX_KEY_QUANTITY_> spriteTs_ = {};

	//初期座標
	Vector2 initialPosition_ = { .x = 20.0f,.y = 10.0f };
	Vector2 initialPositionAddAnchorPoint = {};

	//終点座標
	std::array<Vector2, MAX_KEY_QUANTITY_> endPositions_ = {};
	//鍵取得するかどうか
	std::unique_ptr<Elysia::Sprite> pickUpKey_ = nullptr;
	//プレイヤーと全ての鍵の距離
	std::list<float_t>keyAndPlayerDistances_ = {};

	//拾う音
	uint32_t pickUpSEHandle = 0u;
	//鍵の場所を知らせる音
	uint32_t notificationSEHandle_ = 0u;
	//取得可能か
	bool isAbleToPickUpKey_ = false;
	//鍵の数
	uint32_t keyQuantity_ = 0u;

	//宝箱に入っている鍵用
	//宝箱を開けたかどうか
	bool isOpenTreasureBox_ = false;
	//取得したかどうか
	bool isPickUpKeyInHouse_ = false;
	//皿が落ちる音
	uint32_t dropPlateTime_ = 0u;
	uint32_t dropPlateSEHandle_ = 0u;


	//墓場の鍵を取ったかどうか
	bool isPickUpKeyInCemetery_ = false;

	

};

