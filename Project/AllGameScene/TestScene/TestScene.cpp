#include "TestScene.h"

#include <imgui.h>
#include <numbers>

#include "Input.h"
#include "ModelManager.h"
#include "LevelDataManager.h"
#include "VectorCalculation.h"
#include "CollisionCalculation.h"
#include "PushBackCalculation.h"

TestScene::TestScene(){
	//インスタンスの取得	
	//入力
	input_ = Elysia::Input::GetInstance();
	//モデル管理クラス
	modelManager_ = Elysia::ModelManager::GetInstance();
	//レベルエディタ管理クラス
	levelDataManager_ = Elysia::LevelDataManager::GetInstance();
}

void TestScene::Initialize(){

	//ハンドルの取得
	levelHandle_ = levelDataManager_->Load("CollisionTest/CollisionTest.json");

	//仮プレイヤー
	uint32_t playerModelHandle = modelManager_->Load("Resources/Model/Sample/Cube", "cube.obj");
	playerModel_.reset(Elysia::Model::Create(playerModelHandle));
	playerWorldTransform_.Initialize();
	playerWorldTransform_.scale = { .x = 0.5f,.y = 1.0f,.z=2.0f };
	playerCenterPosition_ = { .x = 0.0f,.y = 0.0f,.z = -5.0f };
	playerWorldTransform_.translate = playerCenterPosition_;


	//球モデル
	uint32_t sphereModelHandle = modelManager_->Load("Resources/Model/Sample/Sphere", "Sphere.obj");
	//四隅
	for (uint32_t i = 0; i < COUNER_QUANTITY_; ++i) {
		playerCounerModel_[i].reset(Elysia::Model::Create(sphereModelHandle));
		playerCounerWorldTransform_[i].Initialize();

	}
	
	//吸収パーティクル
	deadParticle_ = std::move(Elysia::Particle3D::Create(ParticleMoveType::Absorb));
	deadParticle_->SetIsReleaseOnceMode(false);
	deadParticle_->SetIsToTransparent(true);
	deadParticle_->SetCount(5u);
	deadParticle_->SetFrequency(1.0f);


	particle2_ = std::move(Elysia::Particle3D::Create(ParticleMoveType::NormalRelease));
	particle2_->SetIsReleaseOnceMode(false);
	particle2_->SetIsToTransparent(true);
	particle2_->SetCount(10u);
	particle2_->SetFrequency(0.5f);

	

	//カメラ
	camera_.Initialize();
	camera_.rotate.x = std::numbers::pi_v<float>/6.0f;
	camera_.translate = { .x = 0.0f,.y = 21.0f,.z = -40.0f };
	//マテリアルの初期化
	playerMaterial_.Initialize();
	playerMaterial_.lightingKinds = LightingType::DirectionalLighting;
	//平行光源の初期化
	directionalLight_.Initialize();

	//背景
	backTexture_ = std::make_unique<Elysia::BackTexture>();
	backTexture_->Initialize();
}

void TestScene::Update(Elysia::GameManager* gameManager){


	//再読み込み
	if (input_->IsTriggerKey(DIK_R) == true) {
		levelDataManager_->Reload(levelHandle_);
	}

	//レベルエディタの更新
	levelDataManager_->Update(levelHandle_);
	std::vector<AABB> objects = levelDataManager_->GetObjectAABBs(levelHandle_,"Stage");

	const float_t SPEED = 0.1f;
	//方向
	Vector3 direction = {};

	//入力
	if (input_->IsPushKey(DIK_UP) == true) {
		direction.z = SPEED;
	}
	else if (input_->IsPushKey(DIK_DOWN) == true) {
		direction.z = -SPEED;
	}
	else if (input_->IsPushKey(DIK_RIGHT) == true) {
		direction.x = SPEED;
	}
	else if (input_->IsPushKey(DIK_LEFT) == true) {
		direction.x = -SPEED;
	}

	//座標の加算
	playerCenterPosition_= VectorCalculation::Add(playerCenterPosition_, direction);
	//プレイヤーのAABBを計算
	playerAABB_.max = VectorCalculation::Add(playerCenterPosition_, playerWorldTransform_.scale);
	playerAABB_.min = VectorCalculation::Subtract(playerCenterPosition_, playerWorldTransform_.scale);
	

	for (size_t i = 0; i < objects.size(); ++i) {
		PushBackCalculation::FixPosition(playerCenterPosition_,playerAABB_, objects[i]);
	}


	//四隅の球の更新
	for (uint32_t i = 0; i < COUNER_QUANTITY_; ++i) {
		playerCounerWorldTransform_[i].Update();

	}

	//プレイヤー更新
	playerWorldTransform_.translate = playerCenterPosition_;
	playerWorldTransform_.Update();
	camera_.Update();
	directionalLight_.Update();
	playerMaterial_.Update();

	//吸収座標の設定
	deadParticle_->SetAbsorbPosition(playerWorldTransform_.GetWorldPosition());


	gameManager;

#ifdef _DEBUG

	ImGui::Begin("仮プレイヤー");
	ImGui::InputFloat3("中心座標", &playerCenterPosition_.x);
	ImGui::SliderFloat3("スケール", &playerWorldTransform_.scale.x, 0.0f, 2.0f);
	ImGui::End();

	ImGui::Begin("テストシーンカメラ");
	ImGui::SliderFloat3("回転", &camera_.rotate.x, -3.0f, 3.0f);
	ImGui::SliderFloat3("座標", &camera_.translate.x, -30.0f, 30.0f);
	ImGui::End();
#endif // _DEBUG


}

void TestScene::DrawObject3D(){
	//レベルエディタ  
	//levelDataManager_->Draw(levelHandle_, camera_, directionalLight_);
	//仮プレイヤー
	//playerModel_->Draw(playerWorldTransform_,camera_, playerMaterial_, directionalLight_);

	
	//パーティクル
	deadParticle_->Draw(camera_, playerMaterial_, directionalLight_);
	particle2_->Draw(camera_, playerMaterial_, directionalLight_);

}

void TestScene::PreDrawPostEffect(){
	//描画前処理
	backTexture_->PreDraw();
}

void TestScene::DrawPostEffect(){
	//描画前処理
	backTexture_->Draw();
}

void TestScene::DrawSprite(){

}


