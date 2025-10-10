#pragma once

/**
 * @file BaseTitleBackTexture.h
 * @brief 背景テクスチャの基底クラス
 * @author 茂木翼
 */

#include <memory>

#include "BackTexture.h"

/// <summary>
/// 背景テクスチャの基底クラス
/// </summary>
class BaseTitleBackTexture{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 描画前処理
	/// </summary>
	virtual void PreDraw();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseTitleBackTexture() = default;


protected:
	//背景
	std::unique_ptr<Elysia::BackTexture>backTexture_ = nullptr;


};

