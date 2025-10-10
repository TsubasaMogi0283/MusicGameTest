#include "BaseObjectForLevelEditor.h"

void BaseObjectForLevelEditor::Draw(const Camera& camera){
	//ライティング無しに設定
	material_.lightingKinds = LightingType::NoneLighting;
	//変更したのでここで更新させる
	material_.Update();
	//モデルの描画
	model_->Draw(worldTransform_, camera, material_);
}

void BaseObjectForLevelEditor::Draw(const Camera& camera, const DirectionalLight& directionalLight){
	//平行光源に設定
	material_.lightingKinds = LightingType::DirectionalLighting;
	//変更したのでここで更新させる
	material_.Update();
	//モデルの描画
	model_->Draw(worldTransform_, camera, material_, directionalLight);
}

void BaseObjectForLevelEditor::Draw(const Camera& camera, const PointLight& pointLight){
	//点光源に設定
	material_.lightingKinds = LightingType::PointLighting;
	//変更したのでここで更新させる
	material_.Update();
	//モデルの描画
	model_->Draw(worldTransform_, camera, material_, pointLight);
}

void BaseObjectForLevelEditor::Draw(const Camera& camera, const SpotLight& spotLight){
	//スポットライトに設定
	material_.lightingKinds = LightingType::SpotLighting;
	//変更したのでここで更新させる
	material_.Update();
	//モデルの描画
	model_->Draw(worldTransform_, camera, material_, spotLight);
}
