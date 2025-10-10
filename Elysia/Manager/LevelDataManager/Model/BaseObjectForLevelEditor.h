#pragma once

/**
 * @file BaseObjectForLevelEditor.h
 * @brief レベルエディタ用のモデル(基底クラス)
 * @author 茂木翼
 */

#include "Model.h"
#include "WorldTransform.h"
#include "Vector3.h"
#include "Material.h"
#include "AABB.h"
#include "Transform.h"

/// <summary>
/// オブジェクトの種類
/// </summary>
enum LevelEditorObjectType {
	//ステージ
	StageObject,
	//オーディオ
	AudioObject,
};


/// <summary>
/// レベルエディタ用のモデル(基底クラス)
/// </summary>
class BaseObjectForLevelEditor {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelhandle"></param>
	/// <param name="position"></param>
	virtual void Initialize(const uint32_t& modelhandle,const Transform& position)=0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	virtual void Draw(const Camera& camera);

	/// <summary>
	/// 描画(平行光源)
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="material"></param>
	/// <param name="directionalLight"></param>
	virtual void Draw(const Camera& camera, const DirectionalLight& directionalLight);

	/// <summary>
	/// 描画(点光源)
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="material"></param>
	/// <param name="pointLight"></param>
	virtual void Draw(const Camera& camera, const PointLight& pointLight);

	/// <summary>
	/// 描画(スポットライト)
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="material"></param>
	/// <param name="spotLight"></param>
	virtual void Draw(const Camera& camera, const SpotLight& spotLight);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseObjectForLevelEditor() = default;


public:
	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns></returns>
	virtual Vector3 GetWorldPosition()const {
		return worldTransform_.GetWorldPosition();
	};

	/// <summary>
	/// AABBの取得
	/// </summary>
	/// <returns></returns>
	virtual AABB GetAABB() const{
		return aabb_;
	};


public:
	/// <summary>
	/// 衝突したかどうかの設定
	/// Colliderから持ってくる
	/// </summary>
	/// <param name="isTouch"></param>
	inline void SetIsTouch(const bool& isTouch) {
		this->isTouch_ = isTouch;
	}

	/// <summary>
	/// リスナーが動いているかどうかの設定
	/// </summary>
	/// <param name="isMove"></param>
	/// <returns></returns>
	inline void SetIsListenerMove(const bool& isMove) {
		isListenerMove_ = isMove;
	}


	/// <summary>
	/// 拡縮
	/// </summary>
	/// <param name="scale"></param>
	inline void SetScale(const Vector3& scale) {
		this->worldTransform_.scale = scale;
	}

	/// <summary>
	/// 回転
	/// </summary>
	/// <param name="rotate"></param>
	inline void SetRotate(const Vector3& rotate) {
		this->worldTransform_.rotate = rotate;
	}

	/// <summary>
	/// 座標の設定
	/// </summary>
	/// <param name="position"></param>
	inline void SetPositione(const Vector3& position) {
		this->worldTransform_.translate = position;
	}

	/// <summary>
	/// 色の設定
	/// </summary>
	/// <param name="color"></param>
	void SetColor(const Vector4& color) {
		this->material_.color = color;
	}

	/// <summary>
	/// 透明度の設定
	/// </summary>
	/// <param name="transparency"></param>
	void SetTransparency(const float& transparency) {
		this->material_.color.w = transparency;
	}


protected:
	//モデル
	std::unique_ptr<Elysia::Model> model_ = nullptr;

	//ワールドトランスフォーム
	WorldTransform worldTransform_ = {};
	//マテリアル
	Material material_ = {};

	//オブジェクトのタイプ
	//LevelEditorObjectTypeから選ぶよ
	uint32_t objectType_ = 0u;

	//基本オブジェクトはAABBにする
	AABB aabb_ = {};

	//衝突
	bool isTouch_ = false;

	//リスナーが動いているかどうか
	bool isListenerMove_=false;
};