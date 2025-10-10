#pragma once
/**
 * @file PlayerCollisionToAudioObject.h
 * @brief オーディオオブジェクトに対してのコライダークラス
 * @author 茂木翼
 */

#include "BasePlayerCollision.h"

/// <summary>
/// オーディオオブジェクトに対してのコライダー
/// </summary>
class PlayerCollisionToAudioObject:public BasePlayerCollision {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerCollisionToAudioObject() = default;

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
	~PlayerCollisionToAudioObject() = default;



};

