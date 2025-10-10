#pragma once

/**
 * @file AudioObjectForLevelEditor.h
 * @brief オーディオ用のオブジェクトクラス
 * @author 茂木翼
 */



#include <memory>
#include <cstdint>

#include "BaseObjectForLevelEditor.h"
#include "AudioObjectForLevelEditorCollider.h"
#include "Audio.h"

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
/// 平行光源
/// </summary>
struct DirectionalLight;

/// <summary>
/// 点光源
/// </summary>
struct PointLight;

/// <summary>
/// スポットライト
/// </summary>
struct SpotLight;

#pragma endregion


/// <summary>
/// オーディオオブジェクトのタイプ
/// </summary>
enum AudioObjectType {
	BGMType,
	ActionType,
};

/// <summary>
/// オーディオオブジェクトのデータ
/// </summary>
struct AudioDataForLevelEditor {

	//ファイル名
	std::string fileName;

	//種類(BGMかSE)
	std::string type;

	//ハンドル
	uint32_t handle;

	//エリア上かどうか
	bool isOnArea;

	//ループ
	bool isLoop;
};

/// <summary>
/// オーディオ用のオブジェクト
/// </summary>
class AudioObjectForLevelEditor :public BaseObjectForLevelEditor {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	AudioObjectForLevelEditor();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelhandle"></param>
	/// <param name="transform"></param>
	void Initialize(const uint32_t& modelhandle, const Transform& transform) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private:
	/// <summary>
	/// BGM
	/// </summary>
	void BackGround();

	/// <summary>
	/// アクションSE
	/// </summary>
	void Action();

	

public:
	/// <summary>
	/// オーディオのハンドルを設定
	/// 初期化より先でやってね
	/// </summary>
	inline void SetLevelDataAudioData(const AudioDataForLevelEditor& levelDataAudioData) {
		this->audioDataForLevelEditor_ = levelDataAudioData;
	}

	
private:
	//オーディオ
	Elysia::Audio* audio_ = nullptr;
	
	//レベルエディタ側で設定した値を持ってくる
	AudioDataForLevelEditor audioDataForLevelEditor_ = {};
	
	int32_t audioType_ = -1;

};