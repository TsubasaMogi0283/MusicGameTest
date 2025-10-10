#include "NightBackTexture.h"

void NightBackTexture::Initialize() {
	//生成
	backTexture_ = std::make_unique<Elysia::BackTexture>();
	//色の設定
	const Vector4 SUNUSET_BACK_TEXTURE_COLOUR = { .x = 0.0f,.y = 0.0f,.z = 0.0f,.w = 1.0f };
	backTexture_->SetClearColour(SUNUSET_BACK_TEXTURE_COLOUR);
	//初期化
	backTexture_->Initialize();

}
