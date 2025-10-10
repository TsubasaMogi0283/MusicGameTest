#include "PlayGameScene.h"

#include <imgui.h>
#include <numbers>

#include "Camera.h"
#include "Input.h"
#include "Audio.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "LevelDataManager.h"
#include "VectorCalculation.h"
#include "SingleCalculation.h"
#include "GameScene/GameScene.h"
#include "Easing.h"

PlayGameScene::PlayGameScene(){
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


void PlayGameScene::Initialize(){
	//空の場合停止させる
	assert(player_);
	assert(enemyManager_);
	assert(keyManager_);

	//コリジョン管理クラスの生成
	collisionManager_ = std::make_unique<Elysia::CollisionManager>();

	//操作
	uint32_t operationTextureHandle = textureManager_->Load("Resources/Sprite/Operation/Operation.png");
	//生成
	operationSprite_.reset(Elysia::Sprite::Create(operationTextureHandle, { .x = 20.0f,.y = 0.0f }));
	//出口の画像読み込み
	uint32_t escapeTextureHandle = textureManager_->Load("Resources/Sprite/Escape/EscapeText.png");
	//生成
	escapeTextSprite_.reset(Elysia::Sprite::Create(escapeTextureHandle, INITIAL_SPRITE_POSITION_));
	//最初は非表示にする
	escapeTextSprite_->SetInvisible(true);

	//門の初期回転
	rightGateRotateTheta_ = 0.0f;
	leftGateRotateTheta_ = -std::numbers::pi_v<float_t>;

	//白フェード
	//白画像読み込み
	uint32_t whiteTextureHandle = textureManager_->Load("Resources/Sprite/Back/White.png");
	//生成
	whiteFadeSprite_.reset(Elysia::Sprite::Create(whiteTextureHandle, INITIAL_SPRITE_POSITION_));
	whiteFadeSprite_->SetTransparency(PERFECT_TRANSPARENT_);

	//黒フェード
	//黒画像読み込み
	uint32_t blackTextureHandle = textureManager_->Load("Resources/Sprite/Back/Black.png");
	//生成
	blackFadeSprite_.reset(Elysia::Sprite::Create(blackTextureHandle, INITIAL_SPRITE_POSITION_));
	blackFadeSprite_->SetTransparency(PERFECT_TRANSPARENT_);

	//ゴールに向かえのテキスト
	uint32_t toEscapeTextureHandle = textureManager_->Load("Resources/Sprite/Escape/ToGoal.png");
	toEscapeSprite_.reset(Elysia::Sprite::Create(toEscapeTextureHandle, INITIAL_SPRITE_POSITION_));
	
	//矢印のモデルを読み込む
	uint32_t arrowModelHandle = Elysia::ModelManager::GetInstance()->Load("Resources/Model/Arrow","Arrow.obj");
	//門の中心座標を計算
	Vector3 gateCenterPosition_ = VectorCalculation::Add(levelDataManager_->GetInitialTranslate(levelDataHandle_, gateRightString_), levelDataManager_->GetInitialTranslate(levelDataHandle_, gateLeftString_));
	gateCenterPosition_ = VectorCalculation::Divide(gateCenterPosition_, 2.0f);

	//アシスト用の矢印
	escapeAssistArrow_ = std::make_unique<EscapeAssistArrow>();
	escapeAssistArrow_->SetPlayer(player_);
	escapeAssistArrow_->Initialize(arrowModelHandle,gateCenterPosition_);

	//ドア
	door_ = std::make_unique<Door>();
	door_->SetLevelDataHandle(levelDataHandle_);
	door_->SetPlayer(player_);
	//初期化
	door_->Initialize();

	//骨のポルターガイスト
	poltergeistBone_ = std::make_unique<PoltergeistBone>();
	poltergeistBone_->SetLevelDataHandle(levelDataHandle_);
	poltergeistBone_->SetPlayer(player_);
	poltergeistBone_->Initialize();

	//ゲートのモデルの読み込み
	uint32_t gateModelhandle = Elysia::ModelManager::GetInstance()->Load("Resources/Model/Sample/Gate", "Gate.obj");
	//生成
	gate_ = std::make_unique<Gate>();
	//初期化
	gate_->Initialize(gateModelhandle);
	
	//目
	surpriseEye_ = std::make_unique<SurpriseEye>();
	surpriseEye_->SetPlayer(player_);
	surpriseEye_->SetLevelDataHandle(levelDataHandle_);
	surpriseEye_->Initialize();

	//門を開ける音
	openGateAudioHandle_ = audio_->Load("Resources/Audio/Action/OpenGate.mp3");
	//墓地が閉まる音
	closeGateAudioHandle_= audio_->Load("Resources/Audio/Action/CloseGate.mp3");
	//チャージ
	chargeSEHandle_ = audio_->Load("Resources/Audio/Action/ChargeSE.wav");
	//攻撃
	attackSEHandle_ = audio_->Load("Resources/Audio/Action/Onoma-Flash12-1(Dry).mp3");

	//環境音の音量設定
	enviromentAudioVolume_ = MAX_VOLUME_;

}

void PlayGameScene::Update(GameScene* gameScene){

	//フレーム初めに
	//コリジョンリストのクリア
	collisionManager_->ClearList();
	//ゲートの更新
	gate_->Update();
	//ドアの更新
	door_->Update();
	//目の更新
	surpriseEye_->Update();
	//コントロール可能にする
	player_->SetIsAbleToControll(true);
	//プレイヤーの移動
	PlayerMove();
	//回転
	PlayerRotate();
	//プレイヤーにそれぞれの角度を設定する
	player_->SetTheta(theta_);
	player_->SetPhi(phi_);
	//脱出の仕組み
	EscapeCondition();
	
	//脱出アシスト
	//全て取った時だけ通す
	if (player_->GetHavingKey() == keyManager_->GetMaxKeyQuantity()) {
		escapeAssistArrow_->SetIsOpaque(true);
	}
	//アシスト用の矢印の更新
	escapeAssistArrow_->Update();
	//オブジェクトの当たり判定
	CemeteryProcess();
	//コリジョン管理クラスに登録
	RegisterToCollisionManager();
	//墓場内の鍵を取ったら動き出す
	if (keyManager_->GetIsPickUpKeyInCemetery() == true) {
		poltergeistBone_->SetStart();
		poltergeistBone_->Update();

	}

	//体力が0になったら負け
	//または一発アウトの敵に接触した場合
	if (player_->GetIsAlive() == false || isTouchStrongEnemy_ == true) {
		//コントロールを失う
		player_->SetIsAbleToControll(false);
		//敵の音を止める
		enemyManager_->StopAudio();
		//鍵の音を止める
		keyManager_->StopAudio();

		//フェードの透明度を設定
		fadeTransparency_ += FADE_AMOUNT_;
		blackFadeSprite_->SetTransparency(fadeTransparency_);

		//非表示
		escapeTextSprite_->SetInvisible(true);
		toEscapeSprite_->SetInvisible(true);

		//音量の設定
		//環境音の音量の設定
		enviromentAudioVolume_ -= VOLUME_DECREASE_VALUE_;
		if (enviromentAudioVolume_ <= MIN_VOLUME_) {
			enviromentAudioVolume_ = MIN_VOLUME_;
		}
		

		//処理終了にし負け
		if (fadeTransparency_ >= PERFECT_NONE_TRANSPARENT_) {
			gameScene->SetIsEnd();
			gameScene->SetIsLose();
			return;
		}
		

	}
	//成功
	if (isSucceedEscape_ == true) {
		//開く音を再生
		if (isPlayOpenSE_ == false) {
			Elysia::Audio::GetInstance()->Play(openGateAudioHandle_, false);
			isPlayOpenSE_ = true;
		}
		
		//回転とフェードを線形補間で管理する
		openT_ += OPEN_T_VALUE_;
		float_t newOpenT_ = Easing::EaseInOutQuart(openT_);
		rightGateRotateTheta_ = SingleCalculation::Lerp(0.0f, MAX_OPEN_VALUE_, newOpenT_);
		leftGateRotateTheta_=SingleCalculation::Lerp(-std::numbers::pi_v<float_t>, -MAX_OPEN_VALUE_, newOpenT_);
		whiteFadeSprite_->SetTransparency(newOpenT_);

		//門の回転
		levelDataManager_->SetRotate(levelDataHandle_, gateRightString_, { .x = 0.0f,.y = rightGateRotateTheta_,.z = 0.0f });
		levelDataManager_->SetRotate(levelDataHandle_, gateLeftString_, { .x = 0.0f,.y = -leftGateRotateTheta_,.z = 0.0f });

		//音を止める
		enemyManager_->StopAudio();
		//鍵の音を止める
		keyManager_->StopAudio();

		//非表示
		escapeTextSprite_->SetInvisible(true);
		//振動しないようにする
		player_->SetIsAbleToControll(false);

		//音量の設定
		//環境音の音量の設定
		enviromentAudioVolume_ -= VOLUME_DECREASE_VALUE_;
		if (enviromentAudioVolume_ <= MIN_VOLUME_) {
			enviromentAudioVolume_ = MIN_VOLUME_;
		}

		//処理終了にし勝ち
		if (openT_ >= PERFECT_NONE_TRANSPARENT_) {
			gameScene->SetIsEnd();
			gameScene->SetIsWin();
			return;
		}
		
	}
	
	//環境音の設定
	gameScene->SetEnviromentAudioVolume(enviromentAudioVolume_);

	//当たり判定チェック
	collisionManager_->CheckAllCollision();

#ifdef _DEBUG
	//常に表示
	escapeAssistArrow_->SetIsOpaque(true);
	//ImGui表示用
	DisplayImGui();
#endif // _DEBUG
}

void PlayGameScene::DrawObject3D(const Camera& camera, const SpotLight& spotLight){
	//アシスト用の矢印
	escapeAssistArrow_->Draw(camera, spotLight);
	//ポルターガイスト(破片)
	poltergeistBone_->Draw(camera, spotLight);
}

void PlayGameScene::DrawSprite(){
	//操作
	operationSprite_->Draw();
	//プレイヤー
	player_->DrawSprite();
	//鍵
	keyManager_->DrawSprite();
	//脱出
	escapeTextSprite_->Draw();
	//ドア
	door_->DrawSprite();
	//出口へ向かえ
	if (player_->GetHavingKey() == keyManager_->GetMaxKeyQuantity()) {
		toEscapeSprite_->Draw();
	}
	//敵管理クラス
	enemyManager_->DrawSprite();
	//目の描画
	surpriseEye_->DrawSprite();
	//白フェード
	whiteFadeSprite_->Draw();
	//黒フェード
	blackFadeSprite_->Draw();
}

void PlayGameScene::RegisterToCollisionManager(){

	//懐中電灯に対してのコライダーを登録
	collisionManager_->RegisterList(player_->GetFlashLightCollision());

	//エネミーをコリジョンマネージャーに追加
	//通常の敵のリストの取得
	std::vector<NormalEnemy*> enemyes = enemyManager_->GetNormalEnemies();
	for (const auto& enemy : enemyes) {
		//ダメージを受け入れる
		enemy->SetIsAcceptDamaged(isReleaseAttack_);
		if (isReleaseAttack_ == true) {
			//ライトの強さを反映
			enemy->SetLightStrength(player_->GetFlashLight()->GetChargeCondition());
			collisionManager_->RegisterList(enemy->GetEnemyFlashLightCollision());

		}

		//攻撃
		if (enemy->GetIsAttack() == true) {
			//敵の攻撃
			collisionManager_->RegisterList(enemy->GetEnemyAttackCollision());
			player_->SetIsAcceptDamegeFromNoemalEnemy(true);
		}

	}

	//プレイヤーのコライダー
	std::vector<BasePlayerCollision*> playerColliders = player_->GetColliders();
	for (const auto& collider : playerColliders) {
		collisionManager_->RegisterList(collider);
	}

	//強敵
	std::vector<StrongEnemy*> strongEnemyes = enemyManager_->GetStrongEnemies();
	for (const StrongEnemy* strongEnemy : strongEnemyes) {
		bool isTouch = strongEnemy->GetStrongEnemyCollisionToPlayer()->GetIsTouchPlayer();
		collisionManager_->RegisterList(strongEnemy->GetStrongEnemyCollisionToPlayer());
		//接触
		if (isTouch == true) {
			isTouchStrongEnemy_ = true;
		}
	}

	//レベルエディタのオーディオ用
	std::vector<BaseObjectForLevelEditorCollider*> audioColliders = levelDataManager_->GetCollider(levelDataHandle_, "Audio");
	for (std::vector<BaseObjectForLevelEditorCollider*>::iterator it = audioColliders.begin(); it != audioColliders.end(); ++it) {
		collisionManager_->RegisterList(*it);

	}
}

void PlayGameScene::PlayerMove(){

	//何も押していない時つまり動いていないので通常はfalseと0にしておく
	//キーボードで動かしているかどうか
	bool isPlayerMoveKey = false;
	//動いているかどうか
	bool isPlayerMove = false;
	//向き
	Vector3 playerMoveDirection = { .x = 0.0f,.y = 0.0f,.z = 0.0f };

#pragma region キーボード
	//移動
	//Dキー(右)
	if (input_->IsPushKey(DIK_D) == true) {
		//動く方向
		playerMoveDirection = {
			.x = std::cosf(theta_ - std::numbers::pi_v<float_t> / 2.0f),
			.y = 0.0f,
			.z = std::sinf(theta_ - std::numbers::pi_v<float_t> / 2.0f),
		};

		//キーボード入力をしている
		isPlayerMoveKey = true;
		//動いている
		isPlayerMove = true;

	}
	//Aキー(左)
	if (input_->IsPushKey(DIK_A) == true) {
		//動く方向
		playerMoveDirection = {
			.x = std::cosf(theta_ + std::numbers::pi_v<float_t> / 2.0f),
			.y = 0.0f,
			.z = std::sinf(theta_ + std::numbers::pi_v<float_t> / 2.0f),
		};

		//キーボード入力をしている
		isPlayerMoveKey = true;
		//動いている
		isPlayerMove = true;
	}
	//Wキー(前)
	if (input_->IsPushKey(DIK_W) == true) {
		//動く方向
		playerMoveDirection = {
			.x = std::cosf(theta_),
			.y = 0.0f,
			.z = std::sinf(theta_),
		};

		//キーボード入力をしている
		isPlayerMoveKey = true;
		//動いている
		isPlayerMove = true;
	}
	//Sキー(後ろ)
	if (input_->IsPushKey(DIK_S) == true) {
		playerMoveDirection.x = std::cosf(theta_ + std::numbers::pi_v<float_t>);
		playerMoveDirection.z = std::sinf(theta_ + std::numbers::pi_v<float_t>);

		//キーボード入力をしている
		isPlayerMoveKey = true;
		//動いている
		isPlayerMove = true;
	}


#pragma endregion

#pragma region コントローラー

	//接続時
	//キーボード入力していない時かつ移動できる時に受け付ける
	if (isPlayerMoveKey == false) {

		//コントローラーの入力
		bool isInput = false;
		//左スティック
		Vector3 leftStickInput = {
			.x = (static_cast<float_t>(input_->GetCurrentState().Gamepad.sThumbLX) / SHRT_MAX),
			.y = 0.0f,
			.z = (static_cast<float_t>(input_->GetCurrentState().Gamepad.sThumbLY) / SHRT_MAX),
		};


		//デッドゾーンの設定
		const float_t DEAD_ZONE = 0.1f;
		//X軸
		if (std::abs(leftStickInput.x) < DEAD_ZONE) {
			leftStickInput.x = 0.0f;
		}
		else {
			isInput = true;
		}
		//Z軸
		if (std::abs(leftStickInput.z) < DEAD_ZONE) {
			leftStickInput.z = 0.0f;
		}
		else {
			isInput = true;
		}


		//入力されていたら計算
		if (isInput == true) {
			//動いている
			isPlayerMove = true;

			//角度を求める
			float_t radian = std::atan2f(leftStickInput.z, leftStickInput.x);
			//値を0～2πに直してtheta_に揃える
			if (radian < 0.0f) {
				radian += 2.0f * std::numbers::pi_v<float_t>;
			}
			const float_t OFFSET = std::numbers::pi_v<float_t> / 2.0f;
			float_t resultTheta = theta_ + radian - OFFSET;


			//向きを代入
			playerMoveDirection.x = std::cosf(resultTheta);
			playerMoveDirection.z = std::sinf(resultTheta);
		}



	}

#pragma endregion

#pragma region

#ifdef _DEBUG



	//プレイヤーが動いている時
	if (isPlayerMove == true) {
		//ダッシュ
		//ダッシュしているかどうか
		bool isPlayerDash = false;
		if (isPlayerMoveKey == true) {
			if (input_->IsPushKey(DIK_RSHIFT) == true) {
				isPlayerDash = true;
			}
			else {
				isPlayerDash = false;
			}
		}
		//コントローラー接続時
		else {
			if (input_->IsPushButton(XINPUT_GAMEPAD_LEFT_SHOULDER) == true) {
				isPlayerDash = true;
			}
			else {
				isPlayerDash = false;
			}
		}
		//ダッシュをしているかどうかの設定
		player_->SetIsDash(isPlayerDash);
	}
#endif // _DEBUG

	//プレイヤーの動く方向を入れる
	player_->SetMoveDirection(playerMoveDirection);



	//チャージ
	bool isCharge = false;
	//エンターキーまたはXボタンでチャージ開始
	if (input_->IsPushKey(DIK_RETURN) == true || input_->IsPushButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) == true) {
		isCharge = true;
		//チャージ音再生
		audio_->Play(chargeSEHandle_, true);
	}
	else {
		isCharge = false;
		//停止
		audio_->Stop(chargeSEHandle_);
	}

	//チャージ状態を設定
	player_->GetFlashLight()->SetIsCharge(isCharge);

	//エンターキーまたはYボタンを離した瞬間に攻撃する
	if (input_->IsReleaseKey(DIK_RETURN) == true || input_->IsReleaseButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) == true) {
		isReleaseAttack_ = true;
		//クールタイムにする
		player_->GetFlashLight()->SetIsCoolTime(true);
		//カメラの振動
		//攻撃できる時だけにする。
		if (player_->GetFlashLight()->GetChargeCondition() >= ChargeCondition::NormalChargeAttack) {
			player_->GetEyeCamera()->SetIsShake(true);
		}

		//攻撃音再生
		audio_->Play(attackSEHandle_, false);
	}
	else {
		isReleaseAttack_ = false;
	}
}

void PlayGameScene::PlayerRotate(){

	//回転キーXY
	bool isRotateYKey = false;
	bool isRotateXKey = false;
	//回転の大きさ
	const float_t ROTATE_INTERVAL_ = 0.025f;

	//+が左回り
	//左を向く
	if (input_->IsPushKey(DIK_LEFT) == true) {
		theta_ += ROTATE_INTERVAL_;
		isRotateYKey = true;
	}
	//右を向く
	if (input_->IsPushKey(DIK_RIGHT) == true) {
		theta_ -= ROTATE_INTERVAL_;
		isRotateYKey = true;
	}
	//上を向く
	if (input_->IsPushKey(DIK_UP) == true) {
		phi_ -= ROTATE_INTERVAL_;
		isRotateXKey = true;
	}
	//下を向く
	if (input_->IsPushKey(DIK_DOWN) == true) {
		phi_ += ROTATE_INTERVAL_;
		isRotateXKey = true;
	}

	//2πより大きくなったら0にまた戻す
	if (theta_ > 2.0f * std::numbers::pi_v<float_t>) {
		theta_ = 0.0f;
	}
	//-2πより大きくなったら0にまた戻す
	if (theta_ < -2.0f * std::numbers::pi_v<float_t>) {
		theta_ = 0.0f;
	}


#pragma region コントローラーの回転


	

	//キーボード入力していない時
	if (isRotateYKey == false && isRotateXKey == false) {

		//入力
		float_t rotateMoveX = (static_cast<float_t>(input_->GetCurrentState().Gamepad.sThumbRY) / SHRT_MAX * ROTATE_INTERVAL_);
		float_t rotateMoveY = (static_cast<float_t>(input_->GetCurrentState().Gamepad.sThumbRX) / SHRT_MAX * ROTATE_INTERVAL_);

		//勝手に動くので制限を掛ける
		if (rotateMoveY < MOVE_LIMITATION_ && rotateMoveY > -MOVE_LIMITATION_) {
			rotateMoveY = 0.0f;
		}
		if (rotateMoveX < MOVE_LIMITATION_ && rotateMoveX > -MOVE_LIMITATION_) {
			rotateMoveX = 0.0f;
		}

		//補正後の値を代入する
		theta_ -= rotateMoveY;
		phi_ -= rotateMoveX;
	}

#pragma endregion

	//±π/6くらいに制限を掛けておきたい
	//それ以下以上だと首が大変なことになっているように見えるからね
	if (phi_ > std::numbers::pi_v<float_t> / 6.0f) {
		phi_ = std::numbers::pi_v<float_t> / 6.0f;
	}
	if (phi_ < -std::numbers::pi_v<float_t> / 6.0f) {
		phi_ = -std::numbers::pi_v<float_t> / 6.0f;
	}
}

void PlayGameScene::EscapeCondition(){
	
	//ゲート
	if (gate_->isCollision(player_->GetWorldPosition())) {

		//3個取得したら脱出できる
		if (player_->GetHavingKey() >= keyManager_->GetMaxKeyQuantity()) {
			//「出口へ」が表示
			escapeTextSprite_->SetInvisible(false);
			if (escapeTextSprite_->GetIsInvisible() == false) {
				toEscapeSprite_->SetInvisible(true);
			}
			//コントローラーのBボタンを押したら脱出のフラグがたつ
			//Bボタンを押したとき
			//SPACEキーを押したら脱出のフラグがたつ
			if (input_->IsPushButton(XINPUT_GAMEPAD_B) == true || input_->IsPushKey(DIK_SPACE) == true) {
				//脱出
				isSucceedEscape_ = true;
			}
		}
	}
	else {
		//まだ脱出できない
		escapeTextSprite_->SetInvisible(true);
		//3個取得している
		if (player_->GetHavingKey() >= keyManager_->GetMaxKeyQuantity()) {
			toEscapeSprite_->SetInvisible(false);
		}
	}
}

void PlayGameScene::CemeteryProcess(){

	//初期座標を取得
	Vector3 initialPosition = levelDataManager_->GetInitialTranslate(levelDataHandle_, "CloseFenceInCemetery");

	//墓場の鍵を取ったら柵が消える
	if (keyManager_->GetIsPickUpKeyInCemetery() == true) {
		fenceTranslate_ = initialPosition;
	}
	//取っていないかつ墓場にいたら柵が下がり閉じ込められる
	else {
		if (player_->GetWorldPosition().z <= -26.0f &&
			player_->GetWorldPosition().z >= -50.0f) {
			fenceTranslate_ = initialPosition;
			fenceTranslate_.y = 0.0f;
			if (isPlayCloseSE_ == false) {

				//閉まる音を再生
				Elysia::Audio::GetInstance()->Play(closeGateAudioHandle_, false);
				isPlayCloseSE_ = true;
			}

		}
		else {
			fenceTranslate_ = initialPosition;
		}
	}
	//座標の再設定
	levelDataManager_->SetTranslate(levelDataHandle_, "CloseFenceInCemetery", fenceTranslate_);


}

void PlayGameScene::DisplayImGui(){

	ImGui::Begin("プレイ(ゲーム)");
	ImGui::SliderFloat3("座標", &fenceTranslate_.x, 0.0f, 100.0f);
	ImGui::SliderFloat("右門の回転", &rightGateRotateTheta_, 0.0f, 3.0f);
	ImGui::InputFloat("線形補間", &openT_);
	ImGui::InputFloat("フェード", &fadeTransparency_);

	ImGui::End();

}
