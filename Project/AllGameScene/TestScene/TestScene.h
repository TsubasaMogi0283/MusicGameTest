#pragma once

#include <memory>
#include <array>

#include "IGameScene.h"
#include "BackTexture.h"
#include "Model.h"
#include "Particle3D.h"
#include "WorldTransform.h"
#include "AABB.h"
#include "Camera.h"
#include "Material.h"
#include "DirectionalLight.h"

/// <summary>
/// EllysiaEngine(前方宣言)
/// </summary>
namespace Elysia {
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
}


class TestScene : public Elysia::IGameScene{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	TestScene();

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
	~TestScene() = default;

private:
	//入力
	Elysia::Input* input_ = nullptr;
	//モデル管理クラス
	Elysia::ModelManager* modelManager_ = nullptr;
	//レベルエディタ
	Elysia::LevelDataManager* levelDataManager_ = nullptr;
	//ハンドル
	uint32_t levelHandle_ = 0u;

private:
	//背景
	std::unique_ptr<Elysia::BackTexture>backTexture_ = nullptr;

	//仮プレイヤー
	std::unique_ptr<Elysia::Model>playerModel_ = nullptr;
	WorldTransform playerWorldTransform_ = {};
	const float_t SIZE = 1.0f;
	const Vector3 CUBE_SIZE = { .x = SIZE ,.y = SIZE ,.z = SIZE };
	AABB playerAABB_ = {};
	Vector3 playerCenterPosition_ = {};

	//パーティクル
	std::unique_ptr<Elysia::Particle3D>deadParticle_ = nullptr;
	std::unique_ptr<Elysia::Particle3D>particle2_ = nullptr;


	//四隅
	static const uint32_t COUNER_QUANTITY_ = 4u;
	std::array<std::unique_ptr<Elysia::Model>, COUNER_QUANTITY_>playerCounerModel_ = {};
	std::array<WorldTransform, COUNER_QUANTITY_>playerCounerWorldTransform_ = {};

	//カメラ
	Camera camera_ = {};
	//平行光源
	DirectionalLight directionalLight_ = {};
	//マテリアル
	Material playerMaterial_ = {};

};