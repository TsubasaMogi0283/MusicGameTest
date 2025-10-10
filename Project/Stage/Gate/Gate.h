#pragma once
#include <memory>

#include "Model.h"
#include "Vector3.h"
#include "WorldTransform.h"
#include "Material.h"

/// <summary>
/// カメラ
/// </summary>
struct Camera;

/// <summary>
/// スポットライト
/// </summary>
struct SpotLight;


/// <summary>
/// ゲート
/// </summary>
class Gate{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Gate() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelHandle"></param>
	void Initialize(const uint32_t& modelHandle);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="spotLight"></param>
	void Draw(Camera& camera, SpotLight& spotLight);

	/// <summary>
	/// デストラク
	/// </summary>
	~Gate()=default;


public:
	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPosition()const {
		return worldTransform_.GetWorldPosition();
	};

	/// <summary>
	/// ゲートの付近にいたらtrue
	/// </summary>
	/// <param name="playerPosition"></param>
	/// <returns></returns>
	bool isCollision(const Vector3& playerPosition);

private:
	//モデル
	std::unique_ptr<Elysia::Model> model_ = nullptr;
	//ワールドトランスフォーム
	WorldTransform worldTransform_ = {};
	//マテリアル
	Material material_ = {};

	float width_ = 0.0f;
	float distance_ = 0.0f;

};

