#include "LevelDataManager.h"

#include <cassert>
#include <numbers>
#include <filesystem>
#include <iostream>

#include "ModelManager.h"
#include "Camera.h"
#include "Material.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Audio.h"

#include "Model/AudioObjectForLevelEditor.h"
#include "Model/StageObjectForLevelEditor.h"
#include <StringOption.h>



Elysia::LevelDataManager::LevelDataManager() {
	//オーディオのインスタンスを取得
	audio_ = Elysia::Audio::GetInstance();
}


Elysia::LevelDataManager* Elysia::LevelDataManager::GetInstance() {
	static LevelDataManager instance;
	return &instance;
}

void Elysia::LevelDataManager::Place(nlohmann::json& objects, LevelData& levelData) {

	//"objects"の全オブジェクトを走査
	for (nlohmann::json& object : objects) {
		//各オブジェクトに必ずtypeが入っているよ
		assert(object.contains("type"));

		//種別を取得
		std::string type = object["type"].get<std::string>();

		//種類ごとの処理
		//MESHの場合
		if (type.compare("MESH") == 0) {
			//要素追加
			levelData.objectDatas.emplace_back(ObjectData{});
			//今追加した要素の参照を得る
			ObjectData& objectData = levelData.objectDatas.back();


			if (object.contains("name")) {
				objectData.name = object["name"];
			}

			//ここでのファイルネームはオブジェクトの名前
			if (object.contains("file_name")) {
				//ファイル名
				objectData.modelFileName = object["file_name"];
			}

			//トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];

			//スケール
			objectData.transform.scale.x = static_cast<float>(transform["scaling"][0]);
			objectData.transform.scale.y = static_cast<float>(transform["scaling"][2]);
			objectData.transform.scale.z = static_cast<float>(transform["scaling"][1]);
			//初期スケール
			objectData.initialTransform.scale.x = static_cast<float>(transform["scaling"][0]);
			objectData.initialTransform.scale.y = static_cast<float>(transform["scaling"][2]);
			objectData.initialTransform.scale.z = static_cast<float>(transform["scaling"][1]);



			//回転角
			//そういえばBlenderは度数法だったね
			//弧度法に直そう
			const float DEREES_TO_RADIUS_ = static_cast<float>(std::numbers::pi) / 180.0f;
			objectData.transform.rotate.x = -static_cast<float>(transform["rotation"][0]) * DEREES_TO_RADIUS_;
			objectData.transform.rotate.y = -static_cast<float>(transform["rotation"][2]) * DEREES_TO_RADIUS_;
			objectData.transform.rotate.z = -static_cast<float>(transform["rotation"][1]) * DEREES_TO_RADIUS_;
			//初期回転
			objectData.initialTransform.rotate.x = -static_cast<float>(transform["rotation"][0]) * DEREES_TO_RADIUS_;
			objectData.initialTransform.rotate.y = -static_cast<float>(transform["rotation"][2]) * DEREES_TO_RADIUS_;
			objectData.initialTransform.rotate.z = -static_cast<float>(transform["rotation"][1]) * DEREES_TO_RADIUS_;



			//Blenderと軸の方向が違うので注意！
			//座標
			objectData.transform.translate.x = static_cast<float>(transform["translation"][0]);
			objectData.transform.translate.y = static_cast<float>(transform["translation"][2]);
			objectData.transform.translate.z = static_cast<float>(transform["translation"][1]);
			//初期座標
			objectData.initialTransform.translate.x = static_cast<float>(transform["translation"][0]);
			objectData.initialTransform.translate.y = static_cast<float>(transform["translation"][2]);
			objectData.initialTransform.translate.z = static_cast<float>(transform["translation"][1]);



			//オブジェクトのタイプを取得
			if (object.contains("object_type")) {
				objectData.type = object["object_type"];
			}


			//コライダーの読み込み
			//まずあるかどうか
			if (object.contains("collider")) {
				nlohmann::json& collider = object["collider"];

				objectData.isHavingCollider = true;

				// コライダーの種別を取得
				if (collider.contains("type")) {
					objectData.colliderType = collider["type"];
				}


				//BOX
				if (objectData.colliderType == "BOX") {
					//中心座標
					objectData.center.x = static_cast<float>(collider["center"][0]) + objectData.transform.translate.x;
					objectData.center.y = static_cast<float>(collider["center"][2]) + objectData.transform.translate.y;
					objectData.center.z = static_cast<float>(collider["center"][1]) + objectData.transform.translate.z;
					//サイズ
					objectData.size.x = static_cast<float>(collider["size"][0]);
					objectData.size.y = static_cast<float>(collider["size"][2]);
					objectData.size.z = static_cast<float>(collider["size"][1]);

				}
				//AABB
				else if (objectData.colliderType == "AABB") {

					Vector3 center = {};
					center.x = static_cast<float>(collider["center"][0]);
					center.y = static_cast<float>(collider["center"][2]);
					center.z = static_cast<float>(collider["center"][1]);


					//中心座標
					objectData.center.x = static_cast<float>(collider["center"][0]) + objectData.transform.translate.x;
					objectData.center.y = static_cast<float>(collider["center"][2]) + objectData.transform.translate.y;
					objectData.center.z = static_cast<float>(collider["center"][1]) + objectData.transform.translate.z;
					//サイズ
					objectData.size.x = static_cast<float>(collider["size"][0])/2.0f;
					objectData.size.y = static_cast<float>(collider["size"][2])/2.0f;
					objectData.size.z = static_cast<float>(collider["size"][1])/2.0f;

					//右上奥
					objectData.upSize.x = objectData.center.x + objectData.size.x;
					objectData.upSize.y = objectData.center.y + objectData.size.y;
					objectData.upSize.z = objectData.center.z + objectData.size.z;
					//左下手前
					objectData.downSize.x = objectData.center.x - objectData.size.x;
					objectData.downSize.y = objectData.center.y - objectData.size.y;
					objectData.downSize.z = objectData.center.z - objectData.size.z;
				}
				else if (objectData.colliderType == "Plane") {
					//中心座標
					objectData.center.x = static_cast<float>(collider["center"][0]) + objectData.transform.translate.x;
					objectData.center.y = static_cast<float>(collider["center"][2]) + objectData.transform.translate.y;
					objectData.center.z = static_cast<float>(collider["center"][1]) + objectData.transform.translate.z;
					//サイズ
					objectData.size.x = static_cast<float>(collider["size"][0]);
					objectData.size.y = static_cast<float>(collider["size"][2]);
					objectData.size.z = static_cast<float>(collider["size"][1]);

				}


			}
			else {
				objectData.isHavingCollider = false;
			}

			//非表示設定
			if (object.contains("is_invisible")) {
				objectData.isInvisible = object["is_invisible"];
			}


			//オーディオの読み込み
			//まずあるかどうか
			if (objectData.type == "Audio") {
				if (object.contains("audio")) {
					nlohmann::json& audio = object["audio"];


					//種類を記録
					if (audio.contains("type")) {
						objectData.levelAudioData.type = audio["type"];
					}

					//ファイル名を記録
					objectData.levelAudioData.fileName = audio["file_name"];

					//ループをするかどうか
					objectData.levelAudioData.isLoop = audio["loop"];

					//エリア上かどうか
					//trueの場合エリア上でした音が鳴らない
					//falseの場合リスナーと距離が離れると音が小さくなっていく
					//この時コライダーいらない
					if (objectData.isHavingCollider == true) {
						objectData.levelAudioData.isOnArea = audio["on_area"];

					}
					else {
						objectData.levelAudioData.isOnArea = false;
					}
				}
			}

		}
	}

}

void Elysia::LevelDataManager::Ganarate(LevelData& levelData) {

	//ディレクトリパス
	std::string levelEditorDirectoryPath = LEVEL_DATA_PATH_ + levelData.folderName;

	for (ObjectData& objectData : levelData.objectDatas) {

		//ステージ
		if (objectData.type == "Stage") {

			//オブジェクトの生成
			StageObjectForLevelEditor* stageObject = new StageObjectForLevelEditor();

			//モデルの読み込み
			uint32_t modelHandle = ModelManager::GetInstance()->LoadModelFileForLevelData(levelEditorDirectoryPath, objectData.modelFileName);

			//オブジェクトの生成
			stageObject->SetSize(objectData.size);
			stageObject->Initialize(modelHandle, objectData.transform);
			objectData.objectForLeveEditor = stageObject;
			objectData.isModelGenerate = true;

			//コライダーがある場合
			if (objectData.isHavingCollider == true) {
				//生成
				StageObjectForLevelEditorCollider* collider = new StageObjectForLevelEditorCollider();
				collider->SetSize(objectData.size);
				collider->Initialize();

				//代入
				objectData.levelDataObjectCollider = collider;
			}
		}
		//オーディオ
		else if (objectData.type == "Audio") {

			//Audioフォルダの中で読み込み
			std::string audioDir = levelEditorDirectoryPath + "/" + objectData.modelFileName + "/";
			std::string extension = StringOption::FindExtension(audioDir, objectData.levelAudioData.fileName);
			std::string fullPath = audioDir + objectData.levelAudioData.fileName + extension;

			//オーディオデータ
			AudioDataForLevelEditor audioDataForLevelEditor = {
				//ファイル名を記録
				.fileName = objectData.levelAudioData.fileName,
				//種類を記録
				.type = objectData.levelAudioData.type,
				//ハンドルは後で入力する
				.handle = audio_->Load(fullPath),
				//エリア上かどうか
				.isOnArea = objectData.levelAudioData.isOnArea,
				//ループをするかどうか
				.isLoop = objectData.levelAudioData.isLoop,
			};


			//オーディオオブジェクトの生成
			AudioObjectForLevelEditor* audioObject = new AudioObjectForLevelEditor();
			audioObject->SetLevelDataAudioData(audioDataForLevelEditor);

			//モデルの読み込み
			uint32_t modelHandle = ModelManager::GetInstance()->LoadModelFileForLevelData(levelEditorDirectoryPath, objectData.modelFileName);

			//初期化
			audioObject->Initialize(modelHandle, objectData.transform);
			//オブジェクトの生成
			objectData.objectForLeveEditor = audioObject;
			objectData.isModelGenerate = true;

			//コライダーがある場合
			if (objectData.isHavingCollider == true) {

				//生成
				AudioObjectForLevelEditorCollider* collider = new AudioObjectForLevelEditorCollider();
				collider->SetSize(objectData.size);
				collider->Initialize();

				//代入
				objectData.levelDataObjectCollider = collider;

			}
		}
		//モデルは生成しない
		else {
			objectData.isModelGenerate = false;
		}
	}
}

nlohmann::json Elysia::LevelDataManager::Deserialize(const std::string& fullFilePath) {
	std::ifstream file;
	//ファイルを開ける
	file.open(fullFilePath);

	//読み込めないなら止める
	if (file.fail()) {
		assert(0);
	}

	//JSON文字列から解凍したデータ
	nlohmann::json data;
	file >> data;

	//正しいレベルデータファイルかチェック
	//objectかどうか
	assert(data.is_object());
	//namaeのキーワードがあるかどうか
	assert(data.contains("name"));
	//nameはstring型かどうか
	assert(data["name"].is_string());

	//"name"を文字列として取得
	std::string name = data["name"].get<std::string>();
	//正しいレベルデータファイルはチェック
	assert(name.compare("scene") == 0);


	//値を返す
	return data;

}

uint32_t Elysia::LevelDataManager::Load(const std::string& filePath) {

	//パスの結合
	std::string fullFilePath = LEVEL_DATA_PATH_ + filePath;

	//一度読み込んだものはその値を返す
	//新規は勿論読み込みをする
	auto it = levelDatas_.find(filePath);
	if (it != levelDatas_.end()) {
		return it->second->handle;
	}
	//JSON文字列から解凍したデータ
	nlohmann::json deserialized = Deserialize(fullFilePath);

	//ファイルパスの分解
	std::string folderName = {};
	std::string fileName = {};
	size_t slashPosition = filePath.find('/');

	if (slashPosition != std::string::npos) {
		//0からslashPositionまで
		folderName = filePath.substr(0, slashPosition);
		//「/」から最後まで
		fileName = filePath.substr(slashPosition + 1);
	}

	//ハンドルの加算
	++handle_;

	//インスタンスを生成
	levelDatas_[fullFilePath] = std::make_unique<LevelData>();

	//それぞれに情報を記録
	levelDatas_[fullFilePath]->folderName = folderName;
	levelDatas_[fullFilePath]->fileName = fileName;
	levelDatas_[fullFilePath]->handle = handle_;
	levelDatas_[fullFilePath]->fullPath = fullFilePath;

	

	//指定したファイルパスのレベルデータを持ってくる
	LevelData& levelData = *levelDatas_[fullFilePath];

	//配置
	Place(deserialized["objects"], levelData);

	//生成
	Ganarate(levelData);



	//番号を返す
	return levelDatas_[fullFilePath]->handle;

}

void Elysia::LevelDataManager::Reload(const uint32_t& levelDataHandle) {


	//一度全てのオブジェクトのデータを消す
	for (auto& [key, levelDataPtr] : levelDatas_) {
		if (levelDataPtr->handle == levelDataHandle) {

			//モデルを消す
			for (auto& object : levelDataPtr->objectDatas) {
				// Modelの解放
				if (object.objectForLeveEditor != nullptr) {
					delete object.objectForLeveEditor;
				}
				if (object.levelDataObjectCollider != nullptr) {
					delete object.levelDataObjectCollider;
				}

			}


			//listにある情報を全て消す
			levelDataPtr->objectDatas.clear();

			//無駄なループ処理をしないようにする
			break;
		}
	}



	//引数から探す
	std::string fullFilePath = {};
	for (std::map<std::string, std::unique_ptr<LevelData>>::iterator it = levelDatas_.begin(); it != levelDatas_.end(); ++it) {
		LevelData* levelData = it->second.get();

		//見つけたらfullFilePathに入れる
		if (levelData->handle == levelDataHandle) {
			fullFilePath = levelData->fullPath;
			break;
		}
	}



	//JSON文字列から解凍したデータ
	nlohmann::json deserialized = Deserialize(fullFilePath);



	//元々あったデータを取得する
	//オブジェクトは全部消したけどここにファイルパスの情報が残っているよ
	LevelData& levelData = *levelDatas_[fullFilePath];

	//読み込み
	Place(deserialized["objects"], levelData);

	//生成
	Ganarate(levelData);

}

void Elysia::LevelDataManager::Update(const uint32_t& levelDataHandle) {

	//この書き方はC++17からの構造化束縛というものらしい
	//イテレータではなくこっちでやった方が良いかな
	//ファイル名とかで指定したい時はkeyを使ったら楽だね。今回はハンドルだけどね。
	for (auto& [key, levelData] : levelDatas_) {
		if (levelData->handle == levelDataHandle) {

			//リスナーが動いているかどうか
			bool isListenerMove = false;

			//動いていなかった場合
			if (levelData->listener.move.x == 0.0f &&
				levelData->listener.move.y == 0.0f &&
				levelData->listener.move.z == 0.0f) {
				isListenerMove = false;
			}
			else {
				isListenerMove = true;
			}

			for (const auto& object : levelData->objectDatas) {
				//モデルを生成した時
				if (object.isModelGenerate == true) {
					//更新
					object.objectForLeveEditor->SetIsListenerMove(isListenerMove);
					object.objectForLeveEditor->Update();
					Vector3 objectWorldPosition = object.objectForLeveEditor->GetWorldPosition();

					//衝突判定の設定
					if (object.isHavingCollider == true) {
						bool isTouch = object.levelDataObjectCollider->GetIsTouch();
						object.objectForLeveEditor->SetIsTouch(isTouch);
						object.levelDataObjectCollider->SetObjectPosition(objectWorldPosition);
						object.levelDataObjectCollider->SetCenterPosition(object.center);
						object.levelDataObjectCollider->Update();
					}
				}
			}
			break;
		}
	}
}

void Elysia::LevelDataManager::Delete(const uint32_t& levelDataHandle) {


	//一度全てのオブジェクトのデータを消す
	for (auto& [key, levelDataPtr] : levelDatas_) {
		if (levelDataPtr->handle == levelDataHandle) {

			//モデルを消す
			for (auto& object : levelDataPtr->objectDatas) {
				// Modelの解放
				if (object.objectForLeveEditor != nullptr) {
					delete object.objectForLeveEditor;
				}
				if (object.levelDataObjectCollider != nullptr) {
					delete object.levelDataObjectCollider;
				}

			}


			//listにある情報を全て消す
			levelDataPtr->objectDatas.clear();

			//無駄なループ処理をしないようにする
			break;
		}
	}
}

#pragma region 描画

void Elysia::LevelDataManager::Draw(const uint32_t& levelDataHandle, const Camera& camera) {
	//指定したハンドルのデータだけを描画
	for (auto& [key, levelData] : levelDatas_) {
		if (levelData->handle == levelDataHandle) {

			//描画
			for (const auto& object : levelData->objectDatas) {
				if (object.isInvisible == false && object.isModelGenerate == true) {
					object.objectForLeveEditor->Draw(camera);
				}
			}

			//無駄なループ処理を防ぐよ
			break;

		}

	}
}


void Elysia::LevelDataManager::Draw(const uint32_t& levelDataHandle, const Camera& camera, const DirectionalLight& directionalLight) {

	//指定したハンドルのデータだけを描画
	for (auto& [key, levelData] : levelDatas_) {
		if (levelData->handle == levelDataHandle) {

			//描画
			for (const auto& object : levelData->objectDatas) {
				if (object.isInvisible == false && object.isModelGenerate == true) {
					object.objectForLeveEditor->Draw(camera, directionalLight);
				}
			}

			//無駄なループ処理を防ぐよ
			break;

		}

	}
}

void Elysia::LevelDataManager::Draw(const uint32_t& levelDataHandle, const Camera& camera, const PointLight& pointLight) {
	//指定したハンドルのデータだけを描画
	for (auto& [key, levelData] : levelDatas_) {
		if (levelData->handle == levelDataHandle) {

			//描画
			for (const auto& object : levelData->objectDatas) {
				if (object.isInvisible == false && object.isModelGenerate == true) {
					object.objectForLeveEditor->Draw(camera, pointLight);
				}
			}

			//無駄なループ処理を防ぐよ
			break;

		}

	}
}

void Elysia::LevelDataManager::Draw(const uint32_t& levelDataHandle, const Camera& camera, const SpotLight& spotLight) {

	//指定したハンドルのデータだけを描画
	for (auto& [key, levelData] : levelDatas_) {
		if (levelData->handle == levelDataHandle) {

			//描画
			for (const auto& object : levelData->objectDatas) {
				//描画
				if (object.isInvisible == false && object.isModelGenerate == true) {
					object.objectForLeveEditor->Draw(camera, spotLight);
				}
			}
			//無駄なループ処理を防ぐよ
			break;
		}
	}
}

#pragma endregion


void Elysia::LevelDataManager::Finalize() {

	//全て解放
	for (auto& [key, levelData] : levelDatas_) {
		for (const auto& object : levelData->objectDatas) {
			//オブジェクトの解放
			if (object.objectForLeveEditor != nullptr) {
				delete object.objectForLeveEditor;
			}
			//コライダーの解放
			if (object.levelDataObjectCollider != nullptr) {
				delete object.levelDataObjectCollider;
			}
		}
	}
	//クリア
	levelDatas_.clear();
}


