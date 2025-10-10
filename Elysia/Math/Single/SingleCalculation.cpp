#include "SingleCalculation.h"
#include <cassert>
#include <numbers>
#include <cmath>

#include "Vector2.h"
#include "Vector3.h"


float_t SingleCalculation::Cot(const float_t& theta){
	return (1.0f / std::tanf(theta));
}


float_t SingleCalculation::Lerp(const float_t& start, const float_t& end, const float_t& t){
	return (1.0f - t) * start + t * end;
}

float_t SingleCalculation::InverseLerp(const float_t& start, const float_t& end, const float_t& value){
	//始点と終点が同じだったら計算をしない
	if (start == end) {
		return 0.0f; 
	}
	return (value - start) / (end - start);
	
}

float_t SingleCalculation::Length(const Vector2& v){
	return sqrtf(v.x * v.x + v.y * v.y);
}

float_t SingleCalculation::Length(const Vector3& v){
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float_t SingleCalculation::Dot(const Vector2& v1, const Vector2& v2){
	return (v1.x * v2.x + v1.y * v2.y);
}

float_t SingleCalculation::Dot(const Vector3& v1, const Vector3& v2){
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}
