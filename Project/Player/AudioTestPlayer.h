#pragma once
#include "WorldTransform.h"
#include "Material.h"
#include "Model.h"
#include "PlayerCollisionToAudioObject.h"
#include "PlayerCollisionToStageObject.h"

struct DirectionalLight;
struct Camera;

enum TestPlayerMoveCondition {
	//動かない
	NoneTestPlayerMove,
	//動く
	OnTestPlayerMove,
};

class AudioTestPlayer{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	AudioTestPlayer() = default;


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="directionalLight"></param>
	void Draw(const Camera& camera, const DirectionalLight& directionalLight);


	/// <summary>
	/// デストラクタ
	/// </summary>
	~AudioTestPlayer() = default;

public:
	/// <summary>
	/// 方向を設定
	/// </summary>
	/// <param name="direction"></param>
	void SetDirection(const Vector3& direction) {
		this->direction_ = direction;
	}

	
	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition()const {
		return worldTransform_.GetWorldPosition();
	}


	/// <summary>
	/// 方向を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetMoveDirection()const {
		return direction_;
	}

	/// <summary>
	/// コライダー(オーディオ用)取得
	/// </summary>
	/// <returns></returns>
	PlayerCollisionToAudioObject* GetCollosionToAudioObject() const{
		return collosionToAudioObject_.get();
	}






	/// <summary>
	/// 動きの状態の設定
	/// </summary>
	/// <param name="condition"></param>
	void SetMoveCondition(const uint32_t condition) {
		this->moveCondition_ = condition;
	}


private:
	//ワールドトランスフォーム
	WorldTransform worldTransform_ = {};

	//マテリアル
	Material material_ = {};

	//モデル
	std::unique_ptr<Elysia::Model>model_ = nullptr;


	//動きの状態
	uint32_t moveCondition_ = 0u;

	//方向
	Vector3 direction_ = {};

	//コライダー
	//オーディオ用
	std::unique_ptr<PlayerCollisionToAudioObject>collosionToAudioObject_ = nullptr;
};