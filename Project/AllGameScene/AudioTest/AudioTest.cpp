#include "AudioTest.h"

#include "Input.h"
#include "SingleCalculation.h"
#include "VectorCalculation.h"
#include "PushBackCalculation.h"
#include "AnimationManager.h"
#include "TextureManager.h"
#include "GameManager.h"
#include "ModelManager.h"
#include "LevelDataManager.h"
#include "GlobalVariables.h"
#include "Audio.h"

AudioTest::AudioTest(){
	//インスタンスの取得
	//入力	
	input_ = Elysia::Input::GetInstance();
	//テクスチャ管理クラス
	textureManager_ = Elysia::TextureManager::GetInstance();
	//モデル管理クラス
	modelManager_ = Elysia::ModelManager::GetInstance();
	//レベルエディタ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
	//グローバル変数クラス
	globalVariables_ = Elysia::GlobalVariables::GetInstance();
	//オーディオ
	audio_ = Elysia::Audio::GetInstance();
}

void AudioTest::Initialize(){
	//ポストエフェクト
	backTexture_ = std::make_unique<Elysia::BackTexture>();
	backTexture_->SetClearColour({.x=0.0f,.y=0.5f,.z=0.5f,.w=1.0f});
	backTexture_->Initialize();


	//読み込み
	bgmHandle_ = audio_->Load("Resources/Audio/Win/WinBGM.wav");
	audio_->Play(bgmHandle_, true);

	//latestSpectrum_.resize();

	
}

void AudioTest::Update(Elysia::GameManager* gameManager){


	gameManager;
}

void AudioTest::DrawObject3D(){

}

void AudioTest::PreDrawPostEffect(){
	backTexture_->PreDraw();
}

void AudioTest::DrawPostEffect(){
	backTexture_->Draw();
}

void AudioTest::DrawSprite(){

}
