#pragma once

/**
 * @file LightingType.h
 * @brief ライティングの種類の列挙型
 * @author 茂木翼
 */

/// <summary>
/// ライティングの種類
/// </summary>
enum LightingType {
	//無し
	NoneLighting,
	//平行光源
	DirectionalLighting,
	//点光源
	PointLighting,
	//スポット
	SpotLighting,
	//環境マップ
	EnviromentMap,

};