#pragma once
#pragma once
/**
 * @file WellForGPU.h
 * @brief GPUに送るためのWellの構造体
 * @author 茂木翼
 */

#include <Matrix4x4.h>

//Well...Paletteの色1つ1つを置く場所

/// <summary>
/// GPUに送るためのWell
/// </summary>
struct WellForGPU {
	//位置用
	Matrix4x4 skeletonSpaceMatrix;
	//法線用
	Matrix4x4 skeletonSpaceIncerseTransposeMatrix;
};