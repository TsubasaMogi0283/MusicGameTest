#include "StrongEnemyMove.h"

#include <imgui.h>

#include "Enemy/StrongEnemy/StrongEnemy.h"
#include "VectorCalculation.h"


StrongEnemyMove::StrongEnemyMove(){
	stateName_ = "Move";
}

void StrongEnemyMove::Initialize(){
	
	//方向をランダムで決める
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());

	//スピード(方向)を決める
	std::uniform_real_distribution<float> speedDistribute(-1.0f, 1.0f);
	direction_ = { .x = speedDistribute(randomEngine) ,.y = 0.0f,.z = speedDistribute(randomEngine) };

}

void StrongEnemyMove::Update(StrongEnemy* strongEnemy){

	const float SPEED_AMOUNT = 0.02f;
	//本体の座標を取得
	Vector3 worldPosition = strongEnemy->GetWorldPosition();
	//正規化
	direction_ = VectorCalculation::Normalize(direction_);
	//スピードの値をかける
	direction_ = VectorCalculation::Multiply(direction_, SPEED_AMOUNT);
	//加算
	strongEnemy->SetTranslate(VectorCalculation::Add(worldPosition, direction_));
	
#ifdef _DEBUG
	ImGui::Begin("強敵通常動作");
	ImGui::InputFloat3("方向", &direction_.x);
	ImGui::End();
#endif // DEBUG

}
