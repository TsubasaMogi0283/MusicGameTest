#pragma once

/**
 * @file TitleRailCamera.h
 * @brief レールカメラ(タイトル用)のクラス
 * @author 茂木翼
 */

#include <vector>
#include <sstream>

#include "WorldTransform.h"
#include "Camera.h"

/// <summary>
/// レールカメラ(タイトル用)
/// </summary>
class TitleRailCamera{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	TitleRailCamera() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="csvPath">CSVファイル</param>
	void Initialize(const std::string& csvPath);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleRailCamera() = default;


private:
	/// <summary>
	/// ImGui表示用関数
	/// </summary>
	void DisplayImGui();

	/// <summary>
	/// スプライン曲線を使ったループ
	/// </summary>
	/// <param name="points">座標</param>
	/// <param name="t">割合</param>
	/// <returns>座標</returns>
	Vector3 CatmullRomPositionLoop(const std::vector<Vector3>& points, const float_t& t);


public:
	/// <summary>
	/// カメラの取得
	/// </summary>
	/// <returns>カメラ</returns>
	inline Camera GetCamera() const{
		return camera_;
	};

	/// <summary>
	/// 方向の取得
	/// </summary>
	/// <returns>方向</returns>
	inline Vector3 GetMoveDirection()const {
		return direction_;
	}


private:
	//ワールドトランスフォーム
	WorldTransform worldTransform_ = {};
	//カメラ
	Camera camera_ = {};

	//方向
	Vector3 direction_ = {};

	//線形補間用
	float_t cameraT_ = 0.0f;
	//tの増える値
	const float_t increaseTValue_ = 0.0001f;

	//制御点
	std::vector<Vector3>points_ = {};
	//生成の文字列を入れる
	std::stringstream positionsFromCSV_;


};

