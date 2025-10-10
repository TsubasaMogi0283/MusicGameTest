#pragma once
/**
 * @file GameManager.h
 * @brief ゲーム管理クラス
 * @author 茂木翼
 */

#include <cstdint>
#include <memory>

#include "IGameScene.h"
#include "IAbstractSceneFactory.h"


/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {

	/// <summary>
	/// ゲーム管理クラス
	/// </summary>
	class GameManager {
	public:

		/// <summary>
		/// コンストラクタ
		/// </summary>
		GameManager() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// シーン遷移
		/// </summary>
		/// <param name="sceneName"></param>
		void ChangeScene(const std::string& sceneName);

		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		/// <summary>
		/// 描画(3Dオブジェクト)
		/// </summary>
		void DrawObject3D();

		/// <summary>
		/// 描画(スプライト)
		/// </summary>
		void DrawSprite();

		/// <summary>
		/// ポストエフェクト描画前処理
		/// </summary>
		void PreDrawPostEffect();

		/// <summary>
		/// 描画(ポストエフェクト)
		/// </summary>
		void DrawPostEffect();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~GameManager() = default;


	private:
		//シーンファクトリー
		std::unique_ptr<IAbstractSceneFactory> abstractSceneFactory_ = nullptr;

		//現在入っているシーン
		std::unique_ptr<IGameScene> currentGamaScene_ = nullptr;
		std::string  preSceneName_ = "";
		std::string  currentSceneName_ = "";

		//現在のシーンの数
		uint32_t currentSceneNumber_ = 0;
	};

}