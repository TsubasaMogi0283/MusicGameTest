#include "PushBackCalculation.h"

#include <cstdlib>
#include <numbers>

#include "CollisionCalculation.h"
#include "VectorCalculation.h"

Vector3 PushBackCalculation::AABBPushBack(const AABB& aabb1, const AABB& aabb2){
	//手前側(左・下・前)
	Vector3 defferenceFront = VectorCalculation::Subtract(aabb2.max, aabb1.min);
	//奥側(右・上・奥)
	Vector3 defferenceBack = VectorCalculation::Subtract(aabb1.max, aabb2.min);

	//最小の押し戻し量を求める
	Vector3 minPush = {
		.x = (defferenceFront.x < defferenceBack.x) ? defferenceFront.x : -defferenceBack.x,
		.y = (defferenceFront.y < defferenceBack.y) ? defferenceFront.y : -defferenceBack.y,
		.z = (defferenceFront.z < defferenceBack.z) ? defferenceFront.z : -defferenceBack.z
	};

	Vector3 result = {};
	// 一番押し戻し量が少ない軸で処理する
	if (std::abs(minPush.x) <= std::abs(minPush.y) &&
		std::abs(minPush.x) <= std::abs(minPush.z)) {
		result = {.x = minPush.x,.y = 0.0f,.z = 0.0f };
	}
	else if (std::abs(minPush.y) <= std::abs(minPush.x) &&
		std::abs(minPush.y) <= std::abs(minPush.z)) {
		result = {.x = 0.0f,.y = minPush.y,.z = 0.0f };
	}
	else {
		result = {.x = 0.0f,.y = 0.0f,.z = minPush.z };
	}


	return result;
}

void PushBackCalculation::FixPosition(Vector3& centerPosition, AABB& mainAABB, const AABB& targetAABB) {
	if (CollisionCalculation::IsCollisionAABBPair(mainAABB, targetAABB)) {

		//押し出し距離
		Vector3 pushBack = PushBackCalculation::AABBPushBack(mainAABB, targetAABB);

		//プレイヤーの中心座標を更新
		centerPosition = VectorCalculation::Add(centerPosition, pushBack);

		//縦横高さそれぞれの幅を計算
		Vector3 mainAABBSize= VectorCalculation::Subtract(mainAABB.max,targetAABB.min);
		mainAABBSize = { .x = mainAABBSize.x / 2.0f,.y = mainAABBSize.y / 2.0f,.z = mainAABBSize.z };
		//AABBを更新
		mainAABB.max = VectorCalculation::Add(centerPosition, mainAABBSize);
		mainAABB.min = VectorCalculation::Subtract(centerPosition, mainAABBSize);
	}

}
