#include "GameSceneFactory.h"

#include "TitleScene/TitleScene.h"
#include "GameScene/GameScene.h"
#include "WinScene/WinScene.h"
#include "LoseScene/LoseScene.h"
#include "TestScene/TestScene.h"
#include <AudioTest/AudioTest.h>

std::unique_ptr<Elysia::IGameScene> GameSceneFactory::CreateScene(const std::string& sceneName){

    //シーンを生成
    std::unique_ptr<Elysia::IGameScene> newScene = nullptr;

    //タイトル
    if (sceneName == "Title") {
        newScene = std::make_unique<TitleScene>();
    }
    //ゲーム
    else if (sceneName == "Game") {
        newScene = std::make_unique<GameScene>();
    }
    //勝ち
    else if (sceneName == "Win") {
        newScene = std::make_unique<WinScene>();
    }
    //負け
    else if (sceneName == "Lose") {
        newScene = std::make_unique<LoseScene>();
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
