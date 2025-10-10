#pragma once

/**
 * @file IAbstractSceneFactory.h
 * @brief シーン工場インターフェイス
 * @author 茂木翼
 */

#include <string>
#include <memory>

#include "IGameScene.h"




/// <summary>
/// シーン工場
/// </summary>
class IAbstractSceneFactory{
public:

	/// <summary>
	/// シーン生成
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	/// <returns>シーン</returns>
	virtual std::unique_ptr<Elysia::IGameScene> CreateScene(const std::string& sceneName) = 0;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IAbstractSceneFactory() = default;

};

