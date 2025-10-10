#include "TitleRailCamera.h"

#include <imgui.h>
#include <cmath>
#include <algorithm>
#include <fstream>

#include "VectorCalculation.h"
#include "SingleCalculation.h"
#include "Matrix4x4Calculation.h"
#include "Quaternion.h"
#include "Calculation/QuaternionCalculation.h"

void TitleRailCamera::Initialize(const std::string& csvPath){

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	//カメラの初期化
	camera_.Initialize();

	//ファイルを開ける
	std::ifstream file;
	file.open(csvPath);
	//開かなかったら止まる
	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	positionsFromCSV_ << file.rdbuf();
	//ファイルを閉じる
	file.close();

	//1行分の文字列を入れる変数
	std::string line;


	//制御点読み込み
	while (std::getline(positionsFromCSV_, line)) {

		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream lineStream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		std::getline(lineStream, word, ',');

		//「//」があった行の場合コメントなので飛ばす
		if (word.find("//") == 0) {
			//コメントは飛ばす
			continue;
		}


		Vector3 position = {};
		//X座標
		std::getline(lineStream, word, ',');
		position.x = static_cast<float_t>(std::atof(word.c_str()));

		//Y座標
		std::getline(lineStream, word, ',');
		position.y = static_cast<float_t>(std::atof(word.c_str()));

		//Z座標
		std::getline(lineStream, word, ',');
		position.z = static_cast<float_t>(std::atof(word.c_str()));

		//挿入
		points_.push_back(position);


	}

}

void TitleRailCamera::Update(){

	//線形補間
	cameraT_ += increaseTValue_;
	
	//少し先のTの値を見て次の位置の計算をする
	const float_t NEXT_VALUE = 0.001f;
	float_t nextT = cameraT_ + NEXT_VALUE;
	//ループ時の補正
	const float_t MAX_T_VALUE = 1.0f;
	if (nextT > MAX_T_VALUE) {
		nextT -= MAX_T_VALUE;
	};

	//座標
	worldTransform_.translate = CatmullRomPositionLoop(points_, cameraT_);

	//現在の位置
	Vector3 currentPosition = worldTransform_.translate;
	//次の座標
	Vector3 nextPosition = CatmullRomPositionLoop(points_, nextT);
	//差分
	Vector3 difference = VectorCalculation::Subtract(nextPosition, currentPosition);
	//進行方向ベクトル
	direction_ = VectorCalculation::Normalize(difference);

	//Y軸の回転
	worldTransform_.rotate.y = std::atan2(direction_.x, direction_.z);
	//X軸の回転
	//XZの長さ.まず先に長さを求めてから回転を求める.
	float_t velocityXZ = std::sqrt((direction_.x * direction_.x) + (direction_.z * direction_.z));
	worldTransform_.rotate.x = std::atan2(-direction_.y, velocityXZ);

	//ワールド行列を計算
	worldTransform_.worldMatrix = Matrix4x4Calculation::MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
	//ビュー行列の計算
	camera_.viewMatrix = Matrix4x4Calculation::Inverse(worldTransform_.worldMatrix);



#ifdef _DEBUG
	//IMGui表示用
	DisplayImGui();
#endif // _DEBUG

	
}

void TitleRailCamera::DisplayImGui(){
	ImGui::Begin("レールカメラ");
	ImGui::DragFloat3("座標", &worldTransform_.translate.x);
	ImGui::InputFloat3("方向", &direction_.x);
	ImGui::SliderFloat("T", &cameraT_, 0.0f, 1.0f);
	ImGui::InputFloat3("回転", &worldTransform_.rotate.x);
	ImGui::End();

}

Vector3 TitleRailCamera::CatmullRomPositionLoop(const std::vector<Vector3>& points, const float_t& t) {
	//4点以上で動作するので
	//それ未満は止める
	assert(points.size() >= 4);

	//区間数は制御点の数-1
	//初期化処理の所の制御点に入っている数を参照してあげる
	size_t division = points.size() - 1;
	//1区間の長さ(全体を1.0とした割合)
	float_t areaLength = 1.0f / division;

	//区間内の始点を0.0f、終点を1.0としたときの現在位置
	float_t t2 = std::fmod(t, areaLength) * division;
	//下限(0.0f)と上限(1.0f)の範囲に収める
	t2 = std::clamp(t2, 0.0f, 1.0f);

	int32_t index = static_cast<int32_t>(t / areaLength);
	int32_t index0 = index - 1;
	int32_t index1 = index;
	int32_t index2 = index + 1;
	int32_t index3 = index + 2;



	//始点&終点だった場合制御点を設定し直すよ
	//最初の区間のp0はp1を重複使用する
	if (index == 0) {
		index0 = index1;
	}

	t;
	//最後の区間のp3はp2を重複使用する
	if (index3 >= points.size()) {
		index3 = index2;

		//また最初に戻る
		if (cameraT_ > 1.0f) {
			cameraT_ = 0.0f;
			index = 0;
			index0 = index;
			index1 = index;
			index2 = index + 1;
			index3 = index + 2;
		}
	}

	//4点の座標
	const Vector3& P0 = points[index0];
	const Vector3& P1 = points[index1];
	const Vector3& P2 = points[index2];
	const Vector3& P3 = points[index3];


	//結果
	return VectorCalculation::CatmullRom(P0, P1, P2, P3, t2);
}
