#pragma once
/**
 * @file Line.h
 * @brief 線のクラス
 * @author 茂木翼
 */

#include <string>

#include "DirectXSetup.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Material.h"
#include "TransformationMatrix.h"


struct Camera;

/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {

	#pragma region 前方宣言
	/// <summary>
	/// DirectXクラス
	/// </summary>
	class DirectXSetup;

	/// <summary>
	/// パイプライン管理クラス
	/// </summary>
	class PipelineManager;

	#pragma endregion

	/// <summary>
	/// 線
	/// </summary>
	class Line {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Line();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="start">始点</param>
		/// <param name="end">終点</param>
		/// <param name="camera">カメラ</param>
		void Draw(const Vector3& start, const Vector3& end, const Camera& camera);


		/// <summary>
		/// デストラクタ
		/// </summary>
		~Line() = default;

	public:

		/// <summary>
		/// 色の設定
		/// </summary>
		/// <param name="color"></param>
		void SetColor(const Vector4& color) {
			this->color_ = color;
		}

	private:
		/// <summary>
		/// 行列
		/// </summary>
		struct LineTransformMatrix {
			Matrix4x4 WVP;
			Matrix4x4 World;
		};

		/// <summary>
		/// 頂点データの拡張
		/// </summary>
		struct LineVertexData {
			Vector4 position;
		};


	private:
		//DirectX
		Elysia::DirectXSetup* directXSetup_ = nullptr;
		//パイプライン管理クラス
		Elysia::PipelineManager* pipelineManager_=nullptr;

	private:

		//頂点
		//バッファービュー
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
		//リソース
		ComPtr<ID3D12Resource> vertexResouce_ = nullptr;
		

		//マテリアル用のリソースを作る
		ComPtr<ID3D12Resource> materialResource_ = nullptr;

		//TransformationMatrix用のResource
		ComPtr<ID3D12Resource> wvpResource_ = nullptr;
		//データ
		LineTransformMatrix* wvpData_ = nullptr;

		//色
		Vector4 color_ = {};
	};

};