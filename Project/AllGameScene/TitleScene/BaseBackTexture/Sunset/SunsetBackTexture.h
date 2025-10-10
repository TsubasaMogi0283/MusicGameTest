#pragma once

/**
 * @file SunsetBackTexture.h
 * @brief 夕暮れの背景
 * @author 茂木翼
 */

#include "TitleScene/BaseBackTexture/BaseTitleBackTexture.h"

/// <summary>
/// 夕暮れの背景
/// </summary>
class SunsetBackTexture : public BaseTitleBackTexture{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SunsetBackTexture() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;


};

