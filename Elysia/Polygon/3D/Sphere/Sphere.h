#pragma once

/**
 * @file Sphere.h
 * @brief 球
 * @author 茂木翼
 */

#include "DirectXSetup.h"

#include "Vector4.h"
#include "Matrix4x4.h"
#include "Matrix4x4Calculation.h"
#include "Transform.h"
#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "DirectionalLight.h"
#include "SphereShape.h"


/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {
	/// <summary>
	/// ウィンドウクラス
	/// </summary>
	class WindowsSetup;

	/// <summary>
	/// DirectX
	/// </summary>
	class DirectXSetup;

	/// <summary>
	/// 球
	/// </summary>
	class Sphere {
	public:

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Sphere();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();



		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="sphereCondtion"></param>
		/// <param name="transform"></param>
		/// <param name="viewMatrix"></param>
		/// <param name="projectionMatrix"></param>
		/// <param name="color"></param>
		void Draw(SphereShape sphereCondtion, Transform transform, Matrix4x4 viewMatrix, Matrix4x4 projectionMatrix, Vector4 color);

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Sphere() = default;


	private:
		//ウィンドウクラス
		WindowsSetup* windowsSetup_ = nullptr;
		//DirectX内にある情報を取り入れる
		DirectXSetup* directXSetup_ = nullptr;


	private:

		//関数用
		D3D12_HEAP_PROPERTIES uploadHeapProperties_{};
		D3D12_RESOURCE_DESC vertexResourceDesc_{};

		//頂点バッファビューを作成する
		D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSphere_;

		//初期化
		//頂点データ
		ComPtr<ID3D12Resource> vertexResourceSphere_ = nullptr;
		VertexData* vertexDataSphere_ = nullptr;


		//マテリアル用のリソースを作る
		ComPtr<ID3D12Resource> materialResourceSphere_ = nullptr;
		MaterialData* materialData_ = nullptr;

		//球を描画するとき用のTransformationMatrix用のリソースを作る。
		//Matrix4x4 1つ分サイズを用意する
		ComPtr<ID3D12Resource> transformationMatrixResourceSphere_ = nullptr;
		TransformationMatrix* transformationMatrixDataSphere_ = nullptr;

		//Lighting用
		ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;
		DirectionalLightData* directionalLightData_ = nullptr;

		//分割数
		const int32_t SUBDIVISION_ = 16;

	};
}