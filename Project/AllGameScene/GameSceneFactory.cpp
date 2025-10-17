#include "GameSceneFactory.h"

#include "GameScene/GameScene.h"
#include "LoseScene/LoseScene.h"
#include "TestScene/TestScene.h"
#include <AudioTest/AudioTest.h>

std::unique_ptr<Elysia::IGameScene> GameSceneFactory::CreateScene(const std::string& sceneName){

    //シーンを生成
    std::unique_ptr<Elysia::IGameScene> newScene = nullptr;

    //ゲーム
    if (sceneName == "Game") {
        newScene = std::make_unique<GameScene>();
    }
    //テスト用
    else if (sceneName == "Test") {
        newScene = std::make_unique<TestScene>();
    }
    else if (sceneName == "AudioTest") {
        newScene = std::make_unique<AudioTest>();
    }

    return newScene;
}
