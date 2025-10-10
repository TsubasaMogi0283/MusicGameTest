#include "Material.h"
#include "Matrix4x4Calculation.h"

void Material::Initialize(){
	//色
	color = {.x = 1.0f,.y = 1.0f,.z = 1.0f,.w = 1.0f};
	//ライティングの種類
	lightingKinds = DirectionalLighting;
	//UV行列
	uvTransform= Matrix4x4Calculation::MakeIdentity4x4();
	//輝度
	shininess=1000.0f;
	//環境光
	ambientIntensity = 0.1f;
	//環境マップ
	isEnviromentMap = false;
	//リソースを生成
	resource = Elysia::DirectXSetup::GetInstance()->CreateBufferResource(sizeof(MaterialData));

}

void Material::Update(){

	//書き込み
	resource->Map(0u, nullptr, reinterpret_cast<void**>(&materialData_));
	//色
	materialData_->color = color;
	//ライティングの種類
	materialData_->lightingKinds = lightingKinds;
	//UVトランスフォーム
	materialData_->uvTransform = uvTransform;
	//輝度
	materialData_->shininess = shininess;
	//環境光
	materialData_->ambientIntensity = ambientIntensity;

	//環境マップ
	materialData_->isEnviromentMap = isEnviromentMap;
	//書き込み終了
	resource->Unmap(0u, nullptr);
}
