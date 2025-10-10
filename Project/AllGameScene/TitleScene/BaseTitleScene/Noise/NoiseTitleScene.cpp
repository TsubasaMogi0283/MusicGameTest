#include "NoiseTitleScene.h"

#include <imgui.h>

#include "Input.h"
#include "TextureManager.h"
#include "LevelDataManager.h"

#include "TitleScene/TitleScene.h"
#include "TitleScene/BaseBackTexture/Night/NightBackTexture.h"
#include "TitleScene/BaseTitleScene/ToGame/ToGameTitleScene.h"

NoiseTitleScene::NoiseTitleScene(){
	//インスタンスの取得
	//テクスチャ管理クラス
	textureManager_ = Elysia::TextureManager::GetInstance();
	//入力クラス
	input_ = Elysia::Input::GetInstance();
	//レベルエディタ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
	//オーディオ
	audio_ = Elysia::Audio::GetInstance();
}

void NoiseTitleScene::Initialize(){
	//ロゴの読み込み
	uint32_t logoTextureHandle = textureManager_->Load("Resources/Sprite/Title/TitleChanged.png");

	//ロゴスプライトのの生成
	logoSprite_.reset(Elysia::Sprite::Create(logoTextureHandle, SPRITE_INITIAL_POSITION_));

	//平行光源の初期化
	directionalLight_.color = { .x = 1.0f,.y = 1.0f,.z = 1.0f,.w = 1.0f };
	directionalLight_.direction = { .x = 0.91f,.y = -1.0f,.z = 0.0f };
	directionalLight_.intensity = 0.05f;
	
	//読み込み
	noiseHandle_ = audio_->Load("Resources/Audio/Title/Noise.wav");
	//再生
	audio_->Play(noiseHandle_, true);
	//ボリュームの設定
	noiseVolume_ = 0.0f;
	
}

void NoiseTitleScene::Update(TitleScene* titleScene){
	//ボリュームの設定
	audio_->ChangeVolume(noiseHandle_, noiseVolume_);

	//時間の加算
	randomEffectTime_ += DELTA_TIME;

	//通常はfalse
	//指定時間内に入ったらtrue
	titleScene->SetIsDisplayRandomEffect(false);
	logoSprite_->SetInvisible(false);
	for (uint32_t i = 0u; i < DISPLAY_LENGTH_QUANTITY_; ++i) {
		if (randomEffectTime_ > RANDOM_EFFECT_DISPLAY_START_TIME[i] &&
			randomEffectTime_ <= RANDOM_EFFECT_DISPLAY_START_TIME[i] + RANDOM_EFFECT_DISPLAY_LENGTH[i]) {
			//ランダムエフェクトの表示
			titleScene->SetIsDisplayRandomEffect(true);
			//一旦非表示
			logoSprite_->SetInvisible(true);
			//聞こえるようにする
			noiseVolume_ = PLAY_NOISE_AUDIO_VALUME_;
			break;
		}
		else {
			//聞こえなくする
			noiseVolume_ = 0.0f;
		}

		//1回目
		if (i == FIRST_EFFECT) {
			//環境音を止める
			audio_->Stop(titleScene->GetEnviromentAudioHandle());
			//夜へ遷移
			titleScene->ChangeBackTexture(std::move(std::make_unique<NightBackTexture>()));
		}
		//2回目
		else if (i == SECOND_EFFECT) {
			//ランダムの終了
			if (randomEffectTime_ > RANDOM_EFFECT_DISPLAY_START_TIME[SECOND_EFFECT] + RANDOM_EFFECT_DISPLAY_LENGTH[SECOND_EFFECT]) {
				//ロゴの非表示
				logoSprite_->SetInvisible(true);
				//表示の終わりを示す
				isEndDisplay_ = true;
			}
		}
	}

	//表示終了
	if (isEndDisplay_==true){
		audio_->Stop(noiseHandle_);

		titleScene->ChangeDetailScene(std::make_unique<ToGameTitleScene>());
		return;
	}
#ifdef _DEBUG
	//ImGui表示用
	DisplayImGui();
#endif // _DEBUG


}


void NoiseTitleScene::DrawSprite(){
	//ロゴ
	logoSprite_->Draw();
}

void NoiseTitleScene::DisplayImGui(){
	ImGui::Begin("ノイズ(タイトル)");
	ImGui::InputFloat("ランダムエフェクトの時間", &randomEffectTime_);

	if (ImGui::TreeNode("平行光源") == true) {
		ImGui::SliderFloat("強さ",&directionalLight_.intensity,0.0f,10.0f);
		ImGui::SliderFloat3("方向", &directionalLight_.direction.x, -1.0f, 1.0f);
		ImGui::TreePop();
	}

	ImGui::End();
}
