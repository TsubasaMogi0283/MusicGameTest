#pragma once

/**
 * @file Node.h
 * @brief ノードの構造体
 * @author 茂木翼
 */

#include <string>
#include <vector>

#include <Matrix4x4.h>
#include <QuaternionTransform.h>

//親からどんどん子に伸びていく

/// <summary>
/// ノード
/// </summary>
struct Node {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};