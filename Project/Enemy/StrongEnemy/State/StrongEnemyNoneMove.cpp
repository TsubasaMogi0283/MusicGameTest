#include "StrongEnemyNoneMove.h"

#include "Enemy/StrongEnemy/StrongEnemy.h"

StrongEnemyNoneMove::StrongEnemyNoneMove(){
	stateName_ = "NoneMove";
}

void StrongEnemyNoneMove::Initialize() {
	//動かない
	speed_ = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
}

void StrongEnemyNoneMove::Update(StrongEnemy* strongEnemy){
	//スピードの設定
	strongEnemy->SetSpeed(speed_);
}
