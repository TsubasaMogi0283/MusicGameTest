#pragma once
/**
 * @file Key.h
 * @brief 鍵のクラス
 * @author 茂木翼
 */

#include <memory>

#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include "Material.h"

#pragma region 前方宣言

/// <summary>
/// カメラ
/// </summary>
struct Camera;

#pragma endregion


/// <summary>
/// 鍵
/// </summary>
class Key{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Key() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelhandle">ハンドル</param>
	/// <param name="position">座標</param>
	void Initialize(const uint32_t& modelhandle,const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画(モデル)
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="spotLight">スポットライト</param>
	void DrawModel(const Camera& camera,const SpotLight& spotLight);

	/// <summary>
	/// 描画(スプライト)
	/// </summary>
	void DrawSprite();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Key() = default;

private:
	/// <summary>
	/// 上昇し回転
	/// </summary>
	void RiseAndRotate();

	/// <summary>
	/// スプライトの動き
	/// </summary>
	void SpriteMove();

	/// <summary>
	/// ImGuiの表示
	/// </summary>
	void DisplayImGui();

public:
	/// <summary>
	/// 鍵がプレイヤーに取得される
	/// </summary>
	void PickedUp() {
		//拾う
		isPickUp_ = true;
	};

public:

	/// <summary>
	/// ワールド座標を取得する
	/// </summary>
	/// <returns>ワールド座標</returns>
	Vector3 GetWorldPosition()const {
		return worldTransform_.GetWorldPosition();
	}

	/// <summary>
	/// 半径の取得
	/// </summary>
	/// <returns>半径</returns>
	float GetRadius()const {
		return radius_;
	}
	/// <summary>
	/// 取得したかどうか
	/// </summary>
	/// <returns></returns>
	bool GetIsPickUp()const {
		return isPickUp_;
	}

	/// <summary>
	/// 取得可能かどうか
	/// </summary>
	/// <returns></returns>
	bool GetIsAbleToPickUp()const {
		return isAbleToPickUp_;
	}

	/// <summary>
	/// 取得可能かどうかの設定
	/// </summary>
	/// <param name="isAbleToPickUp"></param>
	void SetisAbleToPickUp(const bool& isAbleToPickUp) {
		this->isAbleToPickUp_ = isAbleToPickUp;
	}

	/// <summary>
	/// 取得前かどうかの設定
	/// </summary>
	/// <param name="isPrePickUp">取得前</param>
	inline void SetIsPrePickUp(const bool& isPrePickUp) {
		this->isPrePickUp_ = isPrePickUp;
	}

	/// <summary>
	/// 終点の座標を設定
	/// </summary>
	/// <param name="position">座標</param>
	inline void SetEndPosition(const Vector2& position) {
		this->spriteEndPosition_ = position;
	}

	/// <summary>
	/// 取得可能かどうか
	/// </summary>
	/// <returns>取得可能</returns>
	inline bool GetIsPrePickUp()const {
		return isPrePickUp_;
	}

	/// <summary>
	/// 消える
	/// </summary>
	/// <returns>消えるか</returns>
	inline bool GetIsDelete()const {
		return isDelete_;
	}

private:
	
	//上下移動の大きさ
	const float_t MOVE_AMOUNT_ = 0.1f;
	//縮小
	const float_t SCALE_DOWN_AMOUNT_ = 0.05f;
	//回転の大きさ
	const float_t ROTATE_AMOUNT_ = 0.1f;
	//始点
	const Vector2 SPRITE_STRAT_POSITION_ = { .x = 680,.y = 600.0f };
	//縮小
	const float_t SCALE_DOWM_VALUE_ = -0.05f;
	//高さ
	const float_t HEIGHT_ = 0.5f;
private:
	//モデル
	std::unique_ptr<Elysia::Model> model_ = nullptr;
	//取得時回転しUIの所にいく
	std::unique_ptr<Elysia::Sprite> sprite_ = nullptr;
	//ワールドトランスフォーム
	WorldTransform worldTransform_ = {};
	//マテリアル
	Material material_ = {};
	
	//取得できるかどうか
	bool isAbleToPickUp_ = true;
	//取得する前
	bool isPrePickUp_ = false;
	//取得されたか
	bool isPickUp_ = false;
	//半径
	float_t radius_ = 0.0f;

	//元の座標
	float_t originalPositionY_ = 0.001f;

	//上昇前の座標を記録する
	bool isWritePreUpPosition_ = false;
	Vector3 preUpPosition_ = {};

	//sinの動きにしたいのでthetaを作る
	float_t heightTheta_ = 0.0f;

	//回転終わり
	bool isFinishRise_ = false;
	float_t upT_ = 0.0f;
	//終点
	Vector2 spriteEndPosition_ = { };

	//スプライトが動くかどうか
	bool isSpriteMove_ = false;
	//スケールを線形補間で実装する
	float_t scaleTheta_ = 0.0f;
	float_t scaleT_ = 0.0f;
	//スプライトの回転
	float_t spriteRotate_ = 0.0f;

	//消える
	bool isDelete_ = false;
};

