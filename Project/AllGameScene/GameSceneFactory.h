#pragma once

/**
 * @file GameSceneFactory.h
 * @brief ゲームシーンファクトリー
 * @author 茂木翼
 */

#include "IGameScene.h"
#include "IAbstractSceneFactory.h"

/// <summary>
/// ゲームシーンファクトリー
/// </summary>
class GameSceneFactory : public IAbstractSceneFactory {
public:
	/// <summary>
	/// シーンの生成
	/// </summary>
	/// <param name="sceneName">シーンの名前</param>
	/// <returns>生成したシーン</returns>
	std::unique_ptr<Elysia::IGameScene> CreateScene(const std::string& sceneName)override;

};

