#include "LevelEditorSample.h"
#include <imgui.h>
#include <numbers>

#include "Input.h"
#include "GameScene/GameScene.h"
#include "GameManager.h"
#include "ModelManager.h"
#include "AnimationManager.h"
#include "TextureManager.h"
#include "VectorCalculation.h"
#include "SingleCalculation.h"


LevelEditorSample::LevelEditorSample(){
	//レベルエディタ
	levelEditor_ = LevelDataManager::GetInstance();
	//インプット
	input_ = Input::GetInstance();
}

void LevelEditorSample::Initialize(){

	//平行光源の初期化
	directionalLight_.Initialize();
	
	//マテリアルの初期化
	material_.Initialize();
	material_.lightingKinds_ = Directional;

	//カメラ
	camera_.Initialize();
	camera_.translate = {.x = 0.0f,.y = 2.0f,.z = -30.0f };

	//ポストエフェクト
	back_ = std::make_unique<BackText>();
	//初期化
	back_->Initialize();

	//オーディオ確認用のプレイヤー
	player_ = std::make_unique<AudioTestPlayer>();
	//初期化
	player_->Initialize();


	//読み込み
	levelHandle_ = levelEditor_->Load("Test/AudioAreaTest.json");

	
	//audio_->PlayMP3(audioHandleMP3_, true);
	//デバッグ用のモデル
	uint32_t debugModelHandle = ModelManager::GetInstance()->LoadModelFile("Resources/Model/Sample/Sphere","Sphere.obj");

	//パーティクルの設定
	particle3D_.reset(Particle3D::Create(debugModelHandle,ParticleMoveType::NormalRelease));
	particle3D_->SetScale({.x= 10.0f,.y= 10.0f,.z= 10.0f });
	particle3D_->SetTranslate({.x= 0.0f,.y= 0.0f,.z= 0.0f });
	particle3D_->SetIsReleaseOnceMode(true);
	particle3D_->SetIsToTransparent(true);

	//コリジョン管理クラス
	collisionManager_ = std::make_unique<CollisionManager>();
}

void LevelEditorSample::Update(GameManager* gameManager){
	//リストのクリア
	collisionManager_->ClearList();

#ifdef _DEBUG
	ImGui::Begin("Camera"); 
	ImGui::SliderFloat3("Position", &camera_.translate.x, -50.0f, 50.0f);
	ImGui::SliderFloat3("Rotate", &camera_.rotate.x , -3.0f, 3.0f);
	ImGui::End();

	ImGui::Begin("LevelEditor");
	ImGui::Text("Rキーで指定した再読み込みするよ");
	ImGui::End();


	//Gキーで再読み込み
	if (Input::GetInstance()->IsTriggerKey(DIK_R) == true) {
		levelEditor_->Reload(levelHandle_);

	}
	

#endif // _DEBUG

	gameManager;





	//移動
	//なにも
	playerDirection_ = {};
	//右
	if (input_->IsPushKey(DIK_D) == true) {
		playerDirection_.x += 1.0f;
	}
	//左
	else if (input_->IsPushKey(DIK_A) == true) {
		playerDirection_.x -= 1.0f;
	}
	//上
	else if (input_->IsPushKey(DIK_W) == true) {
		playerDirection_.z += 1.0f;
	}
	//下
	else if (input_->IsPushKey(DIK_S) == true) {
		playerDirection_.z -= 1.0f;
	}


	//プレイヤーの更新
	player_->SetDirection(playerDirection_);
	player_->Update();

	//プレイヤーのコライダーを登録
	collisionManager_->RegisterList(player_->GetCollosionToAudioObject());


	//レベルエディタで使うリスナーの設定
	Listener listener = {
		.position = player_->GetWorldPosition(),
		.move = player_->GetDirection(),
	};
	levelEditor_->SetListener(levelHandle_, listener);

	//レベルエディタの更新
	levelEditor_->Update(levelHandle_);

	//マテリアルの更新
	material_.Update();
	
	//平行光源の更新
	directionalLight_.Update();

	//レベルエディタにあるコライダーを登録
	std::vector<IObjectForLevelEditorCollider*> colliders = levelEditor_->GetCollider(levelHandle_);
	for (auto it = colliders.begin(); it != colliders.end(); ++it) {
		collisionManager_->RegisterList(*it);
	}

	//プレイヤーのAABB
	AABB playerAABB = player_->GetAABB();
	//プレイヤーの方向
	Vector3 playerDirection = player_->GetDirection();

	//ステージオブジェクトとの当たり判定(改)
	//座標が取れなかったのでこっちでやる
	//ObjectManagerでやっていたものをこっちに引っ越す
	//上手くいったらGameSceneで実装する
	std::vector<Vector3> positions = levelEditor_->GetStageObjectPositions(levelHandle_);
	std::vector<AABB> aabbs = levelEditor_->GetStageObjectAABBs(levelHandle_);
	for (size_t i = 0; i < positions.size() && i < aabbs.size(); ++i) {


		//オブジェクトとの差分ベクトル
		Vector3 objectAndPlayerDifference = VectorCalculation::Subtract(positions[i], player_->GetWorldPosition());

		//オブジェクトとプレイヤーの距離
		Vector3 normalizedDemoAndPlayer = VectorCalculation::Normalize(objectAndPlayerDifference);

		//内積
		//これが無いと接触したまま動けなくなってしまうので入れる
		float dot = SingleCalculation::Dot(playerDirection, normalizedDemoAndPlayer);
		const float DOT_OFFSET = 0.7f;


		//衝突判定
		//Y成分はいらない
		if ((playerAABB.min.x <= aabbs[i].max.x && playerAABB.max.x >= aabbs[i].min.x) &&
			(playerAABB.min.z <= aabbs[i].max.z && playerAABB.max.z >= aabbs[i].min.z) &&
			(dot > DOT_OFFSET)) {
			uint32_t newCondition = PlayerMoveCondition::NonePlayerMove;
			player_->SetMoveCondition(newCondition);

			//当たったらループを抜ける
			break;
		}
		else{
			//当たっていない
			uint32_t newCondition = PlayerMoveCondition::OnPlayerMove;
			player_->SetMoveCondition(newCondition);

		}

	}


	//衝突判定の計算
	collisionManager_->CheckAllCollision();

	
	//カメラの更新
	//高さの補正も足す
	const Vector3 OFFSET = { .x = 0.0f,.y = 5.0f,.z = -20.0f };
	Vector3 playerViewPoint = VectorCalculation::Add(player_->GetWorldPosition(), OFFSET);
	camera_.rotate.x = 0.26f;
	camera_.translate = playerViewPoint;
	camera_.Update();

}


void LevelEditorSample::DrawObject3D(){
	//プレイヤー
	player_->Draw(camera_, directionalLight_);

	//レベルエディタ  
	levelEditor_->Draw(levelHandle_,camera_, material_, directionalLight_);
	
	//パーティクル
	particle3D_->Draw(camera_, material_, directionalLight_);
}

void LevelEditorSample::PreDrawPostEffectFirst(){
	back_->PreDraw();
}

void LevelEditorSample::DrawPostEffect(){
	back_->Draw();
}

void LevelEditorSample::DrawSprite(){


}

LevelEditorSample::~LevelEditorSample(){
}
