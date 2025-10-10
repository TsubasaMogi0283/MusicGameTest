#include "ToGameTitleScene.h"

#include <imgui.h>

#include "Input.h"
#include "TextureManager.h"
#include "LevelDataManager.h"
#include "TitleScene/TitleScene.h"

ToGameTitleScene::ToGameTitleScene(){

	//インスタンスの取得
	//テクスチャ管理クラス
	textureManager_ = Elysia::TextureManager::GetInstance();
	//入力クラス
	input_ = Elysia::Input::GetInstance();
	//レベルエディタ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
}

void ToGameTitleScene::Initialize(){

	//黒フェードのテクスチャ読み込み
	uint32_t blackTexureHandle = textureManager_->Load("Resources/Sprite/Back/Black.png");

	//黒フェード
	blackFade_.reset(Elysia::Sprite::Create(blackTexureHandle, SPRITE_INITIAL_POSITION_));
	//初期の透明度設定
	blackFade_->SetTransparency(INITIAL_TRANSPARENCY);

	//平行光源の初期化
	directionalLight_.color = { .x = 1.0f,.y = 1.0f,.z = 1.0f,.w = 1.0f };
	directionalLight_.direction = { .x = 0.91f,.y = -1.0f,.z = 0.0f };
	directionalLight_.intensity = 0.05f;
}

void ToGameTitleScene::Update(TitleScene* titleScene){

	//黒フェードの透明度の変更
	blackFadeTransparency_ += INITIAL_TRANSPARENCY;
	blackFade_->SetTransparency(blackFadeTransparency_);

	//完全不透明の時処理が終わる
	if (blackFadeTransparency_ > PERFECT_NO_TRANSPARENT_) {
		titleScene->SetIsEnd();
		return;
	}

#ifdef _DEBUG
	//ImGui表示用
	DisplayImGui();
#endif // _DEBUG

}

void ToGameTitleScene::DrawSprite(){
	//黒フェード
	blackFade_->Draw();
}

void ToGameTitleScene::DisplayImGui(){
	ImGui::Begin("ゲームシーンへ(タイトル)");
	ImGui::InputFloat("フェードの透明度", &blackFadeTransparency_);
	ImGui::End();

}
