#pragma once
#pragma once
/**
 * @file SkinCluster.h
 * @brief スキンクラスターの構造体
 * @author 茂木翼
 */

#include <vector>
#include <span>

#include "DirectXSetup.h"
#include "Matrix4x4.h"
#include "VertexInfluence.h"
#include "WellForGPU.h"
#include "Skeleton.h"
#include "ModelData.h"
#include "WorldTransform.h"
#include "Camera.h"

/// <summary>
/// スキンクラスター
/// </summary>
struct SkinCluster {
public:
	/// <summary>
	/// SkinClusterを作る
	/// </summary>
	/// <param name="skeleton"></param>
	/// <param name="modelData"></param>
	void Create(const Skeleton& skeleton, const ModelData& modelData);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="newSkeleton"></param>
	void Update(const Skeleton& newSkeleton);

	
public:
	//逆転置行列
	std::vector<Matrix4x4> inverseBindPoseMatrices = {};

	//Influence
	//頂点に対して影響を与えるパラメータ群
	ComPtr<ID3D12Resource> influenceResource = nullptr;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView = {};
	std::span<VertexInfluence>mappedInfluence = {};

	//MatrixPalette
	//Skinningを行う際に必要な行列をSkeletonの全Jointの数だけ格納した配列
	ComPtr<ID3D12Resource>paletteResource = nullptr;
	std::span<WellForGPU> mappedPalette = {};
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle = {};
	
	//SRV
	uint32_t srvIndex = 0u;
	
	//スケルトン
	Skeleton skeleton = {};
	


};



