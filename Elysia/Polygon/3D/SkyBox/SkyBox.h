#pragma once

/**
 * @file SkyBox.h
 * @brief スカイボックス
 * @author 茂木翼
 */

#include "DirectXSetup.h"


#include "Vector4.h"
#include "Matrix4x4.h"
#include "Matrix4x4Calculation.h"
#include "Transform.h"
#include "VertexData.h"
#include "TransformationMatrix.h"
#include "DirectionalLight.h"

/// <summary>
/// ワールドトランスフォーム
/// </summary>
struct WorldTransform;

/// <summary>
/// カメラ
/// </summary>
struct Camera;


/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {

	#pragma region 前方宣言

	/// <summary>
	/// DirectX
	/// </summary>
	class DirectXSetup;

	/// <summary>
	/// パイプライン管理クラス
	/// </summary>
	class PipelineManager;

	#pragma endregion

	/// <summary>
	/// スカイボックス
	/// </summary>
	class SkyBox {
	public:

		/// <summary>
		/// コンストラクタ
		/// </summary>
		SkyBox();

		/// <summary>
		/// 生成
		/// </summary>
		void Create();

		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="texturehandle"></param>
		/// <param name="worldTransform"></param>
		/// <param name="camera"></param>
		void Draw(const uint32_t& texturehandle, const WorldTransform& worldTransform, const Camera& camera);

		/// <summary>
		/// デストラクタ
		/// </summary>
		~SkyBox() = default;

	private:

		/// <summary>
		/// マテリアル
		/// </summary>
		struct SkyBoxMaterial {
			Vector4 color;
			Matrix4x4 uvTransform;
		};

		/// <summary>
		/// 頂点の位置
		/// </summary>
		enum SkyBoxVertexPosition {
			//右
			//右上奥
			TopRightBack,
			//右上前
			TopRightFront,
			//右下奥
			BottomRightBack,
			//右下前
			BottomRightFront,
		};

	private:
		//DirectX
		DirectXSetup* directXSetup_ = nullptr;
		//パイプライン管理クラス
		PipelineManager* pipelineManager_ = nullptr;

	private:
		//1面の頂点
		const uint32_t SURFACE_VERTEX_ = 6u;
		//面の数
		const uint32_t SURFACE_AMOUNT_ = 6u;

	private:

		//頂点データ
		//リソース
		ComPtr<ID3D12Resource> vertexResource_ = nullptr;
		//バッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
		//データ
		VertexData* vertexData_ = nullptr;
		
		//index用
		ComPtr<ID3D12Resource> indexResource_ = nullptr;
		//IndexBufferView
		D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
		//データ
		uint32_t* indexData_ = nullptr;

		//マテリアル用のリソースを作る
		ComPtr<ID3D12Resource> materialResource_ = nullptr;
		//データ
		SkyBoxMaterial* materialData_ = nullptr;

	};

};
