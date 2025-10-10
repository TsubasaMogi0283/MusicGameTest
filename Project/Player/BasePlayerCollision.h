#pragma once
#include "Collider.h"

#pragma region 前方宣言
/// <summary>
/// カメラ
/// </summary>
struct Camera;

/// <summary>
/// マテリアル
/// </summary>
struct Material;

/// <summary>
/// スポットライト
/// </summary>
struct SpotLight;


#pragma endregion

/**
 * @file BasePlayerCollision.h
 * @brief プレイヤー用のコリジョン基底クラス
 * @author 茂木翼
 */

/// <summary>
/// プレイヤー用のコリジョン
/// </summary>
class BasePlayerCollision: public Collider{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize()=0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update()=0;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="material">マテリアル</param>
	/// <param name="spotLight">スポットライト</param>
	virtual void Draw(const Camera& camera, const Material& material, const SpotLight& spotLight)=0;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BasePlayerCollision() = default;


public:
	/// <summary>
	/// 接触
	/// </summary>
	void OnCollision()override;

	/// <summary>
	/// 非接触
	/// </summary>
	void OffCollision()override;


	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns>ワールド座標</returns>
	inline Vector3 GetWorldPosition()override {
		return playerPosition_;
	};

public:
	/// <summary>
	/// プレイヤーの座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	virtual inline void SetPlayerPosition(const Vector3& position) {
		this->playerPosition_ = position;
	}

	/// <summary>
	/// 衝突下かどうかのフラグ
	/// </summary>
	/// <returns>フラグ</returns>
	virtual inline bool GetIsTouch()const {
		return isTouch_;
	}

protected:
	//位置
	Vector3 playerPosition_ = {};
	//衝突
	bool isTouch_ = false;

};

