#pragma once
/**
 * @file Sprite.h
 * @brief スプライトのクラス
 * @author 茂木翼
 */

#include <string>
#include <format>


#include "DirectXSetup.h"
#include "Vector4.h"
#include "Vector2.h"
#include "Transform.h"
#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"

/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {
	/// <summary>
	/// ウィンドウクラス
	/// </summary>
	class WindowsSetup;

	/// <summary>
	/// DirectXクラス
	/// </summary>
	class DirectXSetup;

	/// <summary>
	/// パイプライン管理クラス
	/// </summary>
	class PipelineManager;

	/// <summary>
	/// スプライト
	/// </summary>
	class Sprite {
	public:

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Sprite();

		/// <summary>
		/// 生成
		/// </summary>
		/// <param name="textureHandle">ハンドル</param>
		/// <param name="position">座標</param>
		/// <returns>スプライト</returns>
		static Sprite* Create(const uint32_t& textureHandle, const Vector2& position);

		/// <summary>
		/// 描画
		/// </summary>
		void Draw();

		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="texturehandle">ハンドル</param>
		void Draw(const uint32_t& texturehandle);

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Sprite() = default;

	public:
		/// <summary>
		/// 透明度の設定
		/// </summary>
		/// <param name="transparency">透明度</param>
		inline void SetTransparency(const float_t& transparency) {
			this->color_.w = transparency;
		}

		/// <summary>
		/// 色の設定
		/// </summary>
		/// <param name="color">色</param>
		inline void SetColor(const Vector4& color) {
			this->color_ = color;
		}

		/// <summary>
		/// 色の設定
		/// </summary>
		/// <param name="color"></param>
		inline void SetColor(const uint32_t& color) {
			color;
			//this->color_ = color;
		}

		/// <summary>
		/// スケールの設定
		/// </summary>
		/// <param name="scale">スケール</param>
		void SetScale(const Vector2& scale) {
			this->scale_ = scale;
		}

		/// <summary>
		/// 回転の設定
		/// </summary>
		/// <param name="rotate">回転</param>
		inline void SetRotate(const float_t& rotate) {
			this->rotate_ = rotate;
		}

		/// <summary>
		/// 位置の設定
		/// </summary>
		/// <param name="position">座標</param>
		inline void SetPosition(const Vector2& position) {
			this->position_ = position;
		}

		/// <summary>
		/// Y座標の設定
		/// </summary>
		/// <param name="positionY"></param>
		inline void SetPositionY(const float& positionY) {
			this->position_.y = positionY;
		}

		/// <summary>
		/// アンカーポイントの設定
		/// </summary>
		/// <param name="point">ポイント</param>
		inline void SetAnchorPoint(const Vector2& point) {
			this->anchorPoint_ = point;
		}

		/// <summary>
		/// X軸フリップ
		/// </summary>
		/// <param name="isFlipX">X軸のフリップ</param>
		inline void SetFlipX(const bool& isFlipX) {
			this->isFlipX_ = isFlipX;
		}

		/// <summary>
		/// Y軸フリップ
		/// </summary>
		/// <param name="isFlipY">Y軸のフリップ</param>
		inline void SetFlipY(const bool& isFlipY) {
			this->isFlipY_ = isFlipY;
		}

		/// <summary>
		/// 見えなくするかどうか
		/// </summary>
		/// <param name="isInvisible">見えなくするかどうか</param>
		inline void SetInvisible(const bool& isInvisible) {
			this->isInvisible_ = isInvisible;
		}

		/// <summary>
		/// 現在非表示かどうかを取得
		/// </summary>
		/// <returns></returns>
		bool GetIsInvisible()const {
			return isInvisible_;
		}

		/// <summary>
		/// 後ろにするかどうか
		/// </summary>
		/// <param name="isBack">後ろか</param>
		inline void SetIsBack(const bool& isBack) {
			this->isBack_ = isBack;
		}

		/// <summary>
		/// UV座標系左上の設定
		/// </summary>
		/// <param name="textureLeftTop">左上の座標</param>
		inline void SetTextureLeftTop(const Vector2& textureLeftTop) {
			this->textureLeftTop_ = textureLeftTop;
		}

		/// <summary>
		/// テクスチャのサイズの設定
		/// </summary>
		/// <param name="textureSize">サイズ</param>
		inline void SetTextureSize(const Vector2& textureSize) {
			this->textureSize_ = textureSize;
		}

		/// <summary>
		/// UV編集モードにするかどうか
		/// </summary>
		/// <param name="isUVMode">編集するかどうか</param>
		inline void SetUVMode(const bool& isUVMode) {
			this->isUVSetting_ = isUVMode;
		}

	private:
		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="textureHandle">ハンドル</param>
		/// <param name="position">座標</param>
		void Initialize(const uint32_t& textureHandle, const Vector2& position);


	private:
		//ウィンドウクラス
		Elysia::WindowsSetup* windowsSetup_ = nullptr;
		//DirectXクラス
		Elysia::DirectXSetup* directXSetup_ = nullptr;
		//パイプライン管理クラス
		Elysia::PipelineManager* pipelineManager_ = nullptr;

	private:

		//頂点
		//リソース
		ComPtr<ID3D12Resource> vertexResource_ = nullptr;
		//データ
		VertexData* vertexData_ = nullptr;
		//バッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};

		//マテリアル
		//リソース
		ComPtr<ID3D12Resource> materialResource_ = nullptr;
		//データ
		MaterialData* materialData_ = nullptr;

		//TransformationMatrix
		//リソース
		ComPtr<ID3D12Resource> transformationMatrixResource_ = nullptr;
		//データ
		TransformationMatrix* transformationMatrixData_ = nullptr;

		//Index用
		ComPtr<ID3D12Resource> indexResource_ = nullptr;
		//バッファビュー
		D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};
		//インデックスデータ
		uint32_t* indexData_ = nullptr;

	private:

		/// <summary>
		/// 頂点の位置
		/// </summary>
		enum VertexPosition {
			//左下
			LeftBottom,
			//左上
			LeftTop,
			//右下
			RightBottom,
			//右上
			RightTop,

		};


	private:
		//UVトランスフォーム
		Transform uvTransform_ = {};

		//サイズ
		Vector2 size_ = {};
		//S
		Vector2 scale_ = { 1.0f,1.0f };
		//R
		float_t rotate_ = 0.0f;
		//T
		Vector2 position_ = {};

		//アンカーポイント
		Vector2 anchorPoint_ = {};
		//色
		Vector4 color_ = {};

		//左右フリップ
		bool isFlipX_ = false;
		//上下フリップ
		bool isFlipY_ = false;
		//非表示
		bool isInvisible_ = false;

		//手前にするか後ろにするか
		//基本手前にする
		bool isBack_ = false;

		//テクスチャ範囲設定
		Vector2 textureLeftTop_ = {};
		//テクスチャ切り出しサイズ
		Vector2 textureSize_ = { .x = 100.0f,.y = 100.0f };

		//UVの設定をするかどうか
		bool isUVSetting_ = false;

		//テクスチャハンドル
		uint32_t textureHandle_ = 0u;

	};
}

