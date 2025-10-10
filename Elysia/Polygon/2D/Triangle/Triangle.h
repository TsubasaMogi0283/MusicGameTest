#pragma once

/**
 * @file Triangle.h
 * @brief 三角形
 * @author 茂木翼
 */

#include <string>

#include "DirectXSetup.h"

#include "Vector4.h"
#include "Matrix4x4.h"
#include "Matrix4x4Calculation.h"
#include "Transform.h"
#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"

/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia{
	/// <summary>
	/// ウィンドウクラス
	/// </summary>
	class WindowsSetup;

	/// <summary>
	/// DirectXクラス
	/// </summary>
	class DirectXSetup;
	
}


/// <summary>
/// 三角形
/// </summary>
class Triangle {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Triangle();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="transform"></param>
	/// <param name="color"></param>
	void Draw(const Transform& transform,const Vector4& color);


	/// <summary>
	/// デストラクタ
	/// </summary>
	~Triangle()=default;

private:
	//ウィンドウクラス
	Elysia::WindowsSetup* windowsSetup_ = nullptr;
	//DirectXクラス
	Elysia::DirectXSetup* directXSetup_ = nullptr;


private:
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	//リソース
	ComPtr<ID3D12Resource> vertexResouce_;
	//頂点データ
	VertexData* vertexData_;
	
	//マテリアル用のリソース
	ComPtr<ID3D12Resource> materialResource_ = nullptr;
	//マテリアルデータ
	MaterialData* materialData_ = nullptr;
	
	//TransformationMatrix用のリソース
	ComPtr<ID3D12Resource> wvpResource_ = nullptr;
	//TransformationMatrixのデータ
	TransformationMatrix* wvpData_=nullptr;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;


};
