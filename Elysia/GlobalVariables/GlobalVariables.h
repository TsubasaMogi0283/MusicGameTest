#pragma once

/**
 * @file GlobalVariables.h
 * @brief 調整項目クラス
 * @author 茂木翼
 */

#include <iostream>
#include <variant>
#include <string>
#include <map>
#include <fstream>
#include <json.hpp>


#include "Vector3.h"
#include "Vector2.h"

/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {

	/// <summary>
	/// 調整項目クラス(グローバル変数クラス)
	/// </summary>
	class GlobalVariables {
	private:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		GlobalVariables() = default;

		/// <summary>
		/// デストラクタ
		/// </summary>
		~GlobalVariables() = default;



	public:
		/// <summary>
		/// インスタンスの取得
		/// </summary>
		/// <returns></returns>
		static GlobalVariables* GetInstance();

		/// <summary>
		/// コピーコンストラクタ禁止
		/// </summary>
		/// <param name="obj"></param>
		GlobalVariables(const GlobalVariables& obj) = delete;

		/// <summary>
		/// 代入演算子を無効にする
		/// </summary>
		/// <param name="obj"></param>
		/// <returns></returns>
		GlobalVariables& operator=(const GlobalVariables& obj) = delete;




	public:
		/// <summary>
		/// グループの作成
		/// </summary>
		/// <param name="groupName">グループ名</param>
		void CreateGroup(const std::string& groupName);

#pragma region 値のセット

		/// <summary>
		/// 値のセット(int)
		/// </summary>
		/// <param name="groupName">グループ名</param>
		/// <param name="key">キー</param>
		/// <param name="value">値</param>
		void SetValue(const std::string& groupName, const std::string& key, const int32_t& value);

		/// <summary>
		/// 値のセット(float)
		/// </summary>
		/// <param name="groupName">グループ名</param>
		/// <param name="key">キー</param>
		/// <param name="value">値</param>
		void SetValue(const std::string& groupName, const std::string& key, const float& value);

		/// <summary>
		/// 値のセット(Vector2)
		/// </summary>
		/// <param name="groupName">グループ名</param>
		/// <param name="key">キー</param>
		/// <param name="value">値</param>
		void SetValue(const std::string& groupName, const std::string& key, const Vector2& value);


		/// <summary>
		/// 値のセット(Vector3)
		/// </summary>
		/// <param name="groupName">グループ名</param>
		/// <param name="key">キー</param>
		/// <param name="value">値</param>
		void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);


#pragma endregion

#pragma region 項目の追加

		//項目が未登録の場合は追加する
		//項目が登録済みの場合は何もしない
		//これによりファイルの読み込みですでに項目が追加された後に
		//初期化処理で値が上書きされないようにする

		/// <summary>
		///	項目の追加(int)
		/// </summary>
		/// <param name="groupName">グループ名</param>
		/// <param name="key"></param>
		/// <param name="value">値</param>
		void AddItem(const std::string& groupName, const std::string& key, const int32_t& value);

		/// <summary>
		/// 項目の追加(float)
		/// </summary>
		/// <param name="groupName">グループ名</param>
		/// <param name="key"></param>
		/// <param name="value">値</param>
		void AddItem(const std::string& groupName, const std::string& key, const float& value);


		/// <summary>
		/// 項目の追加(Vector2)
		/// </summary>
		/// <param name="groupName">グループ名</param>
		/// <param name="key"></param>
		/// <param name="value">値</param>
		void AddItem(const std::string& groupName, const std::string& key, const Vector2& value);


		/// <summary>
		/// 項目の追加(Vector3)
		/// </summary>
		/// <param name="groupName">グループ名</param>
		/// <param name="key"></param>
		/// <param name="value">値</param>
		void AddItem(const std::string& groupName, const std::string& key, const Vector3& value);



#pragma endregion


#pragma region 値の取得

		/// <summary>
		/// int32の値を取得
		/// </summary>
		/// <param name="groupName"></param>
		/// <param name="key"></param>
		/// <returns></returns>
		int32_t GetIntValue(const std::string& groupName, const std::string& key);

		/// <summary>
		/// floatの値を取得
		/// </summary>
		/// <param name="groupName"></param>
		/// <param name="key"></param>
		/// <returns></returns>
		float GetFloatValue(const std::string& groupName, const std::string& key);

		/// <summary>
		/// Vector2の値を取得
		/// </summary>
		/// <param name="groupName"></param>
		/// <param name="key"></param>
		/// <returns></returns>
		Vector2 GetVector2Value(const std::string& groupName, const std::string& key);

		/// <summary>
		/// Vector3の値を取得
		/// </summary>
		/// <param name="groupName"></param>
		/// <param name="key"></param>
		/// <returns></returns>
		Vector3 GetVector3Value(const std::string& groupName, const std::string& key);

#pragma endregion



		/// <summary>
		/// ファイルに書き出し
		/// </summary>
		/// <param name="groupName">グループ名</param>
		void SaveFile(const std::string& groupName);

		/// <summary>
		/// ディレクトリの「全ファイル」の読み込み
		/// </summary>
		void LoadAllFile();

		/// <summary>
		/// ファイルから読み込む
		/// </summary>
		/// <param name="groupName">グループ</param>
		void LoadFile(const std::string& groupName);


		/// <summary>
		/// 更新
		/// </summary>
		void Update();


	private:
		/// <summary>
		/// 項目
		/// </summary>
		struct Item {
			//項目の値
			//variantは複数種類の型のデータを入れることが出来る万能なもの
			//欲しいものを追加していこう！
			std::variant<int32_t, float,Vector2, Vector3 >value;
		};

		/// <summary>
		/// グループ
		/// </summary>
		struct Group {
			//<キー,値>
			std::map<std::string, Item>items;
		};


	private:
		//グローバル変数の保存先ファイルパス
		const std::string DIRECTORY_PATH_ = "Resources/AdjustmentValues/";

	private:
		//全データ
		std::map<std::string, Group>datas_;



	};

};