#pragma once
/**
 * @file LoseScene.h
 * @brief 負けシーンのクラス
 * @author 茂木翼
 */
#include <memory>

#include "IGameScene.h"
#include "Sprite.h"
#include "Model.h"
#include "Camera.h"
#include "PointLight.h" 
#include "BackTexture.h"
#include "DissolveEffect.h"
#include "Dissolve.h"
#include "BaseLoseScene/BaseLoseScene.h"




/// <summary>
/// EllysiaEngine(前方宣言)
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
	/// グローバル変数
	/// </summary>
	class GlobalVariables;

	/// <summary>
	/// ゲーム管理クラス
	/// </summary>
	class GameManager;

	/// <summary>
	/// オーディオ
	/// </summary>
	class Audio;
};


/// <summary>
/// 負けシーン
/// </summary>
class LoseScene : public Elysia::IGameScene {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	LoseScene();

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
	/// ポストエフェクト描画処理前
	/// </summary>
	void PreDrawPostEffect()override;

	/// <summary>
	/// ポストエフェクト描画処理前
	/// </summary>
	void DrawPostEffect()override;


	/// <summary>
	/// スプライト描画
	/// </summary>
	void DrawSprite()override;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LoseScene()=default;

public:
	/// <summary>
	/// 敗北の細かいシーンの遷移
	/// </summary>
	/// <param name=""></param>
	void ChangeDetailScene(std::unique_ptr<BaseLoseScene> detailScene);

	/// <summary>
	/// 終わったことを設定する
	/// </summary>
	inline void SetIsEnd() {
		this->isEnd_ = true;
	}

	/// <summary>
	/// ゲームを続けるかどうかの設定
	/// </summary>
	/// <param name="isContinue">フラグ</param>
	inline void SetIsContinue(const bool& isContinue) {
		this->isContinue_ = isContinue;
	}

	/// <summary>
	/// BGMのハンドルを取得
	/// </summary>
	/// <returns></returns>
	inline uint32_t GetBgmHandle()const {
		return bgmHandle_;
	}

	/// <summary>
	/// BGMの音量を設定
	/// </summary>
	inline void SetBgmVolumeDown(const float_t& volume) {
		this->bgmVolume_ = volume;
	}


private:
	/// <summary>
	/// /ImGUiの表示
	/// </summary>
	void DisplayImGui();

	/// <summary>
	/// 調整
	/// </summary>
	void Adjustment();

private:
	//入力クラス
	Elysia::Input* input_ = nullptr;
	//レベルデータ管理クラス
	Elysia::LevelDataManager* levelDataManager_ = nullptr;
	//ハンドル
	uint32_t levelDataHandle_ = 0u;
	//グローバル変数クラス
	Elysia::GlobalVariables* globalVariables_ = nullptr;
	//オーディオ
	Elysia::Audio* audio_ = nullptr;
private:
	//調整項目
	//点光源
	const std::string POINT_LIGHT_NAME_ = "LoseScenePointLight";
	//ディゾルブ
	const std::string DISSOLVE_NAME_ = "LoseSceneDissolve";
	

private:
	//カメラ
	Camera camera_ = {};

	//点光源
	PointLight pointLight_ = {};

	//背景(ポストエフェクト)
	std::unique_ptr<Elysia::BackTexture>backTexture_ = nullptr;
	//ディゾルブ
	std::unique_ptr<Elysia::DissolvePostEffect> dissolveEffect_ = nullptr;
	Dissolve dissolve_ = {};

	//細かいシーン
	std::unique_ptr<BaseLoseScene> detailLoseScene_ = nullptr;
	//処理が終わったかどうか
	bool isEnd_ = false;
	//ゲームを続けるかどうか
	bool isContinue_ = false;

	//敗北シーンのハンドル
	uint32_t bgmHandle_ = 0u;
	//音量
	float_t bgmVolume_ = 0.9f;

};

