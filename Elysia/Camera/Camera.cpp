#include "Camera.h"
#include "Matrix4x4Calculation.h"
#include "WindowsSetup.h"

void Camera::Initialize() {
	//リソース作成
	resource = Elysia::DirectXSetup::GetInstance()->CreateBufferResource(sizeof(CameraMatrixData)).Get();

	//アスペクト比
	aspectRatio = static_cast<float_t>(Elysia::WindowsSetup::GetInstance()->GetClientWidth()) / static_cast<float_t>(Elysia::WindowsSetup::GetInstance()->GetClientHeight());
	
	//初期
	scale = {.x= 1.0f,.y= 1.0f,.z= 1.0f };
	rotate = {.x= 0.0f,.y= 0.0f,.z= 0.0f };
	translate = {.x= 0.0f,.y= 0.0f,.z= -9.8f };

	//ワールド行列を計算
	worldMatrix = Matrix4x4Calculation::MakeAffineMatrix(scale, rotate, translate);
	//逆行列を計算
	viewMatrix = Matrix4x4Calculation::Inverse(worldMatrix);
	//射影を計算
	projectionMatrix = Matrix4x4Calculation::MakePerspectiveFovMatrix(fov_, aspectRatio, nearClip, farClip);
	//正射影行列(正規化)を計算
	orthographicMatrix = Matrix4x4Calculation::MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float_t>(Elysia::WindowsSetup::GetInstance()->GetClientWidth()), static_cast<float_t>(Elysia::WindowsSetup::GetInstance()->GetClientHeight()), 0.0f, 100.0f);
}


void Camera::Update() {
	//ワールド行列を計算
	worldMatrix = Matrix4x4Calculation::MakeAffineMatrix(scale, rotate, translate);
	//逆行列を計算
	viewMatrix = Matrix4x4Calculation::Inverse(worldMatrix);
	//射影を計算
	projectionMatrix = Matrix4x4Calculation::MakePerspectiveFovMatrix(fov_, aspectRatio, nearClip, farClip);
	//正射影行列(正規化)を計算
	orthographicMatrix = Matrix4x4Calculation::MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float_t>(Elysia::WindowsSetup::GetInstance()->GetClientWidth()), static_cast<float_t>(Elysia::WindowsSetup::GetInstance()->GetClientHeight()), 0.0f, 100.0f);

	//転送
	Transfer();
}

void Camera::Transfer() {
	//それぞれにデータを書き込む
	resource->Map(0u, nullptr, reinterpret_cast<void**>(&cameraMatrixData));
	cameraMatrixData->viewMatrix_ = viewMatrix;
	cameraMatrixData->projectionMatrix_ = projectionMatrix;
	cameraMatrixData->orthographicMatrix_ = orthographicMatrix;
	resource->Unmap(0u, nullptr);
}