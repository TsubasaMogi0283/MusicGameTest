#pragma once

/**
 * @file GameScene.h
 * @brief ゲームシーンのクラス
 * @author 茂木翼
 */

#include <memory>
#include <array>
#include <vector>
#include "IGameScene.h"
#include "Sprite.h"
#include <Model.h>
#include "Camera.h"
#include "SpotLight.h"

#include "Player/Player.h"
#include "CollisionManager.h"
#include "Key/KeyManager.h"
#include "Enemy/EnemyManager.h"
#include "Stage/Gate/Gate.h"
#include "CollisionCalculation.h"
#include <BackTexture.h>
#include "VignettePostEffect.h"
#include "Vignette.h"
#include "BaseGameScene/BaseGameScene.h"


 /// <summary>
 /// ElysiaEngine(前方宣言)
 /// </summary>
namespace Elysia {
	/// <summary>
	/// ウィンドウクラス
	/// </summary>
	class WindowsSetup;

	/// <summary>
	/// レベルエディタ
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
	/// モデル管理クラス
	/// </summary>
	class ModelManager;

	/// <summary>
	/// 入力クラス
	/// </summary>
	class Input;

	/// <summary>
	/// 調整項目クラス
	/// </summary>
	class GlobalVariables;

	/// <summary>
	/// オーディオ
	/// </summary>
	class Audio;
}

struct Note {
	int32_t left;
	int32_t middle;
	int32_t right;
};
/// <summary>
/// 1小節
/// </summary>
struct NoteBar {

	//ノーツの数
	std::vector<Note >notes;
	//BPM
	uint32_t bpm;
};

struct NoteInstance {
	// 0=左, 1=中, 2=右
	int32_t lane=0;   
	
	//判定線に到着する時間
	float_t judgementArrivalTime=0.0f;
	//動き出す時間
	float_t startMoveTime = 0.0f;
	// 表示中かどうか
	bool isDisplay=false; 
	//座標
	Vector2 position = {};
	//ノーツのスプライト
	std::unique_ptr<Elysia::Sprite> noteSprite=nullptr;


	//
	bool isPlaySE_ = false;
};

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public Elysia::IGameScene {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="gameManager"></param>
	void Update(Elysia::GameManager* gameManager)override;

	/// <summary>
	/// 3Dオブジェクト
	/// </summary>
	void DrawObject3D()override;

	/// <summary>
	/// ポストエフェクト描画前
	/// </summary>
	void PreDrawPostEffect()override;

	/// <summary>
	/// ポストエフェクトの描画
	/// </summary>
	void DrawPostEffect()override;

	/// <summary>
	/// スプライト
	/// </summary>
	void DrawSprite()override;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene() = default;

private:
	/// <summary>
	/// ImGuiの表示
	/// </summary>
	void DisplayImGui();

private:
	//ウィンドウズ
	Elysia::WindowsSetup* windowsSetup_ = nullptr;
	//インプット
	Elysia::Input* input_ = nullptr;
	//テクスチャ管理クラス
	Elysia::TextureManager* textureManager_ = nullptr;
	//モデル管理クラス
	Elysia::ModelManager* modelManager_ = nullptr;
	//レベルエディタ
	Elysia::LevelDataManager* levelDataManager_ = nullptr;
	//ハンドル
	uint32_t levelHandle_ = 0u;
	//グローバル変数クラス
	Elysia::GlobalVariables* globalVariables_ = nullptr;
	//オーディオ
	Elysia::Audio* audio_ = nullptr;

private:
	//時間変化
	const float_t DELTA_TIME_ = 1.0f / 60.0f;
	//BPM(テンポ)
	const uint32_t BPM_=180u;
	//待ち最大時間
	const float_t WAIT_LIMIT_TIME_ = 3.0f;
	//待機の座標
	const float_t WAITING_POSITION_Y_ = -50.0f;
	//判定の座標
	const float_t JUDGEMENT_POSITION_Y_ = 650.0f;

	//動き出す時間のオフセット
	const float_t START_OFFSET_TIME_ = 1.0f;

private:
	
	// ノーツが画面上端から判定線まで移動する基本時間（秒）
	const float_t BASE_APPEAR_OFFSET = 4.0f;

	// ハイスピード倍率
	float_t highSpeedRate_ = 1.0f; // 初期値は1.0倍速

private:
	//カメラ
	Camera camera_ = {};
	//ポストエフェクト
	std::unique_ptr<Elysia::BackTexture>backTexture_ = nullptr;
	//平行光源
	DirectionalLight directionalLight_ = {};

	//ボス
	//モデル
	std::unique_ptr<Elysia::Model>boss_ = nullptr;
	WorldTransform bossWorldTransform_ = {};
	Material bossMaterial_ = {};

	//レーン
	std::unique_ptr<Elysia::Sprite>base_ = nullptr;
	//判定
	std::unique_ptr<Elysia::Sprite>judgement_ = nullptr;


	//パート(攻防)
	std::unique_ptr<Elysia::Sprite>part_ = nullptr;
	uint32_t partTextureHandle_ = 0u;
	uint32_t attackPartTextureHandle_ = 0u;
	uint32_t deffencePartTextureHandle_ = 0u;

	//譜面データ
	std::vector<NoteBar> notesData_;
	//待ち時間
	float_t waitingTime_ = 0.0f;
	//開始までの時間
	float_t startTime_ = 0.0f;
	//開始
	bool isStart_ = false;

	float_t totalTime_ = 0.0f;
	float_t noteMoveTime_ = 0.0f;
	std::vector<NoteInstance> noteInstances_;
	uint32_t noteRadius_ = 0u;

	//レーンの座標
	std::array<float_t, 3u>laneX_ = {100,200,300};
	//曲のハンドル
	uint32_t musicHandle_ = 0u;
	//楽曲の時間
	float_t musicTime_ = 0.0f;

	uint32_t arraiveSEHandle_ = 0u;

};