#pragma once
/**
 * @file PlayerCollisionToStageObject.h
 * @brief ステージオブジェクトに対するCollisionクラス
 * @author 茂木翼
 */

#include <memory>

#include "BasePlayerCollision.h"
#include "WorldTransform.h"
#include "Model.h"


/// <summary>
/// ステージオブジェクトに対するコリジョンクラス
/// </summary>
class PlayerCollisionToStageObject :public BasePlayerCollision {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerCollisionToStageObject() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;


	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="material"></param>
	/// <param name="spotLight"></param>
	void Draw(const Camera& camera, const Material& material, const  SpotLight& spotLight)override;


	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerCollisionToStageObject() = default;


private:
	//モデル
	std::unique_ptr<Elysia::Model> model_ = nullptr;
	//ワールドトランスフォーム
	WorldTransform worldTransform_ = {};


};

