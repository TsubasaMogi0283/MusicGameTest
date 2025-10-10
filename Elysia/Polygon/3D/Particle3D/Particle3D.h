#pragma once

/**
 * @file Particle3Dr.h
 * @brief パーティクル(3D版)のクラス
 * @author 茂木翼
 */


#include <random>
#include <list>
#include <string>
#include <map>

#include "Camera.h"
#include "Particle.h"
#include "AccelerationField.h"
#include "TransformationMatrix.h"
#include "Matrix4x4Calculation.h"
#include "VertexData.h"
#include "DirectXSetup.h"
#include "Emitter.h"
#include "ParticleMoveType.h"

#pragma region 前方宣言

 /// <summary>
 /// マテリアル
 /// </summary>
struct Material;

/// <summary>
/// 平行光源
/// </summary>
struct DirectionalLight;

/// <summary>
/// 点光源
/// </summary>
struct PointLight;

/// <summary>
/// スポットライト
/// </summary>
struct SpotLight;


#pragma endregion



/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {
	/// <summary>
	/// DirectXクラス
	/// </summary>
	class DirectSetup;

	/// <summary>
	/// SRV管理クラス
	/// </summary>
	class SrvManager;

	/// <summary>
	/// パイプライン管理クラス
	/// </summary>
	class PipelineManager;

	/// <summary>
	/// テクスチャ管理クラス
	/// </summary>
	class TextureManager;

	/// <summary>
	/// モデル管理クラス
	/// </summary>
	class ModelManager;


	/// <summary>
	/// パーティクル(3D)
	/// </summary>
	class Particle3D {
	public:

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Particle3D();

		/// <summary>
		/// 生成
		/// </summary>
		/// <param name="moveType">動きの種類</param>
		/// <returns>パーティクル(3D)</returns>
		static std::unique_ptr<Particle3D> Create(const uint32_t& moveType);

		/// <summary>
		/// 生成
		/// </summary>
		/// <param name="modelHandle">モデルハンドル</param>
		/// <param name="moveType">動きの種類</param>
		/// <returns>パーティクル(3D)</returns>
		static std::unique_ptr<Particle3D> Create(const uint32_t& modelHandle, const uint32_t& moveType);


	private:

		/// <summary>
		/// 新しいパーティクルの生成
		/// </summary>
		/// <param name="randomEngine">ランダムエンジン</param>
		/// <returns></returns>
		ParticleInformation MakeNewParticle(std::mt19937& randomEngine);

		/// <summary>
		/// Emitterで発生させる
		/// </summary>
		/// <param name="emmitter">エミッター</param>
		/// <param name="randomEngine">ランダムエンジン</param>
		/// <returns></returns>
		std::list<ParticleInformation> Emission(const Emitter& emmitter, std::mt19937& randomEngine);

		/// <summary>
		/// 更新
		/// </summary>
		/// <param name="camera">カメラ</param>
		void Update(const Camera& camera);

	public:

		/// <summary>
		/// 通常描画
		/// </summary>
		/// <param name="camera">カメラ</param>
		/// <param name="material">マテリアル</param>
		void Draw(const Camera& camera, const Material& material);

		/// <summary>
		/// 描画(平行光源)
		/// </summary>
		/// <param name="camera">カメラ</param>
		/// <param name="material">マテリアル</param>
		/// <param name="directionalLight">平行光源</param>
		void Draw(const Camera& camera, const Material& material, const DirectionalLight& directionalLight);

		/// <summary>
		/// 描画(点光源)
		/// </summary>
		/// <param name="camera">カメラ</param>
		/// <param name="material">マテリアル</param>
		/// <param name="pointLight">点光源</param>
		void Draw(const Camera& camera, const Material& material, const PointLight& pointLight);

		/// <summary>
		/// 描画(スポットライト)
		/// </summary>
		/// <param name="camera">カメラ</param>
		/// <param name="material">マテリアル</param>
		/// <param name="spotLight">スポットライト</param>
		void Draw(const Camera& camera, const Material& material, const SpotLight& spotLight);

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Particle3D() = default;

	public:
		/// <summary>
		/// 放出したかどうか
		/// </summary>
		inline bool GetIsRelease()const {
			return isRelease_;
		}


		/// <summary>
		///	一度だけ放出するかどうか
		/// </summary>
		/// <param name="isReleaseOnce">一度にするかどうかの設定</param>
		inline void SetIsReleaseOnceMode(const bool& isReleaseOnce) {
			this->isReleaseOnceMode_ = isReleaseOnce;
		}

		/// <summary>
		/// 生成を止めるかどうか
		/// </summary>
		/// <param name="isStop">止めるかどうか</param>
		inline void SetIsStopGenerate(const bool& isStop) {
			this->isStopGenerate_ = isStop;
		}

		/// <summary>
		/// 透明になっていくようにするかどうか
		/// </summary>
		/// <param name="isToTransparent">透明にしていくか</param>
		inline void SetIsToTransparent(const bool& isToTransparent) {
			this->isToTransparent_ = isToTransparent;
		}

		/// <summary>
		/// 全て透明になったかどうか
		/// </summary>
		/// <returns>全て消えたことを示すフラグ</returns>
		inline bool GetIsAllInvisible()const {
			return isAllInvisible_;
		}

		/// <summary>
		/// 鉛直投げ上げのY方向のベロシティを設定
		/// </summary>
		/// <param name="velocityY">Yのベロシティ</param>
		inline void SetThrowUpVeloityY(const float_t& velocityY) {
			this->throwUpVelocityY_ = velocityY;
		}

		/// <summary>
		/// 地面のオフセット
		/// </summary>
		/// <param name="offset">オフセット</param>
		inline void SetGroundOffset(const float_t& offset) {
			this->groundOffset_ = offset;
		}

		/// <summary>
		/// 吸収し集まる座標を設定する
		/// </summary>
		/// <param name="position">座標</param>
		inline void SetAbsorbPosition(const Vector3& position) {
			this->absorbPosition_ = position;
		}

		/// <summary>
		/// テクスチャの上書き
		/// </summary>
		/// <param name="textureHandle">テクスチャハンドル</param>
		inline void TextureOverride(const uint32_t& textureHandle) {
			this->textureHandle_ = textureHandle;
		}

		/// <summary>
		/// 吸収用の発生座標を設定
		/// </summary>
		/// <param name="position">座標</param>
		inline void SetReleasePositionForAbsorb(const Vector3& position) {
			this->releasePositionForAbsorb_ = position;
		}

		/// <summary>
		/// 追従座標の設定
		/// </summary>
		/// <param name="position"></param>
		inline void SetTrackingPosition(const Vector3& position) {
			this->trackingPosition_ = position;
		}

#pragma region エミッタの中の設定


		/// <summary>
		/// スケールの設定
		/// </summary>
		/// <param name="scale">スケール</param>
		inline void SetScale(const Vector3& scale) {
			this->emitter_.transform.scale = scale;
		}

		/// <summary>
		/// 回転の設定
		/// </summary>
		/// <param name="rotate">回転</param>
		inline void SetRotate(const Vector3& rotate) {
			this->emitter_.transform.rotate = rotate;
		}
		/// <summary>
		/// 回転の取得
		/// </summary>
		/// <returns>回転</returns>
		inline Vector3 GetRotate() const {
			return emitter_.transform.rotate;
		}

		/// <summary>
		/// 座標の設定
		/// </summary>
		/// <param name="translate">座標</param>
		inline void SetTranslate(const Vector3& translate) {
			this->emitter_.transform.translate = translate;
		}
		/// <summary>
		/// 座標の取得
		/// </summary>
		/// <returns>座標</returns>
		inline Vector3 GetTranslate() const {
			return emitter_.transform.translate;
		}


		/// <summary>
		/// 発生数
		/// </summary>
		/// <param name="count">数</param>
		inline void SetCount(const uint32_t& count) {
			this->emitter_.count = count;
		}

		/// <summary>
		/// 発生頻度
		/// </summary>
		/// <param name="frequency">頻度</param>
		inline void SetFrequency(const float_t& frequency) {
			this->emitter_.frequency = frequency;
		}

		/// <summary>
		/// 発生頻度を設定
		/// </summary>
		/// <param name="frequencyTime">発生頻度</param>
		inline void SetFrequencyTime(const float_t& frequencyTime) {
			this->emitter_.frequencyTime = frequencyTime;
		}




#pragma endregion

	private:

		//モデル管理クラス
		Elysia::ModelManager* modelManager_ = nullptr;
		//テクスチャ管理クラス
		Elysia::TextureManager* textureManager_ = nullptr;
		//DirectXクラス
		Elysia::DirectXSetup* directXSetup_ = nullptr;
		//SRV管理クラス
		Elysia::SrvManager* srvManager_ = nullptr;
		//パイプライン管理クラス
		Elysia::PipelineManager* pipelineManager_ = nullptr;

	private:
		//時間変化
		const float_t DELTA_TIME = 1.0f / 60.0f;
		//線形補間で増える値
		const float_t T_INCREASE_VALUE_ = 0.01f;
	private:

		//頂点リソースを作る
		ComPtr<ID3D12Resource> vertexResource_ = nullptr;
		//頂点バッファビューを作成する
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
		//頂点データ
		std::vector<VertexData> vertices_ = {};

		//カメラ
		//リソース
		ComPtr<ID3D12Resource>cameraResource_ = nullptr;
		//カメラデータ
		Vector3* cameraPositionData_ = {};
		//座標
		Vector3 cameraPosition_ = {};


		//最大数
		const uint32_t MAX_INSTANCE_NUMBER_ = 1000u;
		//描画すべきインスタンス数
		uint32_t numInstance_ = 0u;
		//インスタンスのインデックス
		int instancingIndex_ = 0;
		//インスタンスリソース
		ComPtr<ID3D12Resource> instancingResource_ = nullptr;
		//パーティクル
		std::list<ParticleInformation>particles_;
		//パーティクルデータ
		ParticleForGPU* particleForGpuData_ = nullptr;

		//エミッタの設定
		Emitter emitter_ = {};

		//テクスチャハンドル
		uint32_t textureHandle_ = 0u;
		//動きの種類
		uint32_t moveType_ = ParticleMoveType::NormalRelease;

		//透明になっていくか
		bool isToTransparent_ = true;
		//全て透明になったかどうか
		bool isAllInvisible_ = false;

		//鉛直投げ上げ
		float_t throwUpVelocityY_ = 1.2f;
		//地面の高さ設定
		float_t groundOffset_ = 0.0f;
		//最初に放出する
		bool isFirstRelease_ = false;
		//放出したかどうか
		bool isRelease_ = false;
		//一度だけ放出するかどうか
		bool isReleaseOnceMode_ = true;
		//出し終えたかどうか
		bool isReeasedOnce_ = false;
		//生成を止めるかどうか
		bool isStopGenerate_ = false;

		
		//吸収し集まる場所
		Vector3 absorbPosition_ = {};
		//吸収用の発生座標
		Vector3 releasePositionForAbsorb_ = {};
		//追従座標
		Vector3 trackingPosition_ = {};
	};

};




