#pragma once
/**
 * @file IGameScene.h
 * @brief ゲームシーンのインターフェイス
 * @author 茂木翼
 */



/// <summary>
/// ElysiaEngine(前方宣言)
/// </summary>
namespace Elysia {
	/// <summary>
	/// ゲーム管理クラス
	/// </summary>
	class GameManager;

	/// <summary>
	/// ゲームシーンのインターフェイス
	/// </summary>
	class IGameScene {
	public:
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~IGameScene() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		virtual void Initialize() = 0;

		/// <summary>
		/// 更新
		/// </summary>
		/// <param name="gamaManager"></param>
		virtual void Update(Elysia::GameManager* gamaManager) = 0;


		/// <summary>
		/// Object3Dの描画
		/// </summary>
		virtual void DrawObject3D() = 0;

		/// <summary>
		/// スプライトの描画
		/// </summary>
		virtual void DrawSprite() = 0;

		/// <summary>
		/// ポストエフェクト描画処理前
		/// </summary>
		virtual void PreDrawPostEffect() = 0;

		/// <summary>
		/// ポストエフェクト描画
		/// </summary>
		virtual void DrawPostEffect() = 0;


	};
}

