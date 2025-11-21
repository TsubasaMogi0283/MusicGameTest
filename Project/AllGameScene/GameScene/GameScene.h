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

struct NotePosition {
	int32_t left;
	int32_t middle;
	int32_t right;
};

struct InputCondition {
	bool isInputLeft;
	bool isInputMiddle;
	bool isInputRight;
};

enum LanePosition {
	Left,
	Middle,
	Right,
};


/// <summary>
/// 1小節
/// </summary>
struct NoteBar {

	//ノーツの数
	std::vector<NotePosition >notes;
	//BPM
	uint32_t bpm;
};

enum NoteSelection {
	NoneNote,
	TapNote,
	LongNoteStart,
	LongNoteEnd,
};

enum NoteJudgementSelection {
	Perfect,
	Great,
	Good,
	Miss,
	None,
};




struct NoteInstance {
	// 0=左, 1=中, 2=右
	int32_t lane=0;   
	//何分かの値
	size_t noteLength = 0u;
	//ノーツの種類
	int32_t noteSelection = 0u;


	//判定線に到着する時間
	float_t judgementArrivalTime=0.0f;
	//動き出す時間
	float_t startMoveTime = 0.0f;
	// 表示中かどうか
	bool isDisplay=false; 
	//生成時の座標
	Vector2 initialPosition = {};
	//現在の座標
	Vector2 currentPosition = {};

	//ノーツのスプライト
	std::unique_ptr<Elysia::Sprite> noteSprite=nullptr;
	//ロング用のボディのスプライト
	std::unique_ptr<Elysia::Sprite> longBodyNoteSprite = nullptr;

	//判定されたか
	bool isJudged = false;
	//処理終了
	bool isProcessEnd = false;
	//判定
	uint32_t judgement= NoteJudgementSelection::None;
	//
	bool isPlaySE = false;
};



struct NoteJudgementResult {
	int32_t perfect;
	int32_t great;
	int32_t good;
	int32_t miss;
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
	/// ノーツの流れる処理(左)
	/// </summary>
	void FlowProcessLeft();

	/// <summary>
	/// ノーツの流れる処理
	/// </summary>
	void FlowProcessMiddle();

	/// <summary>
	/// ノーツの流れる処理
	/// </summary>
	void FlowProcessRight();



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
	const float_t WAITING_POSITION_Y_ = -5.0f;
	//判定の座標
	const float_t JUDGEMENT_POSITION_Y_ = 650.0f;

	//オフセット
	const float_t START_OFFSET_TIME_ = 1.00f;


	//判定
	//パーフェクト
	const float_t PERFECT_TAP_ = 0.1f;
	//グレート
	const float_t GREAT_TAP_ = 0.25f;
	//グッド
	const float_t GOOD_TAP_ = 0.4f;
	//ミス
	const float_t MISS_TAP_ = 0.5f;

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

	//待ち時間
	float_t waitingTime_ = 0.0f;
	//開始までの時間
	float_t startTime_ = 0.0f;
	//開始
	bool isStart_ = false;

	float_t totalTime_ = 0.0f;
	float_t noteMoveTime_ = 0.0f;
	//std::vector<NoteInstance> noteInstances_;

	std::vector<NoteInstance> leftNoteInstances_;
	std::vector<NoteInstance> middleNoteInstances_;
	std::vector<NoteInstance> rightNoteInstances_;

	float_t leftTouchTime_ = 0.0f;
	float_t middleTouchTime_ = 0.0f;
	float_t rightTouchTime_ = 0.0f;



	bool isHitLeft_ = false;
	bool isHitMiddle_ = false;
	bool isHitRight_ = false;

	bool isHoldLeft_ = false;
	bool isHoldMiddle_ = false;
	bool isHoldRight_ = false;


	bool isHitLongNoteLeft_ = false;

	int32_t bonusScore_ = 0;

	Vector2 longNoteStartPosition_ = {};
	NoteInstance startLongNoteInstance_ = {};

	uint32_t noteRadius_ = 0u;


	//レーンの座標
	std::array<float_t, 3u>laneX_ = {100,200,300};
	//曲のハンドル
	uint32_t musicHandle_ = 0u;
	float_t musicTime_ = 0.0f;
	uint32_t arraiveSEHandle_ = 0u;


private:
	//判定
	NoteJudgementResult leftResult_ = {};
	NoteJudgementResult middleResult_ = {};
	NoteJudgementResult rightResult_ = {};



	//タッチ
	NotePosition touch_ = {};

};