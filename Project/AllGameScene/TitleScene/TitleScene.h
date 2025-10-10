#pragma once
/**
 * @file TitleScene.h
 * @brief タイトルクラス
 * @author 茂木翼
 */

#include <memory>
#include <vector>

#include "IGameScene.h"
#include "Sprite.h"
#include "Model.h"
#include "Camera.h"
#include "BackTexture.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "RandomNoise.h"
#include "RailCamera/TitleRailCamera.h"
#include "RandomNoisePostEffect.h"
#include "BaseBackTexture/BaseTitleBackTexture.h"
#include "BaseTitleScene/BaseTitleScene.h"

#pragma region 前方宣言


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
	/// オーディオ
	/// </summary>
	class Audio;
}


#pragma endregion


/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene : public Elysia::IGameScene {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	TitleScene();

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
	/// 3Dオブジェクト
	/// </summary>
	void DrawObject3D()override;

	/// <summary>
	/// ポストエフェクト描画処理前
	/// </summary>
	void PreDrawPostEffect()override;

	/// <summary>
	/// ポストエフェクト描画
	/// </summary>
	void DrawPostEffect()override;


	/// <summary>
	/// スプライト
	/// </summary>
	void DrawSprite()override;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleScene() = default;


private:
	/// <summary>
	/// ImGui用関数
	/// </summary>
	void DisplayImGui();


public:

	/// <summary>
	/// 背景テクスチャの遷移
	/// </summary>
	/// <param name="backTexture">背景テクスチャ(ポストエフェクト)</param>
	void ChangeBackTexture(std::unique_ptr<BaseTitleBackTexture> backTexture);


	/// <summary>
	/// 細かいシーンの遷移
	/// </summary>
	/// <param name="detailTitleScene">細かいタイトルシーン</param>
	void ChangeDetailScene(std::unique_ptr<BaseTitleScene>detailTitleScene);
	
	/// <summary>
	/// ランダムノイズを見せるかどうか
	/// </summary>
	/// <param name="isDisplay">見せるかどうか</param>
	inline void SetIsDisplayRandomEffect(const bool& isDisplay) {
		this->isDisplayRandomEffect_= isDisplay;
	}

	/// <summary>
	/// 処理を終えた
	/// </summary>
	inline void SetIsEnd() {
		isEnd_ = true;
	}

	/// <summary>
	/// 環境音のハンドルを取得
	/// </summary>
	/// <returns></returns>
	uint32_t GetEnviromentAudioHandle()const {
		return enviromentAudioHandle_;
	}


private:
	//テクスチャ管理クラス
	Elysia::TextureManager* textureManager_ = nullptr;
	//入力クラス
	Elysia::Input* input_ = nullptr;
	//レベルエディタ
	Elysia::LevelDataManager* levelDataManager_ = nullptr;
	uint32_t levelHandle_ = 0u;
	//オーディオ
	Elysia::Audio* audio_ = nullptr;

private:

	//カメラ
	Camera camera_ = {};
	//レールカメラ
	std::unique_ptr<TitleRailCamera> titleRailCamera_ = nullptr;
	//平行光源
	DirectionalLight directionalLight_ = {};

	//細かいシーン
	std::unique_ptr<BaseTitleScene>detailTitleScene_ = nullptr;
	//処理を終えたかどうか
	bool isEnd_ = false;

	//背景(ポストエフェクト)
	std::unique_ptr<BaseTitleBackTexture> baseTitleBackTexture_ = nullptr;
	//ランダムノイズ
	std::unique_ptr<Elysia::RandomNoisePostEffect> randomEffect_ = nullptr;
	RandomNoise randomNoise_ = {};
	//見せるかどうか
	bool isDisplayRandomEffect_ = false;

	//環境音のハンドル
	uint32_t enviromentAudioHandle_ = 0u;



};