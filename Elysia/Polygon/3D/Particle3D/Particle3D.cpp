#include "Particle3D.h" 


#include <cassert>
#include <numbers>
#include <algorithm>

#include "Camera.h"
#include "TextureManager.h"
#include "PipelineManager.h"
#include "ModelManager.h"
#include "SrvManager.h"

#include "Material.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "VectorCalculation.h"


Elysia::Particle3D::Particle3D() {
	//インスタンスの取得
	//モデル管理
	modelManager_ = Elysia::ModelManager::GetInstance();
	//テクスチャ管理クラス
	textureManager_ = Elysia::TextureManager::GetInstance();
	//DirectXクラス
	directXSetup_ = Elysia::DirectXSetup::GetInstance();
	//SRV管理クラス
	srvManager_ = Elysia::SrvManager::GetInstance();
	//パイプライン管理クラス
	pipelineManager_ = Elysia::PipelineManager::GetInstance();

}

std::unique_ptr<Elysia::Particle3D> Elysia::Particle3D::Create(const uint32_t& moveType) {
	//生成
	std::unique_ptr<Elysia::Particle3D> particle3D = std::make_unique<Elysia::Particle3D>();

	//モデルの読み込み
	uint32_t modelHandle = particle3D->modelManager_->Load("Resources/Model/Particle", "ParticlePlane.obj");
	//テクスチャの読み込み
	particle3D->textureHandle_ = particle3D->textureManager_->Load("Resources/External/Texture/Circle/circle.png");
	//動きの種類
	particle3D->moveType_ = moveType;

	//頂点リソースを作る
	particle3D->vertices_ = particle3D->modelManager_->GetModelData(modelHandle).vertices;
	particle3D->vertexResource_ = particle3D->directXSetup_->CreateBufferResource(sizeof(VertexData) * particle3D->vertices_.size());
	//リソースの先頭のアドレスから使う
	particle3D->vertexBufferView_.BufferLocation = particle3D->vertexResource_->GetGPUVirtualAddress();
	//使用するリソースは頂点のサイズ
	particle3D->vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * particle3D->vertices_.size());
	//１頂点あたりのサイズ
	particle3D->vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点バッファにデータを書き込む
	VertexData* vertexData = nullptr;
	particle3D->vertexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, particle3D->vertices_.data(), sizeof(VertexData) * particle3D->vertices_.size());
	particle3D->vertexResource_->Unmap(0u, nullptr);

	//インスタンシング
	particle3D->instancingResource_ = particle3D->directXSetup_->CreateBufferResource(sizeof(ParticleForGPU) * particle3D->MAX_INSTANCE_NUMBER_);
	//インデックス
	particle3D->instancingIndex_ = particle3D->srvManager_->Allocate();
	//SRVの作成
	particle3D->srvManager_->CreateSRVForStructuredBuffer(particle3D->instancingIndex_, particle3D->instancingResource_.Get(), particle3D->MAX_INSTANCE_NUMBER_, sizeof(ParticleForGPU));
	//カメラ
	particle3D->cameraResource_ = particle3D->directXSetup_->CreateBufferResource(sizeof(Vector3));

	return particle3D;

}

std::unique_ptr<Elysia::Particle3D> Elysia::Particle3D::Create(const uint32_t& modelHandle, const uint32_t& moveType) {
	//生成
	std::unique_ptr<Elysia::Particle3D> particle3D = std::make_unique<Elysia::Particle3D>();

	//モデルの読み込み
	ModelData modelData = particle3D->modelManager_->GetModelData(modelHandle);

	//テクスチャの読み込み
	particle3D->textureHandle_ = particle3D->textureManager_->Load(modelData.textureFilePath);

	//動きの種類
	particle3D->moveType_ = moveType;

	//頂点リソースを作る
	particle3D->vertices_ = particle3D->modelManager_->GetModelData(modelHandle).vertices;
	particle3D->vertexResource_ = particle3D->directXSetup_->CreateBufferResource(sizeof(VertexData) * particle3D->vertices_.size());


	//リソースの先頭のアドレスから使う
	particle3D->vertexBufferView_.BufferLocation = particle3D->vertexResource_->GetGPUVirtualAddress();
	//使用するリソースは頂点のサイズ
	particle3D->vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * particle3D->vertices_.size());
	//１頂点あたりのサイズ
	particle3D->vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点バッファにデータを書き込む
	VertexData* vertexData = nullptr;
	particle3D->vertexResource_->Map(0u, nullptr, reinterpret_cast<void**>(&vertexData));//
	std::memcpy(vertexData, particle3D->vertices_.data(), sizeof(VertexData) * particle3D->vertices_.size());
	particle3D->vertexResource_->Unmap(0u, nullptr);

	//インスタンシング
	particle3D->instancingResource_ = particle3D->directXSetup_->CreateBufferResource(sizeof(ParticleForGPU) * particle3D->MAX_INSTANCE_NUMBER_);
	particle3D->instancingIndex_ = particle3D->srvManager_->Allocate();
	//SRVを作る
	particle3D->srvManager_->CreateSRVForStructuredBuffer(particle3D->instancingIndex_, particle3D->instancingResource_.Get(), particle3D->MAX_INSTANCE_NUMBER_, sizeof(ParticleForGPU));
	
	//カメラ
	particle3D->cameraResource_ = particle3D->directXSetup_->CreateBufferResource(sizeof(Vector3));

	return particle3D;

}

ParticleInformation Elysia::Particle3D::MakeNewParticle(std::mt19937& randomEngine) {

	//ランダムの値で位置を決める
	//SRは固定
	std::uniform_real_distribution<float_t> distribute(-1.0f, 1.0f);
	ParticleInformation particle;
	particle.transform.scale = emitter_.transform.scale;
	particle.transform.rotate = { .x = 0.0f,.y = 0.0f,.z = 0.0f };
	Vector3 randomTranslate = { .x = distribute(randomEngine),.y = distribute(randomEngine) + 1.0f,.z = distribute(randomEngine) };
	particle.transform.translate = VectorCalculation::Add(emitter_.transform.translate, randomTranslate);
	//投げ上げは少しだけ上にずらす
	if (moveType_ == ThrowUp) {
		Vector3 offset = { .x = randomTranslate.x,.y = 0.1f,.z = randomTranslate.z };
		particle.transform.translate = VectorCalculation::Add(emitter_.transform.translate, offset);

	}
	//初期トランスフォームを記録
	particle.initialTransform = particle.transform;
	//速度
	std::uniform_real_distribution<float_t>distVelocity(-1.0f, 1.0f);
	particle.velocity = { .x = distVelocity(randomEngine),.y = distVelocity(randomEngine),.z = distVelocity(randomEngine) };

	//色
	std::uniform_real_distribution<float_t> distColor(1.0f, 1.0f);
	particle.color = { .x = distColor(randomEngine),.y = distColor(randomEngine),.z = distColor(randomEngine),.w = 1.0f };

	//時間
	std::uniform_real_distribution<float_t> distTime(0.5f, 1.5f);
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0.0f;

	//線形補間用
	particle.absorbT = 0.0f;

	return particle;

}

std::list<ParticleInformation> Elysia::Particle3D::Emission(const Emitter& emmitter, std::mt19937& randomEngine) {
	std::list<ParticleInformation> particles;

	for (uint32_t count = 0u; count < emmitter.count; ++count) {
		//emmitterで設定したカウントまで増やしていくよ
		particles.push_back(MakeNewParticle(randomEngine));
	}

	return particles;
}

void Elysia::Particle3D::Update(const Camera& camera) {

	//ランダムエンジン
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
	//一度だけ出すモード
	if (isReleaseOnceMode_ == true) {
		//パーティクルを作る
		if (isReeasedOnce_ == false) {
			particles_.splice(particles_.end(), Emission(emitter_, randomEngine));
			isReeasedOnce_ = true;
		}
	}
	else {
		//最初に1回だけ出す
		if (isFirstRelease_ == false) {
			particles_.splice(particles_.end(), Emission(emitter_, randomEngine));
			//出し終わったことを示す
			isFirstRelease_ = true;

		}

		//そのあと循環
		//時間経過
		emitter_.frequencyTime += DELTA_TIME;
		//頻度より大きいかつまだ生成が停止されていない
		if (emitter_.frequency <= emitter_.frequencyTime && isStopGenerate_==false) {
			//パーティクルを作る
			particles_.splice(particles_.end(), Emission(emitter_, randomEngine));
			//出した
			isRelease_ = true;
			//もう一度最初から
			emitter_.frequencyTime=0.0f;
		}
		else {
			//出していない
			isRelease_ = false;
		}
	}


	//書き込み
	instancingResource_->Map(0u, nullptr, reinterpret_cast<void**>(&particleForGpuData_));


	//座標の計算など
	numInstance_ = 0u;
	for (std::list<ParticleInformation>::iterator particleIterator = particles_.begin();
		particleIterator != particles_.end(); ++particleIterator) {

		//行列の初期化
		Matrix4x4 worldMatrix = {};
		Matrix4x4 scaleMatrix = {};
		Matrix4x4 translateMatrix = {};
		Matrix4x4 billBoardMatrix = {};
		float_t accel = -0.001f;
		particleIterator->currentTime += DELTA_TIME;

		switch (moveType_) {
		case ParticleMoveType::NormalRelease:
#pragma region 通常の放出

			if (isReleaseOnceMode_ == false) {
				if (particleIterator->lifeTime <= particleIterator->currentTime) {

					continue;
				}
			}

			particleIterator->transform.translate.y += 0.0001f;


			//カメラの回転を適用する
			billBoardMatrix = camera.viewMatrix;
			//平行成分はいらないよ
			//あくまで回転だけ
			billBoardMatrix.m[3][0] = 0.0f;
			billBoardMatrix.m[3][1] = 0.0f;
			billBoardMatrix.m[3][2] = 0.0f;
			billBoardMatrix = Matrix4x4Calculation::Inverse(billBoardMatrix);
			//行列を作っていくよ
			//Scale
			scaleMatrix = Matrix4x4Calculation::MakeScaleMatrix(particleIterator->transform.scale);
			//座標
			translateMatrix = Matrix4x4Calculation::MakeTranslateMatrix(particleIterator->transform.translate);


			//パーティクル個別のRotateは関係ないよ
			//その代わりにさっき作ったbillBoardMatrixを入れるよ
			worldMatrix = Matrix4x4Calculation::Multiply(scaleMatrix, Matrix4x4Calculation::Multiply(billBoardMatrix, translateMatrix));

			//最大値を超えて描画しないようにする
			if (numInstance_ < MAX_INSTANCE_NUMBER_) {
				//ワールド行列と色のデータを設定
				particleForGpuData_[numInstance_].world = worldMatrix;
				const Vector4 COLOR = { .x = 1.0f,.y = 1.0f,.z = 1.0f,.w = 1.0f };
				particleForGpuData_[numInstance_].color = COLOR;
				particleIterator->color = COLOR;

				//透明になっていくようにするかどうか
				if (isToTransparent_ == true) {
					//アルファはVector4でのwだね
					float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);
					particleForGpuData_[numInstance_].color.w = alpha;
					particleIterator->color.w = alpha;
				}

			}

			break;


#pragma endregion

		case ParticleMoveType::ThrowUp:
#pragma region 鉛直投げ上げ
			throwUpVelocityY_ += accel;

			//加速を踏まえた位置計算
			particleIterator->transform.translate.x += particleIterator->velocity.x / 3.0f;
			particleIterator->transform.translate.y += throwUpVelocityY_;
			particleIterator->transform.translate.z += particleIterator->velocity.z / 3.0f;

			//カメラの回転を適用する
			billBoardMatrix = camera.viewMatrix;
			//平行成分はいらないよ
			//あくまで回転だけ
			billBoardMatrix.m[3][0] = 0.0f;
			billBoardMatrix.m[3][1] = 0.0f;
			billBoardMatrix.m[3][2] = 0.0f;
			billBoardMatrix = Matrix4x4Calculation::Inverse(billBoardMatrix);
			//行列を作っていくよ
			scaleMatrix = Matrix4x4Calculation::MakeScaleMatrix(particleIterator->transform.scale);
			translateMatrix = Matrix4x4Calculation::MakeTranslateMatrix(particleIterator->transform.translate);


			//パーティクル個別のRotateは関係ないよ
			//その代わりにさっき作ったbillBoardMatrixを入れるよ
			worldMatrix = Matrix4x4Calculation::Multiply(scaleMatrix, Matrix4x4Calculation::Multiply(billBoardMatrix, translateMatrix));

			//最大値を超えて描画しないようにする
			if (numInstance_ < MAX_INSTANCE_NUMBER_) {
				particleForGpuData_[numInstance_].world = worldMatrix;
				particleForGpuData_[numInstance_].color = particleIterator->color;

				//ワールド座標
				Vector3 worldPosition = {
					.x = worldMatrix.m[3][0],
					.y = worldMatrix.m[3][1],
					.z = worldMatrix.m[3][2]
				};

				//設定した地面の高さより小さくなったら透明
				if (worldPosition.y < groundOffset_) {
					//アルファはVector4でのwだね
					particleForGpuData_[numInstance_].color.w = 0.0f;

				}

				
			}
			break;

#pragma endregion

		case ParticleMoveType::Rise:
#pragma region 上昇
			//強制的にビルボードにするよ
			particleIterator->transform.translate.x += particleIterator->velocity.x / 15.0f;
			particleIterator->transform.translate.y += 0.03f;
			particleIterator->transform.translate.z += particleIterator->velocity.z / 15.0f;
			//カメラの回転を適用する
			billBoardMatrix = camera.viewMatrix;
			//平行成分はいらないよ
			//あくまで回転だけ
			billBoardMatrix.m[3][0] = 0.0f;
			billBoardMatrix.m[3][1] = 0.0f;
			billBoardMatrix.m[3][2] = 0.0f;
			billBoardMatrix = Matrix4x4Calculation::Inverse(billBoardMatrix);
			//行列を作っていくよ
			scaleMatrix = Matrix4x4Calculation::MakeScaleMatrix(particleIterator->transform.scale);
			translateMatrix = Matrix4x4Calculation::MakeTranslateMatrix(particleIterator->transform.translate);


			//パーティクル個別のRotateは関係ないよ
			//その代わりにさっき作ったbillBoardMatrixを入れるよ
			worldMatrix = Matrix4x4Calculation::Multiply(scaleMatrix, Matrix4x4Calculation::Multiply(billBoardMatrix, translateMatrix));

			//最大値を超えないようにする
			//そして生成停止までの処理
			if (numInstance_ < MAX_INSTANCE_NUMBER_ && isStopGenerate_ == false) {
				particleForGpuData_[numInstance_].world = worldMatrix;
				particleForGpuData_[numInstance_].color = particleIterator->color;

				//透明になっていくようにするかどうか
				if (isToTransparent_ == true) {
					//アルファはVector4でのwだね
					float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);
					particleIterator->color.w = alpha;
					particleForGpuData_[numInstance_].color.w = alpha;
				}

			}
#pragma endregion
			break;
		case ParticleMoveType::Absorb:
#pragma region 吸収

			//線形補間
			particleIterator->absorbT += T_INCREASE_VALUE_;
			
			//線形補間でやって綺麗に集まるようにする
			Vector3 newPosition = VectorCalculation::Lerp(VectorCalculation::Add(releasePositionForAbsorb_, particleIterator->transform.translate), absorbPosition_, particleIterator->absorbT);

			//カメラの回転を適用する
			billBoardMatrix = camera.viewMatrix;
			//平行成分はいらないよ
			//あくまで回転だけ
			billBoardMatrix.m[3][0] = 0.0f;
			billBoardMatrix.m[3][1] = 0.0f;
			billBoardMatrix.m[3][2] = 0.0f;
			billBoardMatrix = Matrix4x4Calculation::Inverse(billBoardMatrix);
			//行列を作っていくよ
			scaleMatrix = Matrix4x4Calculation::MakeScaleMatrix(particleIterator->transform.scale);
			translateMatrix = Matrix4x4Calculation::MakeTranslateMatrix(newPosition);

			//パーティクル個別のRotateは関係ないよ
			//その代わりにさっき作ったbillBoardMatrixを入れるよ
			worldMatrix = Matrix4x4Calculation::Multiply(scaleMatrix, Matrix4x4Calculation::Multiply(billBoardMatrix, translateMatrix));



			//最大値を超えないようにする
			//そして生成停止までの処理
			if (numInstance_ < MAX_INSTANCE_NUMBER_ && isStopGenerate_ == false) {
				particleForGpuData_[numInstance_].world = worldMatrix;
				particleForGpuData_[numInstance_].color = particleIterator->color;

				//透明になっていくようにするかどうか
				if (isToTransparent_ == true) {
					//どんどん透明にしていく
					float_t alpha = 1.0f - particleIterator->absorbT;
					particleIterator->color.w = alpha;
					particleForGpuData_[numInstance_].color.w = alpha;
				}

			}

#pragma endregion
			break;
		
		case ParticleMoveType::Stay:
#pragma region 滞留
			//カメラの回転を適用する
			billBoardMatrix = camera.viewMatrix;
			//平行成分はいらないよ
			//あくまで回転だけ
			billBoardMatrix.m[3][0] = 0.0f;
			billBoardMatrix.m[3][1] = 0.0f;
			billBoardMatrix.m[3][2] = 0.0f;
			billBoardMatrix = Matrix4x4Calculation::Inverse(billBoardMatrix);
			//行列を作っていくよ
			//拡縮
			scaleMatrix = Matrix4x4Calculation::MakeScaleMatrix(particleIterator->transform.scale);
			//座標
			translateMatrix = Matrix4x4Calculation::MakeTranslateMatrix(VectorCalculation::Add(particleIterator->transform.translate,trackingPosition_));
			//パーティクル個別のRotateは関係ないよ
			//その代わりにさっき作ったbillBoardMatrixを入れるよ
			worldMatrix = Matrix4x4Calculation::Multiply(Matrix4x4Calculation::Multiply(scaleMatrix, billBoardMatrix), translateMatrix);

			//最大値を超えないようにする
			//そして生成停止までの処理
			if (numInstance_ < MAX_INSTANCE_NUMBER_ && isStopGenerate_ == false) {
				particleForGpuData_[numInstance_].world = worldMatrix;
				particleForGpuData_[numInstance_].color = particleIterator->color;

				//透明になっていくようにするかどうか
				if (isToTransparent_ == true) {
					//どんどん透明にしていく
					float_t alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);
					particleIterator->color.w = alpha;
					particleForGpuData_[numInstance_].color.w = alpha;
				}

			}

#pragma endregion
			
			break;
		}

		//生成を止めた時
		if (isStopGenerate_ == true) {
			const float DECREASE_ALPHA_VALUE = 0.1f;
			particleForGpuData_[numInstance_].color.w -= DECREASE_ALPHA_VALUE;
		}
		
		//見えなくなった
		if (particleIterator->color.w <= 0.0f) {
			particleIterator->isInvisible = true;
		}

		//最後に加算をし次に回す
		++numInstance_;

	}


	instancingResource_->Unmap(0u, nullptr);

	//全て見えなくなったらisAllInvisible_がtrueになる
	if (isReeasedOnce_ == true) {
		//all_ofは中にある全ての要素が満たす時にtrueを返す
		//今回の場合はparticles_にあるisInvisibleが全てtrueに鳴ったらtrueを返すという仕組みになっている
		isAllInvisible_ = std::all_of(particles_.begin(), particles_.end(), [](const ParticleInformation& particle) {
			return particle.isInvisible == true;
			});
	}
}

void Elysia::Particle3D::Draw(const Camera& camera, const Material& material) {

	assert(material.lightingKinds == LightingType::NoneLighting);
	//更新
	Update(camera);

	//PS用のカメラ
	cameraResource_->Map(0u, nullptr, reinterpret_cast<void**>(&cameraPositionData_));
	*cameraPositionData_ = camera.GetWorldPosition();
	cameraResource_->Unmap(0u, nullptr);

	//コマンドを積む
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetParticle3DRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetParticle3DGraphicsPipelineState().Get());
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXSetup_->GetCommandList()->IASetVertexBuffers(0u, 1u, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//CBVを設定する
	//マテリアル
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(0u, material.resource->GetGPUVirtualAddress());
	//インスタンシング
	srvManager_->SetGraphicsRootDescriptorTable(1u, instancingIndex_);
	//テクスチャ
	if (textureHandle_ != 0u) {
		textureManager_->GraphicsCommand(2u, textureHandle_);
	}
	//カメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(3u, camera.resource->GetGPUVirtualAddress());
	//PS用のカメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(5u, cameraResource_->GetGPUVirtualAddress());
	//DrawCall
	directXSetup_->GetCommandList()->DrawInstanced(UINT(vertices_.size()), numInstance_, 0u, 0u);
}

void Elysia::Particle3D::Draw(const Camera& camera, const  Material& material, const DirectionalLight& directionalLight) {

	//Directionalではなかったらassert
	if (material.lightingKinds != LightingType::DirectionalLighting) {
		assert(0);
	}

	//更新
	Update(camera);

	//PS用のカメラ
	cameraResource_->Map(0u, nullptr, reinterpret_cast<void**>(&cameraPositionData_));
	*cameraPositionData_ = camera.GetWorldPosition();
	cameraResource_->Unmap(0u, nullptr);

	//コマンドを積む
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetParticle3DRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetParticle3DGraphicsPipelineState().Get());
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXSetup_->GetCommandList()->IASetVertexBuffers(0u, 1u, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//CBVを設定する
	//マテリアル
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(0u, material.resource->GetGPUVirtualAddress());
	//インスタンシング
	srvManager_->SetGraphicsRootDescriptorTable(1u, instancingIndex_);
	//テクスチャ
	if (textureHandle_ != 0u) {
		textureManager_->GraphicsCommand(2u, textureHandle_);
	}
	//カメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(3u, camera.resource->GetGPUVirtualAddress());
	//平行光源
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(4u, directionalLight.resource->GetGPUVirtualAddress());
	//PS用のカメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(5u, cameraResource_->GetGPUVirtualAddress());

	//DrawCall
	directXSetup_->GetCommandList()->DrawInstanced(UINT(vertices_.size()), numInstance_, 0u, 0u);

}

void Elysia::Particle3D::Draw(const Camera& camera, const Material& material, const PointLight& pointLight) {
	//Pointではなかったらassert
	if (material.lightingKinds != LightingType::PointLighting) {
		assert(0);
	}

	//更新
	Update(camera);

	//PS用のカメラ
	cameraResource_->Map(0u, nullptr, reinterpret_cast<void**>(&cameraPositionData_));
	*cameraPositionData_ = camera.GetWorldPosition();
	cameraResource_->Unmap(0u, nullptr);
	//コマンドを積む
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetParticle3DRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetParticle3DGraphicsPipelineState().Get());


	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXSetup_->GetCommandList()->IASetVertexBuffers(0u, 1u, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//CBVを設定する
	//マテリアル
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(0u, material.resource->GetGPUVirtualAddress());
	//インスタンシング
	srvManager_->SetGraphicsRootDescriptorTable(1u, instancingIndex_);
	//テクスチャ
	if (textureHandle_ != 0u) {
		textureManager_->GraphicsCommand(2u, textureHandle_);
	}

	//カメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(3u, camera.resource->GetGPUVirtualAddress());
	//PS用のカメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(5u, cameraResource_->GetGPUVirtualAddress());
	//点光源
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(6u, pointLight.resource->GetGPUVirtualAddress());
	//DrawCall
	directXSetup_->GetCommandList()->DrawInstanced(UINT(vertices_.size()), numInstance_, 0u, 0u);

}

void Elysia::Particle3D::Draw(const Camera& camera, const Material& material, const SpotLight& spotLight) {
	//Spotではなかったらassert
	if (material.lightingKinds != LightingType::SpotLighting) {
		assert(0);
	}

	//更新
	Update(camera);

	//PS用のカメラ
	cameraResource_->Map(0u, nullptr, reinterpret_cast<void**>(&cameraPositionData_));
	*cameraPositionData_ = camera.GetWorldPosition();
	cameraResource_->Unmap(0u, nullptr);


	//コマンドを積む
	directXSetup_->GetCommandList()->SetGraphicsRootSignature(pipelineManager_->GetParticle3DRootSignature().Get());
	directXSetup_->GetCommandList()->SetPipelineState(pipelineManager_->GetParticle3DGraphicsPipelineState().Get());
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	directXSetup_->GetCommandList()->IASetVertexBuffers(0u, 1u, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	directXSetup_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//CBVを設定する
	//マテリアル
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(0u, material.resource->GetGPUVirtualAddress());
	//インスタンシング
	srvManager_->SetGraphicsRootDescriptorTable(1u, instancingIndex_);
	//テクスチャ
	if (textureHandle_ != 0u) {
		textureManager_->GraphicsCommand(2u, textureHandle_);
	}
	//カメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(3u, camera.resource->GetGPUVirtualAddress());
	//PS用のカメラ
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(5u, cameraResource_->GetGPUVirtualAddress());
	//SpotLight
	directXSetup_->GetCommandList()->SetGraphicsRootConstantBufferView(7u, spotLight.resource->GetGPUVirtualAddress());
	//DrawCall
	directXSetup_->GetCommandList()->DrawInstanced(UINT(vertices_.size()), numInstance_, 0u, 0u);

}

