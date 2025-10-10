#pragma once

/**
 * @file LevelDataManager.h
 * @brief レベルデータ管理クラス
 * @author 茂木翼
 */

#include <string>
#include <list>
#include <map>
#include <memory>
#include <fstream>
#include <json.hpp>

#include "Vector3.h"
#include "Model.h"
#include "WorldTransform.h"
#include "Collider.h"
#include "Transform.h"
#include "Model/BaseObjectForLevelEditor.h"
#include "Model/AudioObjectForLevelEditor.h"
#include "Listener.h"

#pragma region 前方宣言

 /// <summary>
 /// カメラ
 /// </summary>
struct Camera;

/// <summary>
/// 平行光源
/// </summary>
struct DirectionalLight;

/// <summary>
/// 点光源
/// </summary>
struct PointLight;

/// <summary>
/// スポットライト
/// </summary>
struct SpotLight;

#pragma endregion



/// <summary>
/// EllysiaEngine
/// </summary>
namespace Elysia {

	/// <summary>
	/// レベルデータ管理クラス
	/// </summary>
	class LevelDataManager final {
	private:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		LevelDataManager();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~LevelDataManager() = default;

	public:
		/// <summary>
		/// インスタンスの取得
		/// </summary>
		/// <returns>インスタンス</returns>
		static LevelDataManager* GetInstance();

		/// <summary>
		/// コピーコンストラクタ禁止
		/// </summary>
		/// <param name="levelDataManager">レベル管理クラス</param>
		LevelDataManager(const LevelDataManager& levelDataManager) = delete;

		/// <summary>
		/// 代入演算子を無効にする
		/// </summary>
		/// <param name="levelDataManager">レベル管理クラス</param>
		/// <returns></returns>
		LevelDataManager& operator=(const LevelDataManager& levelDataManager) = delete;

	public:
		/// <summary>
		/// レベルデータの読み込み
		/// </summary>
		/// <param name="filePath">ファイルパス</param>
		/// <returns>ハンドル</returns>
		uint32_t Load(const std::string& filePath);

		/// <summary>
		/// 再読み込み
		/// </summary>
		/// <param name="levelDataHandle">ハンドル</param>
		void Reload(const uint32_t& levelDataHandle);

		/// <summary>
		/// 更新
		/// </summary>
		/// <param name="levelDataHandle">ハンドル</param>
		void Update(const uint32_t& levelDataHandle);

		/// <summary>
		/// 消去
		/// </summary>
		/// <param name="levelDataHandle">ハンドル</param>
		void Delete(const uint32_t& levelDataHandle);

		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="levelDataHandle"></param>
		/// <param name="camera"></param>
		void Draw(const uint32_t& levelDataHandle, const Camera& camera);

		/// <summary>
		/// 描画(平行光源)
		/// </summary>
		/// <param name="levelDataHandle">ハンドル</param>
		/// <param name="camera">カメラ</param>
		/// <param name="directionalLight">平行光源</param>
		void Draw(const uint32_t& levelDataHandle, const Camera& camera, const DirectionalLight& directionalLight);

		/// <summary>
		/// 描画(点光源)
		/// </summary>
		/// <param name="levelDataHandle">ハンドル</param>
		/// <param name="camera">カメラ</param>
		/// <param name="pointLight">点光源</param>
		void Draw(const uint32_t& levelDataHandle, const Camera& camera, const PointLight& pointLight);

		/// <summary>
		/// 描画(スポットライト)
		/// </summary>
		/// <param name="levelDataHandle">ハンドル</param>
		/// <param name="camera">カメラ</param>
		/// <param name="spotLight">スポットライト</param>
		void Draw(const uint32_t& levelDataHandle, const Camera& camera, const SpotLight& spotLight);

		/// <summary>
		/// 解放
		/// デストラクタの代わり
		/// </summary>
		void Finalize();


	public:

		/// <summary>
		/// 指定したオブジェクトタイプのコライダーを取得する
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <param name="objectType">オブジェクトの型</param>
		/// <returns>コライダー</returns>
		inline std::vector<BaseObjectForLevelEditorCollider*> GetCollider(const uint32_t& handle,const std::string& objectType) {
			std::vector<BaseObjectForLevelEditorCollider*> colliders = {};

			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {

						//コライダーを持っている場合、リストに追加
						if (objectData.levelDataObjectCollider != nullptr&& objectData.type == objectType) {
							colliders.push_back(objectData.levelDataObjectCollider);
						}
					}

					//無駄なループを防ぐ
					break;
				}
			}

			return colliders;
		}

		/// <summary>
		/// オブジェクトの座標を取得
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <param name="objectType">タイプ</param>
		/// <returns></returns>
		inline std::vector<Vector3> GetObjectPositions(const uint32_t& handle, const std::string& objectType) {
			std::vector<Vector3> positions = {};
			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {
						
						//指定されたオブジェクトだったら追加
						if (objectData.type == objectType) {
							if (objectData.isModelGenerate == true) {
								positions.push_back(objectData.objectForLeveEditor->GetWorldPosition());
							}
							else {
								//モデルを生成しない場合は初期座標を入れる
								positions.push_back(objectData.initialTransform.translate);
							}
							
						}

					}

					//無駄なループを防ぐ
					break;
				}
			}

			return positions;
		}



		/// <summary>
		/// AABBを取得
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <param name="objectType">タイプ</param>
		/// <returns></returns>
		inline std::vector<AABB>GetObjectAABBs(const uint32_t& handle, const std::string& objectType) {
			std::vector<AABB> aabbs = {};

			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {


					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {

						//指定したオブジェクトタイプだったら追加
						if (objectData.type == objectType) {
							aabbs.push_back(objectData.objectForLeveEditor->GetAABB());
						}


					}

					//無駄なループを防ぐ
					break;
				}
			}

			return aabbs;
		}

		/// <summary>
		/// コライダーを持っているかどうかの取得
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <param name="objectType">オブジェクトの型</param>
		/// <returns>持っているかどうかのフラグ</returns>
		inline std::vector<bool> GetIsHavingColliders(const uint32_t& handle,const std::string& objectType) {
			std::vector<bool> colliders = {};

			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {
						//コライダーを持っているかどうかのフラグを挿入
						if (objectData.type == objectType) {
							colliders.push_back(objectData.isHavingCollider);
						}
					}

					//無駄なループを防ぐ
					break;
				}
			}

			return colliders;
		}

	private:

		/// <summary>
		/// オブジェクトデータ
		/// </summary>
		struct ObjectData {
			//オブジェクトのタイプ
			//今はステージかオーディオのどちらか
			std::string type;

			//個別の名前
			std::string name;

			//ファイル名
			std::string modelFileName;

			//トランスフォーム
			Transform transform;
			//初期トランスフォーム
			Transform initialTransform;

			//コライダーを持っているかどうか
			bool isHavingCollider = false;

			//Colliderの種類
			std::string colliderType;

			//Sphere,Box
			Vector3 center;
			Vector3 size;

			//AABB
			AABB aabb;
			Vector3 upSize;
			Vector3 downSize;


			//非表示設定
			bool isInvisible = false;

			//レベルデータのオーディオ
			AudioDataForLevelEditor levelAudioData;

			//オブジェクト(ステージかオーディオ)
			BaseObjectForLevelEditor* objectForLeveEditor;

			//モデルを生成するかどうか
			bool isModelGenerate = false;

			//コライダー
			BaseObjectForLevelEditorCollider* levelDataObjectCollider;


		};


		/// <summary>
		/// レベルデータ
		/// </summary>
		struct LevelData {

			//ハンドル
			uint32_t handle = 0u;

			//オブジェクトのリスト
			std::list<ObjectData> objectDatas;

			//リスナー
			//プレイヤーなどを設定してね
			Listener listener = {};

			//フォルダ名
			std::string folderName;
			//ファイル名
			std::string fileName;
			//フルパス
			std::string fullPath;

		};


	public:
		/// <summary>
		/// リスナーの設定
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <param name="listener">リスナー</param>
		inline void SetListener(const uint32_t& handle, const Listener& listener) {
			for (const auto& [key, levelData] : levelDatas_) {

				//一致したら設定
				if (levelData->handle == handle) {
					levelData->listener = listener;
					//無駄なループを防ぐ
					break;
				}
			}
		}

		/// <summary>
		/// 個別のスケールの変更
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="name"></param>
		/// <param name="scale"></param>
		inline void SetScale(const uint32_t& handle, const std::string& name, const Vector3& scale) {

			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {
						//名前が一致したらスケールの変更
						if (objectData.name == name) {
							objectData.objectForLeveEditor->SetScale(scale);
							//無駄なループを防ぐ
							break;
						}


					}

					//無駄なループを防ぐ
					break;
				}
			}

		}

		/// <summary>
		/// 個別の回転を変える
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="name"></param>
		/// <param name="rotate"></param>
		inline void SetRotate(const uint32_t& handle, const std::string& name, const Vector3& rotate) {

			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {
						//名前が一致したらスケールの変更
						if (objectData.name == name) {
							objectData.objectForLeveEditor->SetRotate(rotate);
							//無駄なループを防ぐ
							break;
						}


					}

					//無駄なループを防ぐ
					break;
				}
			}

		}

		/// <summary>
		/// 個別の座標を変更
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="name"></param>
		inline void SetTranslate(const uint32_t& handle, const std::string& name, const Vector3& translate) {

			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {
						//一致したら座標の変更
						if (objectData.name == name) {
							objectData.objectForLeveEditor->SetPositione(translate);
							//無駄なループを防ぐ
							break;
						}

					}

					//無駄なループを防ぐ
					break;
				}
			}

		}

		/// <summary>
		/// 色の設定
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="name"></param>
		/// <param name="color"></param>
		inline void SetColor(const uint32_t& handle, const std::string& name, const Vector4& color) {
			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {
						//一致したら座標の変更
						if (objectData.name == name) {
							objectData.objectForLeveEditor->SetColor(color);
							//無駄なループを防ぐ
							break;
						}
					}
					//無駄なループを防ぐ
					break;
				}
			}
		}

		/// <summary>
		/// 透明度の設定
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="name"></param>
		/// <param name="transparency"></param>
		inline void SetTransparency(const uint32_t& handle, const std::string& name, const float& transparency) {
			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {
						//一致したら座標の変更
						if (objectData.name == name) {
							objectData.objectForLeveEditor->SetTransparency(transparency);
							//無駄なループを防ぐ
							break;
						}
					}
					//無駄なループを防ぐ
					break;
				}
			}
		}

		/// <summary>
		/// 初期スケールを取得
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <param name="name">名前</param>
		/// <returns>スケール</returns>
		inline Vector3 GetInitiaScale(const uint32_t& handle, const std::string& name) {
			Vector3 result = {};
			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {
						//一致したら座標の変更
						if (objectData.name == name) {
							result = objectData.initialTransform.scale;
							//無駄なループを防ぐ
							break;
						}
					}
					//無駄なループを防ぐ
					break;
				}
			}
			return result;
		}

		/// <summary>
		/// 初期回転を取得
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <param name="name">名前</param>
		/// <returns>回転</returns>
		inline Vector3 GetInitialRotate(const uint32_t& handle, const std::string& name) {
			Vector3 result = {};
			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {
						//一致したら座標の変更
						if (objectData.name == name) {
							result = objectData.initialTransform.rotate;
							//無駄なループを防ぐ
							break;
						}
					}
					//無駄なループを防ぐ
					break;
				}
			}
			return result;
		}

		/// <summary>
		/// 初期座標を取得
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <param name="name">名前</param>
		/// <returns>座標</returns>
		inline Vector3 GetInitialTranslate(const uint32_t& handle, const std::string& name) {
			Vector3 result = {};
			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {
						//一致したら座標の変更
						if (objectData.name == name) {
							result = objectData.initialTransform.translate;
							//無駄なループを防ぐ
							break;
						}
					}
					//無駄なループを防ぐ
					break;
				}
			}
			return result;
		}

		/// <summary>
		/// 非表示設定
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <param name="name">名前</param>
		/// <param name="isInvisible">非表示にするかどうか</param>
		inline void SetInvisible(const uint32_t& handle, const std::string& name, const bool& isInvisible) {
			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {
					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {
						//一致したら非表示設定
						if (objectData.name == name) {
							objectData.isInvisible = isInvisible;
							//無駄なループを防ぐ
							break;
						}
					}
					//無駄なループを防ぐ
					break;
				}
			}
		}


	private:

		/// <summary>
		/// 配置
		/// </summary>
		/// <param name="objects">オブジェクト</param>
		/// <param name="levelData">レベルデータ</param>
		void Place(nlohmann::json& objects, LevelData& levelData);

		/// <summary>
		/// 生成
		/// </summary>
		/// <param name="levelData"></param>
		void Ganarate(LevelData& levelData);


		/// <summary>
		/// JSONファイルを解凍
		/// </summary>
		/// <param name="fullFilePath"></param>
		/// <returns></returns>
		nlohmann::json Deserialize(const std::string& fullFilePath);

	private:
		//オーディオ
		Elysia::Audio* audio_ = nullptr;

	private:
		//Resourceにあるレベルデータの場所
		const std::string LEVEL_DATA_PATH_ = "Resources/LevelData/";

	private:
		//ここにデータを入れていく
		std::map<std::string, std::unique_ptr<LevelData>> levelDatas_;
		//ハンドル
		uint32_t handle_ = 0u;



	};

};